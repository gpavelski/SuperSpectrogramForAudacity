/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramModel.cpp

  Guilherme Pavelski

**********************************************************************/

#include "Model/SuperSpectrogramModel.h"

SuperSpectrogramModel::SuperSpectrogramModel()
{
}

//------------------------------------------------------------
// Core computation (moved from Recalc)
//------------------------------------------------------------
void SuperSpectrogramModel::Compute(
   const float* data,
   size_t len,
   double rate,
   const Parameters& params)
{
   if (!data || len == 0)
      return;

   const double targetRate = MapDetailToTargetRate(params.detailLevel);

   // --- Decimation ---
   Decimator decimator(rate, targetRate);
   auto decimatedSignal = decimator.process(data, len);

   // --- STFT ---
   STFTProcessor stftProcessor(params.detailLevel);
   stftProcessor.setLowerThreshold(params.noiseFloor);

   mMatrix = stftProcessor.processFullSTFTMatrix(decimatedSignal);

   // --- Metadata ---
   mMaxFreq = targetRate / 2.0;
   mNumSamples = stftProcessor.getResizedSignalLength();
}

SuperSpectrogramFrame SuperSpectrogramModel::ComputeFrame(
   const float* data,
   size_t len,
   double rate,
   const Parameters& params)
{
   Compute(data, len, rate, params);

   SuperSpectrogramFrame frame;
   frame.matrix = GetMatrix();
   frame.maxFreq = GetMaxFreq();
   frame.numSamples = GetNumSamples();

   return frame;
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
