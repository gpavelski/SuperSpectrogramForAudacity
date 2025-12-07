/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrumAnalyst.cpp

  Dominic Mazzoni
  Paul Licameli split from FreqWindow.cpp

*******************************************************************//**

\class SuperSpectrumAnalyst
\brief Used for finding the peaks, for snapping to peaks.

This class is used to do the 'find peaks' snapping both in FreqPlot
and in the spectrogram spectral selection.

*//*******************************************************************/

/*
  Salvo Ventura - November 2006
  Extended range check for additional FFT windows
*/

#include "STFTProcessor.h"
#include "SuperSpectrumAnalyst.h"
#include "FFT.h"
#include "MemoryX.h"

SuperSpectrumAnalyst::SuperSpectrumAnalyst()
: mAlg(Spectrum)
, mRate(0.0)
, mWindowSize(0)
{
}

SuperSpectrumAnalyst::~SuperSpectrumAnalyst()
{
}

bool SuperSpectrumAnalyst::Calculate(
   const float* data,
   size_t dataLen
)
{
   const int l = 7;

   STFTProcessor stftProcessor(l);
   auto spectrogram = stftProcessor.processFullSTFT(data, dataLen);

   // Convert flat vector to 2D matrix
   int sigma = stftProcessor.getSigma();
   int rows = 8 * sigma;
   int cols = spectrogram.size() / rows;

   mMatrix.resize(rows, std::vector<double>(cols));

   for (int col = 0; col < cols; ++col) {
      for (int row = 0; row < rows; ++row) {
         size_t idx = col * rows + row;
         mMatrix[row][col] = spectrogram[idx];
      }
   }

   return true;
}

const float *SuperSpectrumAnalyst::GetProcessed() const
{
   return &mProcessed[0];
}

int SuperSpectrumAnalyst::GetProcessedSize() const
{
   return mProcessed.size() / 2;
}

float SuperSpectrumAnalyst::GetProcessedValue(float freq0, float freq1) const
{
   float bin0, bin1, binwidth;

   if (mAlg == Spectrum) {
      bin0 = freq0 * mWindowSize / mRate;
      bin1 = freq1 * mWindowSize / mRate;
   } else {
      bin0 = freq0 * mRate;
      bin1 = freq1 * mRate;
   }
   binwidth = bin1 - bin0;

   float value = float(0.0);

   if (binwidth < 1.0) {
      float binmid = (bin0 + bin1) / 2.0;
      int ibin = (int)(binmid) - 1;
      if (ibin < 1)
         ibin = 1;
      if (ibin >= GetProcessedSize() - 3)
         ibin = std::max(0, GetProcessedSize() - 4);

      value = CubicInterpolate(mProcessed[ibin],
                               mProcessed[ibin + 1],
                               mProcessed[ibin + 2],
                               mProcessed[ibin + 3], binmid - ibin);

   } else {
      if (bin0 < 0)
         bin0 = 0;
      if (bin1 >= GetProcessedSize())
         bin1 = GetProcessedSize() - 1;

      if ((int)(bin1) > (int)(bin0))
         value += mProcessed[(int)(bin0)] * ((int)(bin0) + 1 - bin0);
      bin0 = 1 + (int)(bin0);
      while (bin0 < (int)(bin1)) {
         value += mProcessed[(int)(bin0)];
         bin0 += 1.0;
      }
      value += mProcessed[(int)(bin1)] * (bin1 - (int)(bin1));

      value /= binwidth;
   }

   return value;
}

float SuperSpectrumAnalyst::FindPeak(float xPos, float *pY) const
{
   float bestpeak = 0.0f;
   float bestValue = 0.0;
   if (GetProcessedSize() > 1) {
      bool up = (mProcessed[1] > mProcessed[0]);
      float bestdist = 1000000;
      for (int bin = 3; bin < GetProcessedSize() - 1; bin++) {
         bool nowUp = mProcessed[bin] > mProcessed[bin - 1];
         if (!nowUp && up) {
            // Local maximum.  Find actual value by cubic interpolation
            int leftbin = bin - 2;
            /*
            if (leftbin < 1)
               leftbin = 1;
               */
            float valueAtMax = 0.0;
            float max = leftbin + CubicMaximize(mProcessed[leftbin],
                                                mProcessed[leftbin + 1],
                                                mProcessed[leftbin + 2],
                                                mProcessed[leftbin + 3],
                                                &valueAtMax);

            float thispeak;
            if (mAlg == Spectrum)
               thispeak = max * mRate / mWindowSize;
            else
               thispeak = max / mRate;

            if (fabs(thispeak - xPos) < bestdist) {
               bestpeak = thispeak;
               bestdist = fabs(thispeak - xPos);
               bestValue = valueAtMax;
               // Should this test come after the enclosing if?
               if (thispeak > xPos)
                  break;
            }
         }
         up = nowUp;
      }
   }

   if (pY)
      *pY = bestValue;
   return bestpeak;
}

// If f(0)=y0, f(1)=y1, f(2)=y2, and f(3)=y3, this function finds
// the degree-three polynomial which best fits these points and
// returns the value of this polynomial at a value x.  Usually
// 0 < x < 3

float SuperSpectrumAnalyst::CubicInterpolate(float y0, float y1, float y2, float y3, float x) const
{
   float a, b, c, d;

   a = y0 / -6.0 + y1 / 2.0 - y2 / 2.0 + y3 / 6.0;
   b = y0 - 5.0 * y1 / 2.0 + 2.0 * y2 - y3 / 2.0;
   c = -11.0 * y0 / 6.0 + 3.0 * y1 - 3.0 * y2 / 2.0 + y3 / 3.0;
   d = y0;

   float xx = x * x;
   float xxx = xx * x;

   return (a * xxx + b * xx + c * x + d);
}

float SuperSpectrumAnalyst::CubicMaximize(float y0, float y1, float y2, float y3, float * max) const
{
   // Find coefficients of cubic

   float a, b, c, d;

   a = y0 / -6.0 + y1 / 2.0 - y2 / 2.0 + y3 / 6.0;
   b = y0 - 5.0 * y1 / 2.0 + 2.0 * y2 - y3 / 2.0;
   c = -11.0 * y0 / 6.0 + 3.0 * y1 - 3.0 * y2 / 2.0 + y3 / 3.0;
   d = y0;

   // Take derivative

   float da, db, dc;

   da = 3 * a;
   db = 2 * b;
   dc = c;

   // Find zeroes of derivative using quadratic equation

   float discriminant = db * db - 4 * da * dc;
   if (discriminant < 0.0)
      return float(-1.0);              // error

   float x1 = (-db + sqrt(discriminant)) / (2 * da);
   float x2 = (-db - sqrt(discriminant)) / (2 * da);

   // The one which corresponds to a local _maximum_ in the
   // cubic is the one we want - the one with a negative
   // second derivative

   float dda = 2 * da;
   float ddb = db;

   if (dda * x1 + ddb < 0)
   {
      *max = a*x1*x1*x1+b*x1*x1+c*x1+d;
      return x1;
   }
   else
   {
      *max = a*x2*x2*x2+b*x2*x2+c*x2+d;
      return x2;
   }
}
