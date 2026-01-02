/**********************************************************************

  Audacity: A Digital Audio Editor

  Decimator.cpp

  Guilherme Pavelski

*******************************************************************//**

\class Decimator
\brief Constructs a fractional decimator with an anti-aliasing FIR filter.

This class is responsible for downsampling the selected audio for
reducing the computational overhead of the STFT. Besides, it also
applies a low-pass anti-aliasing filter to the signal.
*//*******************************************************************/

#include "Decimator.h"

/**
 * 
 * Computes the effective decimation step based on the input and target
 * sampling rates, and designs a Kaiser-windowed low-pass FIR filter to
 * suppress aliasing prior to resampling.
 */
Decimator::Decimator(double frequencyRate, double targetRate)
{
   if (frequencyRate <= 0.0 || targetRate <= 0.0)
      throw std::invalid_argument("Sample rates must be positive");

   step = computeDecimationLevel(frequencyRate, targetRate);

   if (step <= 0) {
      throw std::runtime_error("Computed decimation factor is invalid");
   }

   // Anti-alias low-pass filter definition
   double cutoff = targetRate * 0.45;       // 90% of Nyquist
   double transition = targetRate * 0.05;   // 5% transition band
   double attenuation = 70.0;               // 70 dB stop-band attenuation

   b_coeffs = designKaiserLowpass(
      frequencyRate,
      cutoff,
      transition,
      attenuation
   );

   filter_order = static_cast<int>(b_coeffs.size()) - 1;
}

/**
 * Computes the fractional decimation ratio between input and target rates.
 *
 * Returns a ratio >= 1.0, where values greater than 1 indicate downsampling
 * and a value of 1 disables decimation.
 */
double Decimator::computeDecimationLevel(double inputRate, double targetRate) const
{
   if (targetRate <= 0.0) {
      throw std::invalid_argument("targetRate must be > 0");
   }

   double ratio = inputRate / targetRate;

   // Enforce minimum of 1 (no decimation)
   return ratio < 1 ? 1 : ratio;
}

// ================= Filter Design =================
//
// Windowed-sinc FIR with Kaiser window

/**
 * Designs a low-pass FIR filter using a Kaiser-windowed sinc function.
 *
 * The filter is parameterized by cutoff frequency, transition bandwidth,
 * and desired stop-band attenuation, and is suitable for anti-aliasing
 * prior to decimation.
 */
std::vector<double> Decimator::designKaiserLowpass(
   double fs,
   double cutoff,
   double transition,
   double attenuation) const
{
   if (transition <= 0.0)
      throw std::invalid_argument("Transition band must be > 0");

   double nyquist = fs * 0.5;
   double fc = cutoff / nyquist;           // Normalized cutoff
   double width = transition / nyquist;    // Normalized transition width

   // Compute Kaiser window beta parameter from attenuation
   double beta;
   if (attenuation > 50.0) {
      beta = 0.1102 * (attenuation - 8.7);
   }
   else if (attenuation >= 21.0) {
      beta = 0.5842 * std::pow(attenuation - 21, 0.4)
         + 0.07886 * (attenuation - 21);
   }
   else {
      beta = 0.0;
   }

   // Estimate filter order
   int N = static_cast<int>(
      std::ceil((attenuation - 8.0) / (2.285 * 2 * M_PI * width))
      );
   if (N % 2 == 0) N++; // Force odd length for symmetry

   int M = (N - 1) / 2;
   std::vector<double> h(N);

   // Approximation of the modified Bessel function I0
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

   // Generate windowed sinc coefficients
   for (int n = 0; n < N; ++n) {
      double k = n - M;

      // Ideal low-pass sinc
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

/**
 * Performs linear interpolation between adjacent samples.
 *
 * Used to resample the filtered signal at fractional positions
 * during decimation.
 */
double Decimator::linearInterp(const std::vector<double>& buf, double idx) const
{
   size_t i = static_cast<size_t>(idx);
   double frac = idx - i;
   return buf[i] * (1.0 - frac) + buf[i + 1] * frac;
}
