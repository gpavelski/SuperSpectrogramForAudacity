/**********************************************************************

  Audacity: A Digital Audio Editor

  Decimator.h

  Guilherme Pavelski

**********************************************************************/

#ifndef DECIMATOR_H
#define DECIMATOR_H

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>

class Decimator {
public:
   Decimator(double frequencyRate, double targetRate);

   // ================= Public API =================

   std::vector<double> process(const double* signal, size_t length) {
      return processImpl(signal, length);
   }
   std::vector<double> process(const float* signal, size_t length) {
      return processImpl(signal, length);
   }
   std::vector<double> process(const std::vector<float>& signal) {
      return processImpl(signal.data(), signal.size());
   }
   std::vector<double> process(const std::vector<double>& signal) {
      return processImpl(signal.data(), signal.size());
   }
   std::vector<double> process(const std::unique_ptr<float[]>& signal, size_t length) {
      return processImpl(signal.get(), length);
   }
   std::vector<double> process(const std::unique_ptr<double[]>& signal, size_t length) {
      return processImpl(signal.get(), length);
   }

private:

   template<typename T>
   std::vector<double> processImpl(const T* signal, size_t length)
   {
      if (!signal || length == 0)
         return {};

      const size_t outputSize = static_cast<size_t>(length / step);

      std::vector<double> X(length + filter_order + 2, 0.0);
      std::vector<double> Y(length + filter_order + 2, 0.0);

      // Pad input to the right
      for (size_t i = 0; i < length; ++i)
         X[i + filter_order] = static_cast<double>(signal[i]);

      // FIR Convolution
      for (size_t n = filter_order; n < X.size(); ++n)
      {
         double acc = 0.0;
         for (int k = 0; k <= filter_order; ++k)
            acc += b_coeffs[k] * X[n - k];

         Y[n] = acc;
      }

      // Fractional decimation sampling
      std::vector<double> output;
      output.reserve(outputSize);

      double pos = filter_order; // starting index for sampling
      for (size_t i = 0; i < outputSize; ++i)
      {
         if (pos + 1 >= Y.size())
            break;

         double y = linearInterp(Y, pos);
         output.push_back((y > -1.0 && y < 1.0) ? y : 0.0);

         pos += step;
      }

      return output;
   }

   std::vector<double> designKaiserLowpass(
      double fs,
      double cutoff,
      double transition,
      double attenuation) const;
   double computeDecimationLevel(double inputRate, double targetRate) const;
   double linearInterp(const std::vector<double>& buf, double idx) const;

   std::vector<double> b_coeffs;    // FIR taps (dynamic)
   int filter_order = 0;

   double step = 1.0;               // fractional index increment
};

#endif // DECIMATOR_H
