/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAudioExtractor.h

  Guilherme Pavelski
  Dominic Mazzoni
  Matthieu Hodgkinson split from FreqWindow.h

**********************************************************************/
#ifndef __SUPER_SPECTROGRAM_AUDIO_EXTRACTOR__
#define __SUPER_SPECTROGRAM_AUDIO_EXTRACTOR__

#include "MemoryX.h"
#include "SuperSpectrogramAnalyst.h"
#include <memory>

class AudacityProject;

class BUILTIN_EFFECTS_API SuperSpectrogramAudioExtractor
{
public:
   SuperSpectrogramAudioExtractor(
      AudacityProject& project
   );
   ~SuperSpectrogramAudioExtractor();

   struct AudioData
   {
      ArrayOf<float> data;
      size_t length = 0;
      double rate = 0.0;
   };

   std::optional<AudioData> Extract();

private:
   size_t ComputeMaxSamples(double rate);
   size_t ComputeMinSamples(double rate);

   AudacityProject* mProject;
   std::unique_ptr<SuperSpectrogramAnalyst> mAnalyst;

   double maxProcessingTime = 30.0;
   double maxTargetRate = 8820.0;
   size_t maxWindowSize = 4096;
};

#endif
