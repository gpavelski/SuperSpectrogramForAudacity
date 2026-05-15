/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAudioExtractor.h

  Guilherme Pavelski
  Dominic Mazzoni
  Matthieu Hodgkinson split from FreqWindow.h

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_AUDIO_EXTRACTOR__
#define __SUPER_SPECTROGRAM_AUDIO_EXTRACTOR__

#include <memory>
#include "MemoryX.h"
#include "Config/SuperSpectrogramConstants.h"
#include "BasicUI.h"
#include "Prefs.h"
#include "SampleFormat.h"
#include "ViewInfo.h"
#include "WaveTrack.h"

class AudacityProject;

class SuperSpectrogramAudioExtractor
{
public:
   SuperSpectrogramAudioExtractor(
      AudacityProject& project
   );
   ~SuperSpectrogramAudioExtractor();

   struct AudioExtractionResult
   {
      enum class Status
      {
         Success,
         NoSelection,
         RateTooLow,
         MismatchedSampleRate,
         TooShort,
         ReadError,
         Truncated
      };

      Status status = Status::Success;
      std::optional<Floats> data;
      size_t length = 0;
      double rate = 0.0;
      size_t trackCount = 0;
   };

   AudioExtractionResult Extract();

private:
   size_t ComputeMaxSamples(double rate);
   size_t ComputeMinSamples(double rate);

   AudacityProject* mProject;
};

#endif
