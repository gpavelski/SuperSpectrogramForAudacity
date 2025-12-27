#include "Decimator.h"

Decimator::Decimator(int decimationFactor)
   : decimation_factor(decimationFactor) {
   if (decimationFactor <= 0) {
      throw std::invalid_argument("Decimation factor must be positive");
   }
}
