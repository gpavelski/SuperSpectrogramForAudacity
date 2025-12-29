#include "Decimator.h"

double Decimator::computeDecimationLevel(double inputRate, double targetRate) const
{
   if (targetRate <= 0.0) {
      throw std::invalid_argument("targetRate must be > 0");
   }

   double ratio = inputRate / targetRate;

   // enforce minimum of 1 (no decimation) to avoid invalid factors
   return ratio < 1 ? 1 : ratio;
}
