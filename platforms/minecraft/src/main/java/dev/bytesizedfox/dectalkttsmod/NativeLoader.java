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
}