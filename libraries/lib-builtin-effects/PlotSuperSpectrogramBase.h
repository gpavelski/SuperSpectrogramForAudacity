/**********************************************************************

  Audacity: A Digital Audio Editor

  PlotSuperSpectrogramBase.h

  Tony Bee
  Dominic Mazzoni
  Matthieu Hodgkinson split from FreqWindow.h

**********************************************************************/
#pragma once

#include "MemoryX.h"
#include "SuperSpectrogramAnalyst.h"
#include <memory>

class AudacityProject;

class BUILTIN_EFFECTS_API PlotSuperSpectrogramBase
{
public:
   PlotSuperSpectrogramBase(AudacityProject& project);

protected:
   bool GetAudio();
   size_t ComputeMaxSamples();
   size_t ComputeMinSamples();

   AudacityProject* mProject;
   std::unique_ptr<SuperSpectrogramAnalyst> mAnalyst;

   double mRate;
   size_t mDataLen;
   ArrayOf<float> mData;
   double maxProcessingTime = 30.0;
   double maxTargetRate = 8820.0;
   size_t maxWindowSize = 4096;
};
