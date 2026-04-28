#ifndef __SUPER_SPECTROGRAM_MODEL__
#define __SUPER_SPECTROGRAM_MODEL__

#include <memory>
#include "SuperSpectrogramAnalyst.h"

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

   void Compute(const float* data, size_t len, double rate);

   const std::vector<std::vector<double>>& GetMatrix() const;
   double GetMaxFreq() const;
   size_t GetNumSamples() const;

private:
   Parameters mParams{};

   std::vector<std::vector<double>> mMatrix;
   double mMaxFreq{};
   size_t mNumSamples{};

   std::unique_ptr<SuperSpectrogramAnalyst> mAnalyst;
};

#endif
