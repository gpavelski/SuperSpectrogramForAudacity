/**********************************************************************

  Audacity: A Digital Audio Editor

  Decimator.cpp

  Guilherme Pavelski

*******************************************************************//**

\class Decimator
\brief Constructs a fractional decimator with an anti-aliasing IIR filter.

This class is responsible for downsampling the selected audio for
reducing the computational overhead of the STFT. Besides, it also
applies a low-pass anti-aliasing filter to the signal.
*//*******************************************************************/

#include "Decimator.h"

Decimator::Decimator(double frequencyRate, double targetRate)
{
   if (frequencyRate <= 0.0 || targetRate <= 0.0)
      throw std::invalid_argument("Frequencies must be positive");

   step = computeDecimationLevel(frequencyRate, targetRate);

   if (step <= 0.0)
      throw std::runtime_error("Computed decimation factor is invalid");

   // ----------------------------------------------------
   // Chebyshev Type I low-pass filter design
   // ----------------------------------------------------
   const double Wn = 0.8 / step;
   const double rp = 0.05;

   Cheby1LowPassIIRFilter filter(filter_order, rp, Wn);

   b_coeffs = filter.b();
   a_coeffs = filter.a();
}

double Decimator::computeDecimationLevel(double inputRate, double targetRate) const
{
   if (targetRate <= 0.0)
      throw std::invalid_argument("targetRate must be > 0");

   const double ratio = inputRate / targetRate;
   return ratio < 1.0 ? 1.0 : ratio;
}

void Decimator::iirFilter(
   const std::vector<double>& x,
   std::vector<double>& y,
   const std::vector<double>& b,
   const std::vector<double>& a)
{
   const size_t N = x.size();
   const size_t order = a.size() - 1;

   y.assign(N, 0.0);

   for (size_t i = 0; i < N; ++i) {
      y[i] = b[0] * x[i];
      for (size_t j = 1; j <= order; ++j) {
         if (i >= j) {
            y[i] += b[j] * x[i - j];
            y[i] -= a[j] * y[i - j];
         }
      }
   }
}

std::vector<double> Decimator::filtfilt(
   const std::vector<double>& x,
   const std::vector<double>& b,
   const std::vector<double>& a)
{
   std::vector<double> y, yr, y2;

   // forward
   iirFilter(x, y, b, a);

   // reverse
   yr = y;
   std::reverse(yr.begin(), yr.end());

   // backward
   iirFilter(yr, y2, b, a);

   // reverse back
   std::reverse(y2.begin(), y2.end());

   return y2;
}
