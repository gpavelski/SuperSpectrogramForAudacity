/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAnalyst.h

  Dominic Mazzoni
  Paul Licameli split from FreqWindow.h

**********************************************************************/

#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

class FFT_API SuperSpectrogramAnalyst
{
public:
   // Return true if successful
   bool Calculate(
      const float *data,
      size_t dataLen,
      size_t detailLevel,
      size_t rate,
      size_t lowerThreshold
   );

   // Returns the last computed 2D spectrogram matrix
   const std::vector<std::vector<double>>& GetMatrix() const
   {
      return mMatrix;
   }

   const double& GetTargetRate() const
   {
      return mTargetRate;
   }

private:
   double mTargetRate;
   std::vector<std::vector<double>> mMatrix;  // stores last computed spectrogram

   // Maps detailLevel to the desired target sampling rate (decimated frequency)
   const std::unordered_map<size_t, double> DETAIL_TO_DECIMATED_FREQ = {
       {4, 551.25},
       {5, 1102.5},
       {6, 2205.0},
       {7, 4410.0},
       {8, 8820.0}
   };
};
