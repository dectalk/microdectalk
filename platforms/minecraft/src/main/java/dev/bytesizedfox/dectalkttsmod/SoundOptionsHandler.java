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
}