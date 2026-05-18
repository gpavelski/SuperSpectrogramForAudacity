/**********************************************************************

  Audacity: A Digital Audio Editor

  Decimator.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>

#include "Model/Cheby1LowPassIIRFilter.h"

class Decimator {
public:
   Decimator(double frequencyRate, double targetRate);

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
   std::vector<double> processImpl(
      const T* signal,
      size_t length
   )
   {
      std::vector<double> x(signal, signal + length);

      // zero-phase filter
      std::vector<double> y = filtfilt(x, b_coeffs, a_coeffs);

      // indexing logic
      size_t nout = (length + step - 1) / step;
      size_t nbeg = step - (step * nout - length);

      std::vector<double> out;
      out.reserve(nout);

      for (size_t i = nbeg - 1; i < length; i += step)
         out.push_back(y[i]);

      return out;
   }

   double computeDecimationLevel(double inputRate, double targetRate) const;

   void iirFilter(
      const std::vector<double>& x,
      std::vector<double>& y,
      const std::vector<double>& b,
      const std::vector<double>& a);

   std::vector<double> filtfilt(
      const std::vector<double>& x,
      const std::vector<double>& b,
      const std::vector<double>& a);

private:
   std::vector<double> b_coeffs;
   std::vector<double> a_coeffs;

   static constexpr int filter_order = 8;
   static constexpr double rp = 0.05;
   double step = 1.0;
   double Wn = 0.8;
};
