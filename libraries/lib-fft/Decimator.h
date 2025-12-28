#ifndef DECIMATOR_H
#define DECIMATOR_H

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>

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
   // Internal template implementation
   template<typename T>
   std::vector<double> processImpl(const T* signal, size_t length)
   {
      const double start = initial_offset + filter_order;

      // Estimate output length
      const size_t outputSize = static_cast<size_t>(length / step);

      std::vector<double> X(length + filter_order + 2, 0.0); // padded
      std::vector<double> Y(length + filter_order + 2, 0.0);
      std::vector<double> output;
      output.reserve(outputSize);

      // Copy & shift into padded buffer
      for (size_t i = 0; i < length; ++i)
         X[i + filter_order + 1] = static_cast<double>(signal[i]);

      // Filter everything (as before; unchanged)
      for (size_t i = filter_order + 1; i < X.size(); ++i) {
         Y[i] = b_coeffs[0] * X[i];
         for (size_t j = 1; j <= filter_order; ++j) {
            Y[i] += b_coeffs[j] * X[i - j];
            Y[i] -= a_coeffs[j] * Y[i - j];
         }
      }

      // FRACTIONAL DECIMATION: sample at non-integer positions
      double pos = start;
      for (size_t n = 0; n < outputSize; ++n) {
         if (pos + 1 >= Y.size())
            break;

         const double y = linearInterp(Y, pos);
         output.push_back((y > -1.0 && y < 1.0) ? y : 0.0);

         pos += step;
      }

      return output;
   }

   inline double linearInterp(const std::vector<double>& buf, double idx) {
      size_t i = (size_t)idx;
      double frac = idx - i;
      return buf[i] * (1.0 - frac) + buf[i + 1] * frac;
   }

   double computeDecimationLevel(
      double inputRate,
      double targetRate
   ) const;

   const std::vector<double> b_coeffs{
       3.58632432538361e-09 , 2.86905946030689e-08 , 1.00417081110741e-07 ,
       2.00834162221482e-07 , 2.51042702776853e-07 , 2.00834162221482e-07 ,
       1.00417081110741e-07 , 2.86905946030689e-08 , 3.58632432538361e-09
   };

   const std::vector<double> a_coeffs{
       1 , -7.39772047094363 , 24.0727277609670 ,
       -44.9989146659833 , 52.8434667669225 , -39.9159143524685 ,
       18.9376658097605 , -5.15917942637568 , 0.617869501520364
   };

   const int filter_order = 8;
   const int initial_offset = 32;
   double step;
};

#endif // DECIMATOR_H
