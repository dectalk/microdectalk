#!/bin/sh -e
if [ "x$1" = "x" ]; then
	TARGET="nt"
else
	TARGET="$1"
fi

if [ "$TARGET" = "dos4g" ]; then
	ARGS="-DNO_FILESYSTEM"
	FILES=""
elif [ "$TARGET" = "nt" ]; then
	ARGS=""
	FILES="src/mman-win32/mman.c"
else
	ARGS=""
	FILES=""
fi

OBJS=""
for i in src/*.c $FILES; do
	OUT="`echo $i | cut -d. -f1`.o"
	OBJS="$OBJS +$OUT"

        if [ -f "$OUT" ]; then
                continue
        fi
        owcc -Wc-fx -c -b$TARGET $ARGS -Iinclude -Isrc -D_REENTRANT -DNOMME -DLTSSIM -DTTSSIM -DANSI -DBLD_DECTALK_DLL -DENGLISH -DENGLISH_US -DACCESS32 -DTYPING_MODE -DACNA -DDISABLE_AUDIO -DSINGLE_THREADED -o $OUT $i
done

wlib -q -b -fo -n dtc.lib $OBJS

owcc -b$TARGET -Iinclude -o say.exe main.c dtc.lib
