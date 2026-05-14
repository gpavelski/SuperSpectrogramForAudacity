/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramFrame.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_FRAME__
#define __SUPER_SPECTROGRAM_FRAME__

#include <vector>

struct SuperSpectrogramFrame
{
   // Raw spectrogram matrix (time/frequency or similar domain)
   std::vector<std::vector<double>> matrix;

   // Maximum frequency represented by the dataset
   double maxFreq{ 0.0 };

   // Number of time samples in original signal
   size_t numSamples{ 0 };
};

#endif
