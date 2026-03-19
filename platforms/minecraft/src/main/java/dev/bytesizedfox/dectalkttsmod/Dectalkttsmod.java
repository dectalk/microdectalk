package dev.bytesizedfox.dectalkttsmod;

import net.minecraftforge.api.distmarker.Dist;
import net.minecraftforge.common.MinecraftForge;
import net.minecraftforge.fml.DistExecutor;
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

        DistExecutor.safeRunWhenOn(Dist.CLIENT, () -> this::registerClientListeners);
    }

    private void registerClientListeners() {
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
}