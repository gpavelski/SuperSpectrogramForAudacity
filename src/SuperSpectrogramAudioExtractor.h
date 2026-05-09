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
#include <memory>

class AudacityProject;

class BUILTIN_EFFECTS_API SuperSpectrogramAudioExtractor
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
      size_t trackCount = 0;        // Number of tracks processed
   };

   AudioExtractionResult Extract();

private:
   size_t ComputeMaxSamples(double rate);
   size_t ComputeMinSamples(double rate);

   AudacityProject* mProject;

};

#endif
