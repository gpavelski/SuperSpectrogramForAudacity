/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAnalyst.cpp

  Tony Bee

*******************************************************************//**

\class SuperSpectrogramAnalyst
\brief Used for finding the peaks, for snapping to peaks.

This class is used to do the 'find peaks' snapping both in FreqPlot
and in the spectrogram spectral selection.

*//*******************************************************************/


#include "STFTProcessor.h"
#include "SuperSpectrogramAnalyst.h"
#include "FFT.h"
#include "MemoryX.h"

bool SuperSpectrogramAnalyst::Calculate(
   const float* data,
   size_t dataLen,
   size_t detailLevel,
   size_t frequencyRate,
   size_t lowerThreshold
)
{
   auto it = DETAIL_TO_DECIMATED_FREQ.find(detailLevel);
   if (it == DETAIL_TO_DECIMATED_FREQ.end()) {
      return false;
   }

   mTargetRate = it->second;

   Decimator decimator(frequencyRate, mTargetRate);
   auto decimatedSignal = decimator.process(data, dataLen);

   STFTProcessor stftProcessor(detailLevel);
   stftProcessor.setLowerThreshold(lowerThreshold);
   auto spectrogram = stftProcessor.processFullSTFT(decimatedSignal);

   // Convert flat vector to 2D matrix
   int sigma = stftProcessor.getSigma();
   int rows = 8 * sigma;
   int cols = spectrogram.size() / rows;

   mMatrix.assign(rows, std::vector<double>(cols));

   for (int col = 0; col < cols; ++col) {
      for (int row = 0; row < rows; ++row) {
         size_t idx = col * rows + row;
         mMatrix[row][col] = spectrogram[idx];
      }
   }

   return true;
}
