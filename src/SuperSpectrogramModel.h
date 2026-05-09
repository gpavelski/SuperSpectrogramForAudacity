/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramModel.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_MODEL__
#define __SUPER_SPECTROGRAM_MODEL__

#include <memory>
#include "SuperSpectrogramAnalyst.h"
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

   void SetParameters(const Parameters& p);
   const Parameters& GetParameters() const;

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
   Parameters mParams{};

   void Compute(
      const float* data,
      size_t len,
      double rate,
      const Parameters& params
   );

   std::vector<std::vector<double>> mMatrix;
   double mMaxFreq{};
   size_t mNumSamples{};

   std::unique_ptr<SuperSpectrogramAnalyst> mAnalyst;
};

#endif
