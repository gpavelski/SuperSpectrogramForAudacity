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

   AudacityProject* mProject;
   std::unique_ptr<SuperSpectrogramAnalyst> mAnalyst;

   int mAxis;
   double mRate;
   size_t mDataLen;
   ArrayOf<float> mData;
};
