#include "SuperSpectrogramModel.h"
#include "STFTProcessor.h"

SuperSpectrogramModel::SuperSpectrogramModel()
{
   mAnalyst = std::make_unique<SuperSpectrogramAnalyst>();
}

//------------------------------------------------------------
// Parameters
//------------------------------------------------------------
void SuperSpectrogramModel::SetParameters(const Parameters& p)
{
   mParams = p;
}

const SuperSpectrogramModel::Parameters&
SuperSpectrogramModel::GetParameters() const
{
   return mParams;
}

//------------------------------------------------------------
// Core computation (moved from Recalc)
//------------------------------------------------------------
void SuperSpectrogramModel::Compute(
   const float* data,
   size_t len,
   double rate)
{
   if (!data || len == 0 || !mAnalyst)
      return;

   mAnalyst->Calculate(
      data,
      len,
      mParams.detailLevel,
      rate,
      mParams.noiseFloor);

   mMatrix = mAnalyst->GetMatrix();
   mMaxFreq = mAnalyst->GetTargetRate() / 2.0;
   mNumSamples = mAnalyst->GetSignalLength();
}

//------------------------------------------------------------
// Outputs
//------------------------------------------------------------
const std::vector<std::vector<double>>&
SuperSpectrogramModel::GetMatrix() const
{
   return mMatrix;
}

double SuperSpectrogramModel::GetMaxFreq() const
{
   return mMaxFreq;
}

size_t SuperSpectrogramModel::GetNumSamples() const
{
   return mNumSamples;
}
