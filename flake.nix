{
  description = "Nix flake for DECtalkMini";

  inputs = {
    nixpkgs.url = "nixpkgs";
  };

  outputs = { self, nixpkgs }:
    let
      lib = nixpkgs.lib;
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = f: lib.genAttrs systems (system: f system);
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
          version =
            if self ? shortRev then
              self.shortRev
            else if self ? dirtyShortRev then
              self.dirtyShortRev
            else
              "dirty";

          commonMeta = {
            homepage = "https://github.com/dectalk/dectalk";
            platforms = systems;
          };

          mkNativeBuild =
            {
              pname,
              withSpeak ? false,
              nativeBuildInputs ? [ ],
              buildInputs ? [ ],
              installPhase,
            }:
            pkgs.stdenv.mkDerivation {
              inherit pname version installPhase;
              src = ./.;

              strictDeps = true;
              enableParallelBuilding = true;

              nativeBuildInputs =
                [
                  pkgs.gnumake
                  pkgs.makeWrapper
                  pkgs.premake5
                ]
                ++ nativeBuildInputs;

              buildInputs = [ pkgs.libiconv ] ++ buildInputs;

              buildPhase = ''
                runHook preBuild
                premake5 gmake ${lib.optionalString withSpeak "--build-speak=yes"}
                make config=release_native
                runHook postBuild
              '';

              meta = commonMeta // {
                description = "Portable DECtalk build output: ${pname}";
              };
            };

          libdtc = mkNativeBuild {
            pname = "libdtc";
            installPhase = ''
              runHook preInstall
              mkdir -p $out/lib $out/share/dectalk
              install -Dm755 bin/Native/Release/libdtc.so $out/lib/libdtc.so
              install -Dm644 DECtalk.conf $out/share/dectalk/DECtalk.conf
              cp -r dic $out/share/dectalk/dic
              runHook postInstall
            '';
          };

          say = mkNativeBuild {
            pname = "say";
            installPhase = ''
              runHook preInstall
              mkdir -p $out/bin $out/libexec/dectalk
              install -Dm755 bin/Native/Release/say $out/libexec/dectalk/say
              install -Dm755 bin/Native/Release/libdtc.so $out/libexec/dectalk/libdtc.so
              install -Dm644 DECtalk.conf $out/libexec/dectalk/DECtalk.conf
              cp -r dic $out/libexec/dectalk/dic
              makeWrapper $out/libexec/dectalk/say $out/bin/say \
                --prefix LD_LIBRARY_PATH : $out/libexec/dectalk
              runHook postInstall
            '';
          };

          speak =
            mkNativeBuild {
              pname = "speak";
              withSpeak = true;
              buildInputs = [
                pkgs.libXpm
                pkgs.libXt
                pkgs.motif
              ];
              installPhase = ''
                audioRuntimePath="${lib.makeLibraryPath [
                  pkgs.alsa-lib
                  pkgs.libjack2
                  pkgs.libpulseaudio
                ]}"

                runHook preInstall
                mkdir -p \
                  $out/bin \
                  $out/libexec/dectalk \
                  $out/share/applications \
                  $out/share/icons/hicolor/256x256/apps
                install -Dm755 bin/Native/Release/speak $out/libexec/dectalk/speak
                install -Dm755 bin/Native/Release/libdtc.so $out/libexec/dectalk/libdtc.so
                install -Dm644 DECtalk.conf $out/libexec/dectalk/DECtalk.conf
                cp -r dic $out/libexec/dectalk/dic
                makeWrapper $out/libexec/dectalk/speak $out/bin/speak \
                  --prefix LD_LIBRARY_PATH : "$audioRuntimePath" \
                  --prefix LD_LIBRARY_PATH : $out/libexec/dectalk
                install -Dm644 resources/speak.desktop $out/share/applications/speak.desktop
                install -Dm644 resources/paul.png $out/share/icons/hicolor/256x256/apps/paul.png
                runHook postInstall
              '';
            };

          appimage =
            lib.optionalAttrs (system == "x86_64-linux") {
              appimage = pkgs.stdenv.mkDerivation {
                pname = "speak-appimage";
                inherit version;
                src = ./.;

                nativeBuildInputs = [
                  pkgs.appimage-run
                ];

                appimagetool = pkgs.fetchurl {
                  url = "https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage";
                  hash = "sha256-ptceK2zWb46NFsN60WRliYXgz1/KqVDJCkgokMudE+A=";
                };

                runtimeFile = pkgs.fetchurl {
                  url = "https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-x86_64";
                  hash = "sha256-okGdzkdWg5WuecAf+ppaNB3TOVgTUv8QTQc1J1Qxd+U=";
                };

                dontUnpack = true;

                buildPhase = ''
                  runHook preBuild

                  mkdir -p appdir/usr/bin appdir/usr/lib
                  cp ${speak}/libexec/dectalk/speak appdir/usr/bin/speak
                  cp ${speak}/libexec/dectalk/libdtc.so appdir/usr/lib/libdtc.so
                  cp $src/resources/speak.desktop appdir/speak.desktop
                  cp $src/resources/paul.png appdir/paul.png
                  cp $src/resources/AppRun appdir/AppRun
                  cp $src/DECtalk.conf appdir/DECtalk.conf
                  cp -r $src/dic appdir/dic
                  chmod +x appdir/AppRun

                  export ARCH=x86_64
                  appimage-run "$appimagetool" \
                    --runtime-file "$runtimeFile" \
                    appdir \
                    "$TMPDIR/Speak-x86_64.AppImage"

                  runHook postBuild
                '';

                installPhase = ''
                  runHook preInstall
                  install -Dm755 "$TMPDIR/Speak-x86_64.AppImage" \
                    "$out/Speak-x86_64.AppImage"
                  runHook postInstall
                '';

                meta = commonMeta // {
                  description = "AppImage bundle for the DECtalkMini speak demo";
                };
              };
            };
        in
        {
          default = say;
          inherit libdtc say speak;
        }
        // appimage
      );

      apps = forAllSystems (
        system:
        let
          packages = self.packages.${system};
        in
        {
          default = {
            type = "app";
            program = "${packages.say}/bin/say";
          };
          say = {
            type = "app";
            program = "${packages.say}/bin/say";
          };
          speak = {
            type = "app";
            program = "${packages.speak}/bin/speak";
          };
        }
      );

      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            packages = [
              pkgs.appimage-run
              pkgs.gnumake
              pkgs.libXpm
              pkgs.libXt
              pkgs.motif
              pkgs.premake5
            ];
          };
        }
      );
    };
}
