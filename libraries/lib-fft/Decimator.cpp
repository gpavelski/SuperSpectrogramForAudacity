#include "Decimator.h"

Decimator::Decimator(double frequencyRate, double targetRate) {
   if (frequencyRate <= 0 || targetRate <= 0) {
      throw std::invalid_argument("Frequencies must be positive");
   }

   step = computeDecimationLevel(frequencyRate, targetRate);

   if (step <= 0) {
      throw std::runtime_error("Computed decimation factor is invalid");
   }
}

double Decimator::computeDecimationLevel(double inputRate, double targetRate) const
{
   if (targetRate <= 0.0) {
      throw std::invalid_argument("targetRate must be > 0");
   }

   double ratio = inputRate / targetRate;

   // enforce minimum of 1 (no decimation) to avoid invalid factors
   return ratio < 1 ? 1 : ratio;
}
