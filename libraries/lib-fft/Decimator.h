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
   Decimator(double frequencyRate, double targetRate)
   {
      if (frequencyRate <= 0.0 || targetRate <= 0.0)
         throw std::invalid_argument("Sample rates must be positive");

      step = computeDecimationLevel(frequencyRate, targetRate);

      if (step <= 0) {
         throw std::runtime_error("Computed decimation factor is invalid");
      }

      // Anti-alias low-pass filter definition
      double cutoff      = targetRate * 0.45;    // 90% of Nyquist
      double transition  = targetRate * 0.05;    // 5% transition band
      double attenuation = 70.0;                 // 70 dB stop-band attenuation

      b_coeffs = designKaiserLowpass(
         frequencyRate,
         cutoff,
         transition,
         attenuation
      );

      filter_order = static_cast<int>(b_coeffs.size()) - 1;
   }

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

   // ================= Core Processing =================

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

   double computeDecimationLevel(double inputRate, double targetRate) const;

   // ================= Filter Design =================
   //
   // Windowed-sinc FIR with Kaiser window

   std::vector<double> designKaiserLowpass(
      double fs,
      double cutoff,
      double transition,
      double attenuation) const
   {
      if (transition <= 0.0)
         throw std::invalid_argument("Transition band must be > 0");

      double nyquist = fs * 0.5;
      double fc      = cutoff / nyquist;        // normalized
      double width   = transition / nyquist;    // normalized transition width

      // Kaiser beta from attenuation
      double beta;
      if (attenuation > 50.0) {
         beta = 0.1102 * (attenuation - 8.7);
      } else if (attenuation >= 21.0) {
         beta = 0.5842 * pow(attenuation - 21, 0.4)
              + 0.07886 * (attenuation - 21);
      } else {
         beta = 0.0;
      }

      // Filter order estimate
      int N = static_cast<int>(
         std::ceil((attenuation - 8.0) / (2.285 * 2 * M_PI * width))
      );
      if (N % 2 == 0) N++; // force odd length

      int M = (N - 1) / 2;
      std::vector<double> h(N);

      // Modified Bessel function I0 approximation
      auto I0 = [](double x) {
         double sum = 1.0;
         double y = x * x / 4.0;
         double t = y;
         for (int k = 1; k < 15; ++k) {
            sum += t / std::tgamma(k + 1);
            t *= y / (k + 1);
         }
         return sum;
      };

      // Generate coefficients
      for (int n = 0; n < N; ++n) {
         double k = n - M;

         // Sinc
         double sinc = (k == 0)
            ? 2.0 * fc
            : std::sin(2.0 * M_PI * fc * k) / (M_PI * k);

         // Kaiser window
         double ratio = (n - M) / static_cast<double>(M);
         double w = I0(beta * std::sqrt(1.0 - ratio * ratio)) / I0(beta);

         h[n] = sinc * w;
      }

      return h;
   }

   // ================= Utilities =================

   inline double linearInterp(const std::vector<double>& buf, double idx) const {
      size_t i = static_cast<size_t>(idx);
      double frac = idx - i;
      return buf[i] * (1.0 - frac) + buf[i + 1] * frac;
   }

private:
   std::vector<double> b_coeffs;    // FIR taps (dynamic)
   int filter_order = 0;

   double step = 1.0;               // fractional index increment
};

#endif // DECIMATOR_H
