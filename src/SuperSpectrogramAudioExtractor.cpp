/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAudioExtractor.cpp

  Guilherme Pavelski
  Dominic Mazzoni
  Matthieu Hodgkinson split from FreqWindow.cpp

**********************************************************************/
#include "SuperSpectrogramAudioExtractor.h"
#include "SuperSpectrogramConstants.h"
#include "BasicUI.h"
#include "Prefs.h"
#include "SampleFormat.h"
#include "ViewInfo.h"
#include "WaveTrack.h"

SuperSpectrogramAudioExtractor::SuperSpectrogramAudioExtractor(AudacityProject& project)
   : mProject{ &project }
{
}

SuperSpectrogramAudioExtractor::~SuperSpectrogramAudioExtractor() = default;

SuperSpectrogramAudioExtractor::AudioExtractionResult SuperSpectrogramAudioExtractor::Extract()
{
   AudioExtractionResult result;

   int selcount = 0;
   bool maxDataLenWarning = false;

   for (auto track : TrackList::Get(*mProject).Selected<const WaveTrack>())
   {
      auto& selectedRegion = ViewInfo::Get(*mProject).selectedRegion;
      auto start = track->TimeToLongSamples(selectedRegion.t0());

      if (selcount == 0)
      {
         result.rate = track->GetRate();

         if (result.rate < SuperSpectrogramConstants::Audio::kMaxTargetRate)
            return { AudioExtractionResult::Status::RateTooLow };

         auto end = track->TimeToLongSamples(selectedRegion.t1());
         auto dataLen = end - start;
         size_t maxDataLen = ComputeMaxSamples(result.rate);
         size_t minDataLen = ComputeMinSamples(result.rate);

         if (dataLen > maxDataLen)
         {
            maxDataLenWarning = true;
            result.length = maxDataLen;
         }
         else if (dataLen < minDataLen)
         {
            return { AudioExtractionResult::Status::TooShort };
         }
         else
         {
            result.length = dataLen.as_size_t();
         }

         result.data = Floats{ result.length };
      }

      const auto nChannels = track->NChannels();

      if (track->GetRate() != result.rate)
         return { AudioExtractionResult::Status::MismatchedSampleRate };

      Floats buffer1{ result.length };
      Floats buffer2{ result.length };
      float* const buffers[]{ buffer1.get(), buffer2.get() };

      if (!track->GetFloats(0, nChannels, buffers, start, result.length, false,
         FillFormat::fillZero, false))
      {
         return { AudioExtractionResult::Status::ReadError };
      }

      size_t iChannel = 0;
      if (selcount == 0)
      {
         for (size_t i = 0; i < result.length; i++)
            (*result.data)[i] = buffers[0][i];
         ++iChannel;
      }

      for (; iChannel < nChannels; ++iChannel)
      {
         const auto buffer = buffers[iChannel];
         for (size_t i = 0; i < result.length; i++)
            (*result.data)[i] += buffer[i];
      }

      ++selcount;
   }

   if (selcount == 0)
      return { AudioExtractionResult::Status::NoSelection };

   if (selcount > 0)
   {
      float divisor = static_cast<float>(selcount + 1);
      for (size_t i = 0; i < result.length; i++)
         (*result.data)[i] /= divisor;
   }

   result.trackCount = selcount;

   if (maxDataLenWarning)
      result.status = AudioExtractionResult::Status::Truncated;

   return result;
}

size_t SuperSpectrogramAudioExtractor::ComputeMaxSamples(double rate)
{
   size_t maxSamplesAfterDecimation = static_cast<size_t>(SuperSpectrogramConstants::Audio::kMaxProcessingTime * SuperSpectrogramConstants::Audio::kMaxTargetRate);
   double minDecimationRatio = rate / SuperSpectrogramConstants::Audio::kMaxTargetRate;
   size_t maxNumberOfSamples = static_cast<size_t>(maxSamplesAfterDecimation * minDecimationRatio);

   return maxNumberOfSamples;
}

// At least one window should be computed for a decimated rate of kMaxTargetRate Hz;
// That would represent 2 windows at kMaxTargetRate/2 Hz, 4 windows at kMaxTargetRate/4 Hz and so on.
size_t SuperSpectrogramAudioExtractor::ComputeMinSamples(double rate)
{
   double minDecimationRatio = rate / SuperSpectrogramConstants::Audio::kMaxTargetRate;
   size_t minNumberOfSamples = static_cast<size_t>(SuperSpectrogramConstants::Audio::kMaxWindowSize * minDecimationRatio);

   return minNumberOfSamples;
}
