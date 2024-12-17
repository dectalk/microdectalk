#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lsdef.h"
#include "kernel.h"
#include "vtm_idef.h"

static double Tone( double, double * );

#define  RISE_TIME	 0.002
#define  MAX_TONE_BLOCK   1024
#define  TONE_SYMBOL    0x7FFF
#define  TWO_PI_EQUIVALENT  2 * M_PI

//extern void OutputData( short *, unsigned int, DWORD, DWORD, DWORD);
//extern void output_data();
extern void write_wav(short *iwave, int length);

int PlayTones(double DurationInMsec, double Freq_0, double Amp_0, double Freq_1, double Amp_1, double SampleRate ) {
    int i;
    int iRiseSamples;
    int iCenterSamples;
    int iCenterCount;
    int iSynthCount;
    int iTotalSamples;
    double * pRise;
    short pToneBuffer[MAX_TONE_BLOCK];
    short * pBuffer;
    double Sample;
    double PhaseIncrement_0;
    double Phase_0;
    double PhaseIncrement_1;
    double Phase_1;
    DWORD dwDurationInFrames;

    // adjust volume to avoid clipping
    Amp_0 *= 0.5;
    Amp_1 *= 0.5;

    dwDurationInFrames = (int)( 0.15625 * DurationInMsec );

    iTotalSamples = (int)( 0.001 * DurationInMsec * SampleRate );
    iRiseSamples = (int)((double)RISE_TIME * SampleRate );
    iCenterSamples = iTotalSamples - ( iRiseSamples << 1 );

    if ( iCenterSamples < 0 ) {
      iRiseSamples = iCenterSamples >> 1;
      iCenterSamples = 0;
    }

    double pRiseBuffer[iRiseSamples];

    /********************************************************************/
    /*  Allocate the tone audio buffer.                                 */
    /********************************************************************/
    //pToneBuffer = (short *) calloc( MAX_TONE_BLOCK, sizeof(short));
    //if ( pToneBuffer == NULL )
    //{
    //  return( TRUE );
    //}

    //pRiseBuffer = (double *) calloc( iRiseSamples, sizeof(double));

    //if ( pRiseBuffer == NULL )
    //{
    //  return( TRUE );
    //}

    PhaseIncrement_0 = 0.25 * TWO_PI_EQUIVALENT / (double)iRiseSamples;
    Phase_0 = 0.0;

    pRise = pRiseBuffer;

    for ( i = 0; i < iRiseSamples; i++ ) {
      Sample = sin(Phase_0);
      *pRise++ = Sample * Sample;
      Phase_0 += PhaseIncrement_0;
    }

    PhaseIncrement_0 = Freq_0 * SamplePeriod * TWO_PI_EQUIVALENT;
    Phase_0 = 0.0;
    PhaseIncrement_1 = Freq_1 * SamplePeriod * TWO_PI_EQUIVALENT;
    Phase_1 = 0.0;

    pBuffer = pToneBuffer;

    for ( i = 0; i < iRiseSamples; i++ ) {
      Sample  = Amp_0 * Tone( PhaseIncrement_0, &Phase_0 );
      Sample += Amp_1 * Tone( PhaseIncrement_1, &Phase_1 );
      *pBuffer++ = (short)( pRiseBuffer[i] * Sample );
    }

    //OutputData( phTTS,
    //          pToneBuffer,
    //          iRiseSamples,
    //          TONE_SYMBOL,
    //          (DWORD)( 0.15625 * DurationInMsec ),0);
    write_wav(pToneBuffer, iRiseSamples);

    iCenterCount = 0;

    while (( iCenterCount < iCenterSamples ) && ( ! KS.halting)) {
      iSynthCount = iCenterSamples - iCenterCount;

      if ( iSynthCount > MAX_TONE_BLOCK )
      {
        iSynthCount = MAX_TONE_BLOCK;
      }
      pBuffer = pToneBuffer;

      for ( i = 0; i < iSynthCount; i++ )
      {
      Sample  = Amp_0 * Tone( PhaseIncrement_0, &Phase_0 );
      Sample += Amp_1 * Tone( PhaseIncrement_1, &Phase_1 );
      *pBuffer++ = (short)Sample;
      }

      iCenterCount += iSynthCount;
      //OutputData( phTTS, pToneBuffer, iSynthCount, TONE_SYMBOL, 0 ,0);
      write_wav(pToneBuffer, iSynthCount);
    }
    pBuffer = pToneBuffer;

    for ( i = iRiseSamples - 1; i >= 0; i-- ) {
      Sample  = Amp_0 * Tone( PhaseIncrement_0, &Phase_0 );
      Sample += Amp_1 * Tone( PhaseIncrement_1, &Phase_1 );
      *pBuffer++ = (short)( pRiseBuffer[i] * Sample );
    }
    //OutputData( phTTS, pToneBuffer, iRiseSamples, TONE_SYMBOL, 0,0 );
    write_wav(pToneBuffer, iRiseSamples);
    //free( pRiseBuffer );
    //free( pToneBuffer );
    return( FALSE );
}

static double Tone( double PhaseIncrement, double * pPhase ) {
  double Sample;

  Sample = sin(*pPhase);

  *pPhase += PhaseIncrement;

  if ( *pPhase >= TWO_PI_EQUIVALENT )
    *pPhase -= TWO_PI_EQUIVALENT;

  return( Sample );
}
