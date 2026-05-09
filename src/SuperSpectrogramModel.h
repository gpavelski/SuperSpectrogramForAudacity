/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramModel.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_MODEL__
#define __SUPER_SPECTROGRAM_MODEL__

#include <memory>
#include "SuperSpectrogramFrame.h"

class STFTProcessor;

class SuperSpectrogramModel
{
public:
   SuperSpectrogramModel();

   struct Parameters {
      int noiseFloor;
      int detailLevel;
   };

   SuperSpectrogramFrame ComputeFrame(
      const float* data,
      size_t len,
      double rate,
      const Parameters& params
   );

   const std::vector<std::vector<double>>& GetMatrix() const;
   double GetMaxFreq() const;
   size_t GetNumSamples() const;

private:

   void Compute(
      const float* data,
      size_t len,
      double rate,
      const Parameters& params
   );

   std::vector<std::vector<double>> mMatrix;
   double mMaxFreq{};
   size_t mNumSamples{};

   static double MapDetailToTargetRate(size_t detailLevel)
   {
      static const std::unordered_map<size_t, double> map = {
         {4, 551.25},
         {5, 1102.5},
         {6, 2205.0},
         {7, 4410.0},
         {8, 8820.0}
      };

      auto it = map.find(detailLevel);
      if (it == map.end())
         throw std::invalid_argument("Invalid detail level");

      return it->second;
   }
};

#endif
