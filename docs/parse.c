
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX 500000

#define get_long_int(ptr) ((uint32_t)\
                       ((((uint8_t *)(ptr))[3] << 24)  | \
                        (((uint8_t *)(ptr))[2] << 16)  | \
                        (((uint8_t *)(ptr))[1] << 8)  | \
                        (((uint8_t *)(ptr))[0])))

const unsigned char *phonemes[100] = {
            "SIL", // 0
            "IY", // 1
            "IH", // 2
            "EY", // 3
            "EH", // 4
            "AE", // 5
            "AA", // 6
            "AY", // 7
            "AW", // 8
            "AH","AO","OW","OY","UH","UW","RR","YU","AX","IX","IR","ER","AR","OR","UR","W","Y","R","LL","HX","RX","LX","M","N","NX","EL","D_DENTALIZED","EN","F","V","TH","DH","S","Z","SH",
            "ZH","P","B","T","D","K","G","DX","TX","Q","CH","JH","DF"
};

#define BLOCK_RULES             (100)                   /* Block allophone substitutes. */
#define S3                              (100+ 1)                /* Tertiary stress */
#define S2                              (100+ 2)                /* Secondary stress             */
#define S1                              (100+ 3)                /* Primary stress               */
#define SEMPH                   (100+ 4)                /* Emphatic stress              */
#define HAT_RISE                (100+ 5)                /* Explicit hat rise            */
#define HAT_FALL                (100+ 6)                /* Explicit hat fall            */
#define HAT_RF                  (100+ 7)                /* Explicit hat rise-fall       */
#define SBOUND                  (100+ 8)                /* Syllable boundary            */
#define MBOUND                  (100+ 9)                /* Morpheme boundary            */
#define HYPHEN                  (100+10)                /* For noun compounds           */
#define WBOUND                  (100+11)                /* Word boundary                */
#define PPSTART                 (100+12)                /* Prep phrase start            */
#define VPSTART                 (100+13)                /* Verb phrase start            */
#define RELSTART                (100+14)                /* Intro. to sentence or clause */
#define COMMA                   (100+15)                /* end of clause                */
#define PERIOD                  (100+16)                /* End of sentence              */
#define QUEST                   (100+17)                /* End of question              */
#define EXCLAIM                 (100+18)                /* End of exclamatory sentence! */
#define NEW_PARAGRAPH   (100+19)                /* Explicit new paragraph       */
#define SPECIALWORD             (100+20)        /* Special word in citation mode */
#define LINKRWORD               (100+21)        /* Linked R in UK */
#define DOUBLCONS               (100+22)        /* For double consonant marker */

const char *special(char c) {
    if (c == BLOCK_RULES) {
        return "BLOCK_RULES";
    }
    if (c == S3) {
        return "S3";
    }
    if (c == S2) {
        return "S2";
    }
    if (c == S1) {
        return "S1";
    }
    if (c == SEMPH) {
        return "SEMPH";
    }
    if (c == HAT_RISE) {
        return "HAT_RISE";
    }
    if (c == HAT_FALL) {
        return "HAT_FALL";
    }
    
    return "UNKNOWN";
}
int main(int argc, char* argv[]) {
    FILE* fptr1;
    char c;
    int i = 0, j;
    int from = 0;
    int to = 9999;
    fptr1 = fopen(argv[1], "rb");
    if (fptr1 == NULL) {
        return 1;
    }

    fseek(fptr1, 0L, SEEK_END);
    int size = ftell(fptr1);
    rewind(fptr1);

    unsigned char str[size];

    for (i = 0, j = 0; i <= size - 1; i++) {
        c = fgetc(fptr1);
        str[i] = c;
    }
    // bytes 0-3
    int entries = get_long_int((char *)str);
    // bytes 4-7
    int bytes = get_long_int((char *) &str[4]);
    char *indices = &str[8];
    char *data = &str[(entries * 4) + 8];

    int ind = 0;

    printf("Entries: %i, bytes: %i, entries:\n", entries, bytes );
    for (int j = 0; j < 100; j++) {
        int ind = get_long_int(&indices[j * 4]);
        char *entry = &data[ ind + 4 ];

        printf("Index: %i:\n", j);
        printf("Word: ");

        int i; // finds beginning of phoneme data
        for (i = 0; entry[i] != 0; i++) {
            printf("%c", (char) entry[i]);
        }
        printf(", phone data: ");
        for (i++; entry[i] != 0; i++) {
            char *phone = phonemes[entry[i]];
            if (phone == NULL) {
                phone = special(entry[i]);
            }
            printf("%s ", phone);
        }
        printf("\n");
    }

    fclose(fptr1);
    return 0;
}
