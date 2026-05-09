/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAnalyst.cpp

  Guilherme Pavelski

*******************************************************************//**

\class SuperSpectrogramAnalyst
\brief Used for wrapping the calculation of the Spectrogram.

This class is responsible for reading the performing an interpolation
on the selected audio signal to the selected frequency, calling the
STFT calculation and returning the calculated Spectrogram.

*//*******************************************************************/

#include "STFTProcessor.h"
#include "SuperSpectrogramAnalyst.h"
#include "FFT.h"
#include "MemoryX.h"

bool SuperSpectrogramAnalyst::Calculate(
   const float* data,
   size_t dataLen,
   size_t frequencyRate,
   size_t detailLevel,
   size_t lowerThreshold
)
{
   auto it = DETAIL_TO_DECIMATED_FREQ.find(detailLevel);
   if (it == DETAIL_TO_DECIMATED_FREQ.end()) return false;

   mTargetRate = it->second;

   Decimator decimator(frequencyRate, mTargetRate);
   auto decimatedSignal = decimator.process(data, dataLen);

   STFTProcessor stftProcessor(detailLevel);
   stftProcessor.setLowerThreshold(lowerThreshold);

   mMatrix = stftProcessor.processFullSTFTMatrix(decimatedSignal);
   mSignalLength = stftProcessor.getResizedSignalLength();

   return true;
}

