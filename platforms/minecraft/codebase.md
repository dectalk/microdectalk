# DECtalkTTSMod — Full Source

## `src/main/native/epsonapi.h`

```h
#ifndef _EPSONAPI_H
#define _EPSONAPI_H
//Sample rate selection
#ifdef WAVE_FORMAT_1M16
#undef WAVE_FORMAT_1M16
#endif
#ifdef WAVE_FORMAT_08M16
#undef WAVE_FORMAT_08M16
#endif

#define WAVE_FORMAT_1M16  0x0001	// 11Khz Output (71 samles per buffer)
#define WAVE_FORMAT_08M16 0x0002	// 8Khz Output (51 samples per buffer)

#define ERR_NOERROR		0	// no errors
#define ERR_ERROR		1	// random error
#define ERR_RESET		2	// Start returned early due to reset
#define ERR_INDEX		3	// Data in callback is index

#ifdef EPSON_ARM7
#define ERR_PHONEME_BUF_TOO_SHORT 4 //phoneme buffer is full and there is more to go in it
#endif

#define PTS_DONE		0
#define PTS_OUTPUT		5

extern int TextToSpeechStart(char *input,short *buffer_deprecated,int output_format);
extern int TextToSpeechInit(short *(*callback)(short *,long, int),void *user_dict);
extern int TextToSpeechReset(void);
extern int TextToSpeechSync();

#endif //_EPSONAPI_H

```

## `src/main/native/TTSNative.c`

```c
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include "epsonapi.h"

#define MAX_BUFFER (10 * 60) * 11025
static short samples[MAX_BUFFER];
static int total_size = 0;
static int halting = 0;

short *audio_callback(short *iwave, long length, int phoneme) {
    if (halting) {
        return NULL;
    }
    
    // Check if we have room
    if (total_size + length > MAX_BUFFER) {
        // Buffer full, drop samples
        return NULL;
    }
    
    // Append to buffer
    for (int i = 0; i < length; i++) {
        samples[total_size + i] = iwave[i];
    }
    total_size += length;
    
    return NULL;
}

JNIEXPORT jboolean JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_init
  (JNIEnv *env, jclass cls) {
    total_size = 0;
    halting = 0;
    int result = TextToSpeechInit(audio_callback, NULL);
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_speak
  (JNIEnv *env, jclass cls, jstring text) {
    const char *native_text = (*env)->GetStringUTFChars(env, text, NULL);
    
    // Reset buffer for new synthesis
    total_size = 0;
    halting = 0;
    
    int result = TextToSpeechStart((char *)native_text, NULL, WAVE_FORMAT_1M16);
    
    (*env)->ReleaseStringUTFChars(env, text, native_text);
    
    return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_sync
  (JNIEnv *env, jclass cls) {
    TextToSpeechSync();
}

JNIEXPORT void JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_reset
  (JNIEnv *env, jclass cls) {
    halting = 1;
    TextToSpeechStart("", NULL, WAVE_FORMAT_1M16);
    total_size = 0;
}

JNIEXPORT jint JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_getAvailableSamples
  (JNIEnv *env, jclass cls) {
    return total_size;
}

JNIEXPORT jint JNICALL Java_dev_bytesizedfox_dectalkttsmod_TTSNative_readSamples
  (JNIEnv *env, jclass cls, jshortArray output, jint count) {
    int samples_to_read = (count < total_size) ? count : total_size;
    
    if (samples_to_read <= 0) {
        return 0;
    }
    
    jshort *native_output = (*env)->GetShortArrayElements(env, output, NULL);
    
    // Copy from beginning of buffer
    memcpy(native_output, samples, samples_to_read * sizeof(short));
    
    (*env)->ReleaseShortArrayElements(env, output, native_output, 0);
    
    return samples_to_read;
}
```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/Dectalkttsmod.java`

```java
package dev.bytesizedfox.dectalkttsmod;

import net.minecraftforge.common.MinecraftForge;
import net.minecraftforge.fml.common.Mod;
import net.minecraftforge.fml.event.lifecycle.FMLClientSetupEvent;
import net.minecraftforge.fml.event.lifecycle.FMLLoadCompleteEvent;
import net.minecraftforge.fml.javafmlmod.FMLJavaModLoadingContext;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

@Mod(Dectalkttsmod.MODID)
public class Dectalkttsmod {
    public static final String MODID = "dectalkttsmod";
    private static final Logger LOGGER = LogManager.getLogger();

    public static ChatTTSHandler chatHandler;


    public Dectalkttsmod() {
        TTSConfig.load();

        var modBus = FMLJavaModLoadingContext.get().getModEventBus();
        modBus.addListener(this::clientSetup);
        modBus.addListener(this::loadComplete);
        modBus.addListener(TTSKeyBindings::register);
    }

    private void clientSetup(FMLClientSetupEvent event) {
        if (!TTSNative.isLoaded()) {
            LOGGER.error("Failed to load TTS native library - mod disabled");
            return;
        }

        LOGGER.info("TTS native library loaded successfully");
    }

    private void loadComplete(FMLLoadCompleteEvent event) {
        event.enqueueWork(() -> {
            if (!TTSNative.isLoaded()) {
                return;
            }

            chatHandler = new ChatTTSHandler();
            MinecraftForge.EVENT_BUS.register(chatHandler);
            MinecraftForge.EVENT_BUS.register(TTSKeyBindings.class);
            MinecraftForge.EVENT_BUS.register(new SoundOptionsHandler()); // Add this
            LOGGER.info("Chat TTS handler registered");
        });
    }
}```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/TTSNative.java`

```java
package dev.bytesizedfox.dectalkttsmod;

public class TTSNative {
    private static boolean loaded = false;

    static {
        try {
            // Load both the TTS library and JNI wrapper
            NativeLoader.loadLibrary("tts_jni");
            loaded = true;
        } catch (Exception e) {
            System.err.println("Failed to load TTS native libraries: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public static boolean isLoaded() {
        return loaded;
    }

    public static native boolean init();
    public static native boolean speak(String text);
    public static native void sync();
    public static native void reset();
    public static native int getAvailableSamples();
    public static native int readSamples(short[] output, int count);
}```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/NativeLoader.java`

```java
package dev.bytesizedfox.dectalkttsmod;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class NativeLoader {
    private static File tempDir;

    public static void loadLibrary(String baseName) {
        try {
            String os = System.getProperty("os.name").toLowerCase();
            String arch = System.getProperty("os.arch").toLowerCase();

            String platform;
            String jniLib;
            String ttsLib;

            if (os.contains("win")) {
                platform = "win64";
                jniLib = baseName + "_win64.dll";
                ttsLib = "dtc.dll";
            } else if (os.contains("mac")) {
                platform = "macos";
                jniLib = "lib" + baseName + "_macos.dylib";
                ttsLib = "libdectalk.dylib";
            } else if (os.contains("linux")) {
                platform = "linux64";
                jniLib = "lib" + baseName + "_linux64.so";
                ttsLib = "libdectalk.so";
            } else {
                throw new UnsupportedOperationException("Unsupported platform: " + os);
            }

            System.out.println("=== Native Library Loading ===");
            System.out.println("Platform: " + platform);
            System.out.println("JNI Library: " + jniLib);
            System.out.println("TTS Library: " + ttsLib);

            // Create temp directory for extracted libraries
            tempDir = new File(System.getProperty("java.io.tmpdir"), "dectalktts_" + System.currentTimeMillis());
            tempDir.mkdirs();
            tempDir.deleteOnExit();

            System.out.println("Temp dir: " + tempDir.getAbsolutePath());

            // Extract TTS library FIRST
            File ttsLibFile = extractLibrary("/natives/" + platform + "/" + ttsLib, ttsLib);
            System.out.println("Extracted TTS lib: " + ttsLibFile.getAbsolutePath());

            // Extract JNI wrapper
            File jniLibFile = extractLibrary("/natives/" + platform + "/" + jniLib, jniLib);
            System.out.println("Extracted JNI lib: " + jniLibFile.getAbsolutePath());

            // Load TTS library first
            System.out.println("Loading TTS library...");
            System.load(ttsLibFile.getAbsolutePath());
            System.out.println("TTS library loaded");

            // Then load JNI wrapper (which depends on TTS library)
            System.out.println("Loading JNI wrapper...");
            System.load(jniLibFile.getAbsolutePath());
            System.out.println("JNI wrapper loaded");

            System.out.println("=== Native loading complete ===");

        } catch (Exception e) {
            throw new RuntimeException("Failed to load native libraries", e);
        }
    }

    private static File extractLibrary(String resourcePath, String fileName) throws Exception {
        InputStream in = NativeLoader.class.getResourceAsStream(resourcePath);
        if (in == null) {
            throw new RuntimeException("Library not found in JAR: " + resourcePath);
        }

        File outputFile = new File(tempDir, fileName);
        outputFile.deleteOnExit();

        try (OutputStream out = new FileOutputStream(outputFile)) {
            byte[] buffer = new byte[8192];
            int bytesRead;
            while ((bytesRead = in.read(buffer)) != -1) {
                out.write(buffer, 0, bytesRead);
            }
        }

        // Make executable on Unix systems
        if (!System.getProperty("os.name").toLowerCase().contains("win")) {
            outputFile.setExecutable(true);
        }

        return outputFile;
    }
}```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/TTSConfig.java`

```java
package dev.bytesizedfox.dectalkttsmod;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import net.minecraft.client.Minecraft;
import net.minecraft.sounds.SoundSource;
import net.minecraftforge.fml.loading.FMLPaths;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.nio.file.Path;

public class TTSConfig {
    private static final Gson GSON = new GsonBuilder().setPrettyPrinting().create();
    private static final Path CONFIG_PATH = FMLPaths.CONFIGDIR.get().resolve("dectalkttsmod.json");

    private static ConfigData data = new ConfigData();

    public static class ConfigData {
        public boolean enableChatTTS = true;
    }

    public static void load() {
        File configFile = CONFIG_PATH.toFile();

        if (configFile.exists()) {
            try (FileReader reader = new FileReader(configFile)) {
                data = GSON.fromJson(reader, ConfigData.class);
                if (data == null) {
                    data = new ConfigData();
                }
            } catch (Exception e) {
                System.err.println("Failed to load TTS config, using defaults: " + e.getMessage());
                data = new ConfigData();
            }
        } else {
            // Create default config
            save();
        }
    }

    public static void save() {
        try (FileWriter writer = new FileWriter(CONFIG_PATH.toFile())) {
            GSON.toJson(data, writer);
        } catch (Exception e) {
            System.err.println("Failed to save TTS config: " + e.getMessage());
        }
    }

    // Getters
    public static boolean isEnabled() {
        return data.enableChatTTS;
    }

    public static double getVolume() {
        return Minecraft.getInstance().options.getSoundSourceVolume(SoundSource.VOICE);
    }

    // Setters (if you want to add keybinds or commands to toggle settings)
    public static void setEnabled(boolean enabled) {
        data.enableChatTTS = enabled;
        save();
    }
}```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/TTSManager.java`

```java
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
            TTSNative.speak(message);
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
```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/TTSMixer.java`

```java
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
```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/ChatTTSHandler.java`

```java
package dev.bytesizedfox.dectalkttsmod;


import net.minecraftforge.client.event.ClientChatReceivedEvent;
import net.minecraftforge.event.TickEvent;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class ChatTTSHandler {
    private static final Logger LOGGER = LogManager.getLogger();
    public TTSManager ttsManager;

    public ChatTTSHandler() {
        LOGGER.info("ChatTTSHandler constructed");
        this.ttsManager = null;
    }

    private TTSManager getTTSManager() {
        if (ttsManager == null) {
            LOGGER.info("Creating TTSManager (lazy init)");
            ttsManager = new TTSManager();
        }
        return ttsManager;
    }

    @SubscribeEvent
    public void onChatReceived(ClientChatReceivedEvent event) {
        LOGGER.debug("Chat event received");

        if (!TTSConfig.isEnabled()) {
            LOGGER.debug("TTS is disabled in config");
            return;
        }

        if (!TTSNative.isLoaded()) {
            LOGGER.warn("Chat received but TTSNative not loaded");
            return;
        }

        // Skip "command set" from command blocks"
        if (event.getMessage().getString().startsWith("Command set:")) {
            return;
        }

        String plainText = event.getMessage().getString();
        LOGGER.info("Chat message received: '{}'", plainText);

        String cleanText = cleanMessage(plainText);
        LOGGER.info("Cleaned message: '{}'", cleanText);

        if (!cleanText.isEmpty()) {
            getTTSManager().speak(cleanText);
        } else {
            LOGGER.info("Message is empty after cleaning, skipping");
        }
    }

    @SubscribeEvent
    public void onClientTick(TickEvent.ClientTickEvent event) {
        if (event.phase == TickEvent.Phase.END && ttsManager != null) {
            ttsManager.tick();
        }
    }

    private String cleanMessage(String message) {
        LOGGER.debug("Cleaning message: '{}'", message);

        // Remove formatting codes (§ codes)
        message = message.replaceAll("§[0-9a-fk-or]", "");

        // Remove player name prefix - extract just the message content
        // Handles both "<PlayerName> message" and "PlayerName: message" formats

        // Check for <PlayerName> format
        if (message.matches("^<[^>]+>.*")) {
            message = message.replaceFirst("^<[^>]+>\\s*", "");
        }
        // Check for PlayerName: format
        else if (message.matches("^[^:]+:.*")) {
            message = message.replaceFirst("^[^:]+:\\s*", "");
        }

        // Remove URLs (TTS will butcher them)
        message = message.replaceAll("https?://\\S+", "link");

        // Keep only single-byte characters (0-255)
        // Removes emoji, unicode, etc. - anything DECtalk can't handle
        StringBuilder cleaned = new StringBuilder();
        for (char c : message.toCharArray()) {
            if (c <= 255) {
                cleaned.append(c);
            }
        }
        message = cleaned.toString();

        return "[:phoneme on]" + message.trim();
    }

    public void shutdown() {
        LOGGER.info("ChatTTSHandler shutting down");
        if (ttsManager != null) {
            ttsManager.stop();
        }
    }
}```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/SoundOptionsHandler.java`

```java
package dev.bytesizedfox.dectalkttsmod;

import net.minecraft.client.OptionInstance;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.client.gui.screens.SoundOptionsScreen;
import net.minecraft.client.gui.components.OptionsList;
import net.minecraft.network.chat.Component;
import net.minecraftforge.client.event.ScreenEvent;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import net.minecraftforge.fml.util.ObfuscationReflectionHelper;

import java.lang.reflect.Field;

public class SoundOptionsHandler {

    @SubscribeEvent
    public void onScreenInit(ScreenEvent.Init.Post event) {
        Screen screen = event.getScreen();

        if (!(screen instanceof SoundOptionsScreen)) {
            return;
        }

        try {
            // https://linkie.shedaniel.dev/mappings?namespace=mojang_srg&version=1.20.1&search=SoundOptionsScreen.list&translateMode=none
            // Use reflection to access the options list
            Field listField = ObfuscationReflectionHelper.findField(SoundOptionsScreen.class, "f_244146_");
            listField.setAccessible(true);
            OptionsList optionsList = (OptionsList) listField.get(screen);

            // Create an OptionInstance for the TTS toggle
            OptionInstance<Boolean> ttsOption = OptionInstance.createBoolean(
                    "options.enableDECtalkMini",
                    OptionInstance.cachedConstantTooltip(Component.translatable("options.enableDECtalkMiniTooltip")),
                    TTSConfig.isEnabled(),
                    (newValue) -> {
                        TTSConfig.setEnabled(newValue);
                    }
            );

            // Add to the options list
            optionsList.addSmall(ttsOption, null);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}```

## `src/main/java/dev/bytesizedfox/dectalkttsmod/TTSKeyBindings.java`

```java
package dev.bytesizedfox.dectalkttsmod;

import com.mojang.blaze3d.platform.InputConstants;
import net.minecraft.client.KeyMapping;
import net.minecraft.client.Minecraft;
import net.minecraft.network.chat.Component;
import net.minecraftforge.client.event.RegisterKeyMappingsEvent;
import net.minecraftforge.client.event.InputEvent;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import org.lwjgl.glfw.GLFW;

public class TTSKeyBindings {
    public static final String CATEGORY = "DECtalkMini TTS";

    public static KeyMapping toggleTTS;
    public static KeyMapping clearTTS;

    public static void register(RegisterKeyMappingsEvent event) {
        toggleTTS = new KeyMapping(
                "key.dectalkttsmod.toggle",
                InputConstants.Type.KEYSYM,
                GLFW.GLFW_KEY_F8,
                CATEGORY
        );
        clearTTS = new KeyMapping(
                "key.dectalkttsmod.clear",
                InputConstants.Type.KEYSYM,
                GLFW.GLFW_KEY_F9,
                CATEGORY
        );

        event.register(toggleTTS);
        event.register(clearTTS);
    }

    @SubscribeEvent
    public static void onKeyInput(InputEvent.Key event) {
        Minecraft mc = Minecraft.getInstance();

        if (toggleTTS.consumeClick()) {
            boolean newState = !TTSConfig.isEnabled();
            TTSConfig.setEnabled(newState);

            if (mc.player != null) {
                mc.player.displayClientMessage(
                        net.minecraft.network.chat.Component.literal(
                                "DECtalkMini TTS: " + (newState ? "§aEnabled" : "§cDisabled")
                        ),
                        true  // Show in action bar
                );
            }
        }

        if (clearTTS.consumeClick() && mc.player != null) {
            if (Dectalkttsmod.chatHandler != null && Dectalkttsmod.chatHandler.ttsManager != null) {
                Dectalkttsmod.chatHandler.ttsManager.clear();
            }
            mc.player.displayClientMessage(
                    Component.literal("DECtalkMini TTS: §eStopped"),
                    true
            );
        }
    }
}```

