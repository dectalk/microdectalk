package dev.bytesizedfox.dectalkttsmod;


import net.minecraft.client.Minecraft;
import net.minecraft.client.multiplayer.ClientPacketListener;
import net.minecraft.client.multiplayer.PlayerInfo;
import net.minecraftforge.client.event.ClientChatReceivedEvent;
import net.minecraftforge.common.UsernameCache;
import net.minecraftforge.event.TickEvent;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.nio.charset.StandardCharsets;
import java.util.UUID;

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

    public String getUsernameFromUUIDClient(UUID uuid) {
        return UsernameCache.getLastKnownUsername(uuid);
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

        // small voice picker
        int namehash = 0;

        int startIndex = plainText.indexOf("<");
        int endIndex = plainText.indexOf(">", startIndex);

        if (startIndex != -1 && endIndex != -1) {
            String name = plainText.substring(startIndex + 1, endIndex);
            namehash = name.hashCode() - 1;
        }

        String names[] = {"np", "nb", "nh", "nf","nk","nr","nu","nd","nw"};
        String name = "[:" + names[Math.abs(namehash) % names.length] + "] ";

        String cleanText = name + cleanMessage(plainText, event.getMessage().getSiblings().isEmpty());
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

    private String cleanMessage(String message, boolean isSiblingsEmpty) {
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
        else if (message.matches("^[A-Za-z0-9_]{3,16}:\\s.*")) {
            message = message.replaceFirst("^[A-Za-z0-9_]{3,16}:\\s*", "");
        } else {
            if (!isSiblingsEmpty) {
                message = ""; // if not unformatted and not a player message, erase the message
            }
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
}