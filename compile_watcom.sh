#!/bin/sh -e
if [ "x$1" = "x" ]; then
	TARGET="nt_dll"
else
	TARGET="$1"
fi

if [ "$TARGET" = "dos4g" ]; then
	ARGS="-DNO_FILESYSTEM"
	FILES=""
elif [ "$TARGET" = "nt_dll" ]; then
	ARGS="-DBLD_DECTALK_DLL"
	FILES="src/mman-win32/mman.c"
else
	ARGS=""
	FILES=""
fi

OBJS=""
OBJS2=""
for i in src/*.c $FILES; do
	OUT="`echo $i | cut -d. -f1`.o"
	OBJS="$OBJS +$OUT"
	OBJS2="$OBJS2 $OUT"

        if [ -f "$OUT" ]; then
                continue
        fi
        owcc -Wc,-fx -c -b$TARGET $ARGS -Iinclude -Isrc -D_REENTRANT -DNOMME -DLTSSIM -DTTSSIM -DANSI  -DENGLISH -DENGLISH_US -DACCESS32 -DTYPING_MODE -DACNA -DDISABLE_AUDIO -DSINGLE_THREADED -o $OUT $i
done

if [ "$TARGET" = "nt_dll" ]; then
	owcc -b$TARGET -o dtc.dll -Wl,"option implib=dtc.lib" $OBJS2
else
	wlib -q -b -fo -n dtc.lib $OBJS
fi

owcc -b$TARGET -Iinclude -o say.exe main.c dtc.lib
