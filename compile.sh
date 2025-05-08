gcc src/*.c  -I include \
    -D _REENTRANT -D NOMME -D LTSSIM -D TTSSIM -D ANSI -D BLD_DECTALK_DLL -D ENGLISH -D ENGLISH_US -D ACCESS32 -D TYPING_MODE -D OS_SIXTY_FOUR_BIT -D ACNA -D DISABLE_AUDIO -DSINGLE_THREADED \
    -D DICDEBUG \
    -lm -w -g -o say

#mkdir tmp
#for filename in src/*.c; do
#    gcc -M $filename  -I include \
#    -D _REENTRANT -D NOMME -D LTSSIM -D TTSSIM -D ANSI -D BLD_DECTALK_DLL -D ENGLISH -D ENGLISH_US -D ACCESS32 -D TYPING_MODE -D OS_SIXTY_FOUR_BIT -D ACNA -D DISABLE_AUDIO -DSINGLE_THREADED \
#    -lm -w -MF tmp/$(basename ${filename})
#done

#gcc -M src/*.c  -I include \
#    -D _REENTRANT -D NOMME -D LTSSIM -D TTSSIM -D ANSI -D BLD_DECTALK_DLL -D ENGLISH -D ENGLISH_US -D ACCESS32 -D TYPING_MODE -D OS_SIXTY_FOUR_BIT -D ACNA -D DISABLE_AUDIO \
#    -lm -w -MF dependencies.txt

./say
aplay output.wav
#rm output.wav
