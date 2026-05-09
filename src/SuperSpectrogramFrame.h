/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramFrame.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_FRAME__
#define __SUPER_SPECTROGRAM_FRAME__

struct SuperSpectrogramFrame
{
   std::vector<std::vector<double>> matrix;
   double maxFreq = 0.0;
   size_t numSamples = 0;
};

#endif