#include "Decimator.h"


Decimator::Decimator(int decimationFactor)
    : decimation_factor(decimationFactor) {
    if (decimationFactor <= 0) {
        throw std::invalid_argument("Decimation factor must be positive");
    }
}

std::vector<double> Decimator::process(const std::vector<double>& signal) {
    return process(signal.data(), signal.size());
}

std::vector<double> Decimator::process(const std::unique_ptr<double[]>& signal, size_t length) {
    return process(signal.get(), length);  // Delegate to the pointer version
}

std::vector<double> Decimator::process(const double* signal, size_t length) {
    const size_t state_size = initial_offset + filter_order;
    const size_t buffer_size = length + state_size + 1;
    const size_t output_size = static_cast<size_t>(std::ceil(static_cast<double>(length) / decimation_factor));

    std::vector<double> X(buffer_size, 0.0);
    std::vector<double> Y(buffer_size, 0.0);
    std::vector<double> output;
    output.reserve(output_size);

    // Initialize input buffer
    for (size_t i = filter_order + 1; i <= length + filter_order + 1; ++i) {
        X[i] = signal[i - filter_order - 1];
    }

    // Process samples
    size_t st = initial_offset + filter_order;
    for (size_t i = filter_order + 1; i < buffer_size; ++i) {
        Y[i] = b_coeffs[0] * X[i];

        for (size_t j = 1; j <= filter_order; ++j) {
            Y[i] += b_coeffs[j] * X[i - j];
            Y[i] -= a_coeffs[j] * Y[i - j];
        }

        if (i == st) {
            output.push_back(Y[i]);
            st += decimation_factor;
        }
    }

    return output;
}

std::vector<double> Decimator::process(const float* signal, size_t length) {
   const size_t state_size = initial_offset + filter_order;
   const size_t buffer_size = length + state_size + 1;
   const size_t output_size = static_cast<size_t>(std::ceil(static_cast<float>(length) / decimation_factor));

   std::vector<float> X(buffer_size, 0.0);
   std::vector<float> Y(buffer_size, 0.0);
   std::vector<double> output;
   output.reserve(output_size);

   // Initialize input buffer
   for (size_t i = filter_order + 1; i <= length + filter_order + 1; ++i) {
      X[i] = signal[i - filter_order - 1];
   }

   // Process samples
   size_t st = initial_offset + filter_order;
   for (size_t i = filter_order + 1; i < buffer_size; ++i) {
      Y[i] = b_coeffs[0] * X[i];

      for (size_t j = 1; j <= filter_order; ++j) {
         Y[i] += b_coeffs[j] * X[i - j];
         Y[i] -= a_coeffs[j] * Y[i - j];
      }

      if (i == st) {
         output.push_back(Y[i]);
         st += decimation_factor;
      }
   }

   return output;
}
