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
}