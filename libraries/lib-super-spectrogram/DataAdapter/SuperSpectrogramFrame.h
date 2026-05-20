/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramFrame.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <vector>

struct SuperSpectrogramFrame
{
   std::vector<std::vector<double>> matrix;

   // Analysis-domain metadata
   double analysisMaxFreq;
   size_t analysisNumSamples;

   // Source-domain metadata
   size_t originalNumSamples;
   double originalSampleRate;
};
