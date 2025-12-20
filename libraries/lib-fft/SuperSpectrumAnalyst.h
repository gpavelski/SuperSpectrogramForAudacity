/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrumAnalyst.h

  Dominic Mazzoni
  Paul Licameli split from FreqWindow.h

**********************************************************************/

#pragma once

#include <vector>
#include <functional>

class FFT_API SuperSpectrumAnalyst
{
public:
   SuperSpectrumAnalyst();
   ~SuperSpectrumAnalyst();

   // Return true if successful
   bool Calculate(
      const float *data,
      size_t dataLen
   );

   // Returns the last computed 2D spectrogram matrix
   const std::vector<std::vector<double>>& GetMatrix() const
   {
      return mMatrix;
   }

private:
   double mRate;
   size_t mWindowSize;
   std::vector<std::vector<double>> mMatrix;  // stores last computed spectrogram
};
