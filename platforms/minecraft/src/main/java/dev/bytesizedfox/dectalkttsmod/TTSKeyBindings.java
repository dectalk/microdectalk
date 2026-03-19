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
}