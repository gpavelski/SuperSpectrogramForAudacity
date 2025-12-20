/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrumAnalyst.cpp

  Tony Bee

*******************************************************************//**

\class SuperSpectrumAnalyst
\brief Used for finding the peaks, for snapping to peaks.

This class is used to do the 'find peaks' snapping both in FreqPlot
and in the spectrogram spectral selection.

*//*******************************************************************/


#include "STFTProcessor.h"
#include "SuperSpectrumAnalyst.h"
#include "FFT.h"
#include "MemoryX.h"

SuperSpectrumAnalyst::SuperSpectrumAnalyst()
: mRate(0.0)
, mWindowSize(0)
{
}

SuperSpectrumAnalyst::~SuperSpectrumAnalyst()
{
}

bool SuperSpectrumAnalyst::Calculate(
   const float* data,
   size_t dataLen,
   size_t detailLevel,
   size_t decimationLevel,
   size_t lowerThreshold
)
{
   STFTProcessor stftProcessor(detailLevel);
   stftProcessor.setLowerThreshold(lowerThreshold);
   auto spectrogram = stftProcessor.processFullSTFT(data, dataLen, decimationLevel);

   // Convert flat vector to 2D matrix
   int sigma = stftProcessor.getSigma();
   int rows = 8 * sigma;
   int cols = spectrogram.size() / rows;

   mMatrix.resize(rows, std::vector<double>(cols));

   for (int col = 0; col < cols; ++col) {
      for (int row = 0; row < rows; ++row) {
         size_t idx = col * rows + row;
         mMatrix[row][col] = spectrogram[idx];
      }
   }

   return true;
}
