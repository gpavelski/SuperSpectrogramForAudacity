/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramModel.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <memory>
#include "Config/SuperSpectrogramConstants.h"
#include "View/SuperSpectrogramFrame.h"
#include "Model/STFTProcessor.h"

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
      auto it = SuperSpectrogramConstants::Model::kDetailToTargetRate.find(detailLevel);
      if (it == SuperSpectrogramConstants::Model::kDetailToTargetRate.end())
         throw std::invalid_argument("Invalid detail level");

      return it->second;
   }
};

