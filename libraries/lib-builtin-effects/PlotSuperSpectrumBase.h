/**********************************************************************

  Audacity: A Digital Audio Editor

  PlotSuperSpectrumBase.h

  Tony Bee
  Dominic Mazzoni
  Matthieu Hodgkinson split from FreqWindow.h

**********************************************************************/
#pragma once

#include "MemoryX.h"
#include "SuperSpectrumAnalyst.h"
#include <memory>

class AudacityProject;

class BUILTIN_EFFECTS_API PlotSuperSpectrumBase
{
public:
   PlotSuperSpectrumBase(AudacityProject& project);

protected:
   bool GetAudio();

   AudacityProject* mProject;
   std::unique_ptr<SuperSpectrumAnalyst> mAnalyst;

   bool mDrawGrid;
   int mSize;
   SuperSpectrumAnalyst::Algorithm mAlg;
   int mFunc;
   int mAxis;
   double mRate;
   size_t mDataLen;
   ArrayOf<float> mData;
};
