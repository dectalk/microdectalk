#rm -rf tmp/*
mkdir -p tmp/src/dtk
mkdir -p tmp/include

DEFINES=(-DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__ -DSINGLE_LANGUAGE -DMultiple_Sample_Rates -UCALLER_ID -UNO_CMD -UOS_IN_SYSTEM -UACNA2)

for file in src/dtk/*.c include/*.c; do
    echo "Formatting File: $file -> tmp/$file"
    clang-format "$file" > "tmp/$file"
    for def in "${DEFINES[@]}"; do
        echo "Cleaning define: $def, in file: $tmp/$file"
        coan source "tmp/$file" -r $def > /dev/null
    done
done
