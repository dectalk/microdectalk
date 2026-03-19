package dev.bytesizedfox.dectalkttsmod;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.concurrent.ConcurrentLinkedQueue;

public class TTSManager {
    private static final Logger LOGGER = LogManager.getLogger();

    private final ConcurrentLinkedQueue<String> messageQueue = new ConcurrentLinkedQueue<>();
    private final TTSMixer mixer = new TTSMixer();

    private volatile boolean initialized = false;

    public TTSManager() {
        LOGGER.info("TTSManager created");
    }

    private void ensureInitialized() {
        if (!initialized && TTSNative.isLoaded()) {
            LOGGER.info("Initializing TTS on thread: {}", Thread.currentThread().getName());
            TTSNative.init();
            mixer.start();
            initialized = true;
        }
    }

    public void speak(String text) {
        if (!TTSNative.isLoaded()) {
            return;
        }

        LOGGER.info("Queueing: '{}'", text);
        messageQueue.offer(text);
    }

    public void tick() {
        if (!TTSNative.isLoaded()) {
            return;
        }

        ensureInitialized();

        String message = messageQueue.poll();
        if (message == null) {
            return;
        }

        LOGGER.info("Synthesizing: '{}'", message);

        synchronized (this) {  // DECtalk engine is not thread-safe
            TTSNative.reset();
            TTSNative.speak("[:rate 180]" + message);
            TTSNative.sync();

            int totalSamples = TTSNative.getAvailableSamples();
            LOGGER.info("Got {} samples for '{}'", totalSamples, message);

            if (totalSamples == 0) {
                LOGGER.warn("No audio generated");
                return;
            }

            short[] audioData = new short[totalSamples];
            int copied = TTSNative.readSamples(audioData, totalSamples);
            LOGGER.info("Copied {} samples", copied);

            mixer.addStream(audioData);
        }
    }

    /** Stop all current playback and discard pending messages, but keep the manager alive. */
    public void clear() {
        messageQueue.clear();
        mixer.clearAll();
    }

    public void stop() {
        messageQueue.clear();
        mixer.stop();

        if (initialized) {
            TTSNative.reset();
        }
    }

    public boolean isSpeaking() {
        return mixer.isActive();
    }
}
