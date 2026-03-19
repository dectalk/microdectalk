package dev.bytesizedfox.dectalkttsmod;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.sound.sampled.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Single-line audio mixer for TTS playback.
 * Opens one SourceDataLine and keeps it open for the lifetime of the manager.
 * Multiple synthesized utterances are mixed together sample-by-sample so they
 * can overlap without opening additional audio lines.
 */
public class TTSMixer {
    private static final Logger LOGGER = LogManager.getLogger();
    private static final AudioFormat FORMAT = new AudioFormat(11025f, 16, 1, true, false);

    // 512 frames ≈ 46ms at 11025 Hz — small enough for low latency,
    // large enough to avoid hammering write() in a tight loop.
    private static final int CHUNK_FRAMES = 512;

    private final ConcurrentLinkedQueue<short[]> pending = new ConcurrentLinkedQueue<>();
    private final List<ActiveStream> active = new ArrayList<>();

    private SourceDataLine line;
    private volatile boolean running;
    private volatile boolean clearRequested;
    private Thread mixerThread;

    private static class ActiveStream {
        final short[] data;
        int pos;

        ActiveStream(short[] data) {
            this.data = data;
        }

        boolean done() {
            return pos >= data.length;
        }
    }

    public void start() {
        try {
            DataLine.Info info = new DataLine.Info(SourceDataLine.class, FORMAT);
            line = (SourceDataLine) AudioSystem.getLine(info);
            // 8 chunks of internal buffer keeps the line fed without excess latency
            line.open(FORMAT, CHUNK_FRAMES * 2 * 8);
            line.start();
        } catch (LineUnavailableException e) {
            LOGGER.error("Failed to open audio line for TTS mixer", e);
            return;
        }

        running = true;
        mixerThread = new Thread(this::mixerLoop, "TTS-Mixer");
        mixerThread.setDaemon(true);
        mixerThread.start();
        LOGGER.info("TTS mixer started");
    }

    /** Queue a fully-synthesized audio buffer for playback. Thread-safe. */
    public void addStream(short[] samples) {
        pending.offer(samples);
    }

    public boolean isActive() {
        return !pending.isEmpty() || !active.isEmpty();
    }

    private void mixerLoop() {
        short[] mixBuf = new short[CHUNK_FRAMES];
        byte[] outBytes = new byte[CHUNK_FRAMES * 2];

        while (running) {
            // Handle a clear request: discard everything on the mixer thread to avoid races
            if (clearRequested) {
                clearRequested = false;
                active.clear();
                line.flush();
            }

            // Drain newly-synthesized buffers into the active list
            short[] incoming;
            while ((incoming = pending.poll()) != null) {
                active.add(new ActiveStream(incoming));
            }

            if (active.isEmpty()) {
                // Nothing to play; sleep briefly rather than busy-spin
                try {
                    Thread.sleep(5);
                } catch (InterruptedException e) {
                    break;
                }
                continue;
            }

            // Mix all active streams into mixBuf
            Arrays.fill(mixBuf, (short) 0);
            float volume = (float) TTSConfig.getVolume();
            Iterator<ActiveStream> it = active.iterator();
            while (it.hasNext()) {
                ActiveStream s = it.next();
                for (int i = 0; i < CHUNK_FRAMES; i++) {
                    if (s.done()) break;
                    // Accumulate with saturation to avoid wrap-around distortion
                    int mixed = mixBuf[i] + (int) (s.data[s.pos++] * volume);
                    if (mixed > Short.MAX_VALUE) mixed = Short.MAX_VALUE;
                    else if (mixed < Short.MIN_VALUE) mixed = Short.MIN_VALUE;
                    mixBuf[i] = (short) mixed;
                }
                if (s.done()) it.remove();
            }

            // Serialize to little-endian PCM bytes
            for (int i = 0; i < CHUNK_FRAMES; i++) {
                short s = mixBuf[i];
                outBytes[i * 2]     = (byte) (s & 0xFF);
                outBytes[i * 2 + 1] = (byte) ((s >> 8) & 0xFF);
            }

            // write() blocks when the line buffer is full, naturally rate-limiting the loop
            line.write(outBytes, 0, outBytes.length);
        }

        line.drain();
        line.stop();
        line.close();
    }

    /**
     * Stop all currently-playing audio and discard queued buffers, but keep the mixer running.
     * Safe to call from any thread.
     */
    public void clearAll() {
        // Discard anything not yet promoted to the active list
        pending.clear();
        // Signal the mixer thread to clear its active list and flush the line buffer.
        // We don't touch `active` directly since it is owned by the mixer thread.
        clearRequested = true;
    }

    public void stop() {
        running = false;
        pending.clear();
        if (mixerThread != null) {
            mixerThread.interrupt();
        }
    }
}
