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

SuperSpectrogramAnalyst::SuperSpectrogramAnalyst()
: mRate(0.0)
, mWindowSize(0)
{
}

SuperSpectrogramAnalyst::~SuperSpectrogramAnalyst()
{
}

size_t SuperSpectrogramAnalyst::computeDecimationLevel(double inputRate, double targetRate)
{
   if (targetRate <= 0.0) {
      throw std::invalid_argument("targetRate must be > 0");
   }

   double ratio = inputRate / targetRate;
   size_t decimation = static_cast<size_t>(std::round(ratio));

   // enforce minimum of 1 (no decimation) to avoid invalid factors
   return decimation < 1 ? 1 : decimation;
}

bool SuperSpectrogramAnalyst::Calculate(
   const float* data,
   size_t dataLen,
   size_t detailLevel,
   size_t frequencyRate,
   size_t lowerThreshold
)
{
   STFTProcessor stftProcessor(detailLevel);
   stftProcessor.setLowerThreshold(lowerThreshold);

   auto it = DETAIL_TO_DECIMATED_FREQ.find(detailLevel);
   if (it == DETAIL_TO_DECIMATED_FREQ.end()) {
      return false;
   }

   mTargetRate = it->second;

   size_t decimationLevel = computeDecimationLevel(frequencyRate, mTargetRate);

   auto spectrogram = stftProcessor.processFullSTFT(data, dataLen, decimationLevel);

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
