#include <stdio.h>
#include <time.h>

#include "epsonapi.h"
#include "kernel.h"

#include "wav.c"

int main () {
    init_wav("output.wav");
    TextToSpeechInit();

    TextToSpeechStart("[:phoneme on] [dey<600,24>][ziy<600,21>][dey<600,17>][ziy<600,12>][gih<200,14>][vmiy<200,16>][yurr<200,17>][ah<400,14>][nsrr<200,17>][duw<1200,12>][ay<600,19>][mhxah<600,24>][fkrey<600,21>][ziy<600,17>][ah<400,14>][llfow<100,16>][rthah<100,17>][llah<400,19>][vah<200,21>][vyu<1200,19>][ih<200,21>][twow<200,22>][ntbiy<200,21>][ah<200,19>][stay<400,24>][llih<200,21>][shmae<200,19>][rih<600,17>jh][ay<200,19>][keh<400,21>][ntah<200,17>][fow<400,14>][rdah<200,17>][keh<200,14>][rih<800,12>jh][buh<200,12>][tyu<400,17>][lluh<200,21>][kswiy<400,19>][tah<200,12>][pah<400,17>][nthah<200,21>][siy<200,19>t][ah<100,21>][vah<100,22>][bay<200,24>][sih<200,21>][kuh<200,17>][llbih<400,19>][lltfow<200,12>][rtuw<1200,17>]");

    close_wav();
}
