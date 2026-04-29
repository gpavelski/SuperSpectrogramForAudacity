/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramAudioExtractor.cpp

  Guilherme Pavelski
  Dominic Mazzoni
  Matthieu Hodgkinson split from FreqWindow.cpp

**********************************************************************/
#include "SuperSpectrogramAudioExtractor.h"
#include "BasicUI.h"
#include "Prefs.h"
#include "SampleFormat.h"
#include "ViewInfo.h"
#include "WaveTrack.h"

SuperSpectrogramAudioExtractor::SuperSpectrogramAudioExtractor(AudacityProject& project)
   : mProject{ &project }
   , mAnalyst(std::make_unique<SuperSpectrogramAnalyst>())
{
}

SuperSpectrogramAudioExtractor::~SuperSpectrogramAudioExtractor() = default;

std::optional<SuperSpectrogramAudioExtractor::AudioData> SuperSpectrogramAudioExtractor::Extract()
{
   AudioData result;

   int selcount = 0;
   bool maxDataLenWarning = false;
   bool minDataLenWarning = false;
   for (auto track : TrackList::Get(*mProject).Selected<const WaveTrack>())
   {
      auto& selectedRegion = ViewInfo::Get(*mProject).selectedRegion;
      auto start = track->TimeToLongSamples(selectedRegion.t0());

      if (selcount == 0)
      {
         result.rate = track->GetRate();
         if (result.rate < maxTargetRate)
         {
            using namespace BasicUI;
            ShowMessageBox(
               XO("The signal sampling rate is too low. Minimum sampling rate: %.2f Hz")
               .Format(maxTargetRate),
               MessageBoxOptions{}.Caption(XO("Error")).IconStyle(Icon::Error));

            result.data.reset();
            result.length = 0;

            return std::nullopt;
         }

         auto end = track->TimeToLongSamples(selectedRegion.t1());
         auto dataLen = end - start;
         size_t maxDataLen = ComputeMaxSamples(result.rate);
         size_t minDataLen = ComputeMinSamples(result.rate);

         // Permit 30 seconds of processing at a decimated sample rate of 8.82 kHz
         if (dataLen > maxDataLen)
         {
            maxDataLenWarning = true;
            result.length = maxDataLen;
         }
         // At least one window should be calculated at decimated sample rate of 8.82 kHz
         else if (dataLen < minDataLen)
         {
            using namespace BasicUI;
            ShowMessageBox(
               XO("To plot the spectrogram, at least %zu samples must be selected.")
               .Format(minDataLen),
               MessageBoxOptions{}.Caption(XO("Error")).IconStyle(Icon::Error));

            result.data.reset();
            result.length = 0;

            return std::nullopt;
         }
         else
         {
            result.length = dataLen.as_size_t();
         }
         result.data = Floats{ result.length };
      }

      const auto nChannels = track->NChannels();

      if (track->GetRate() != result.rate)
      {
         using namespace BasicUI;
         ShowMessageBox(
            XO("To plot the spectrogram, all selected tracks must have the same sample rate."),
            MessageBoxOptions{}.Caption(XO("Error")).IconStyle(Icon::Error));

         result.data.reset();
         result.length = 0;

         return std::nullopt;
      }

      Floats buffer1{ result.length };
      Floats buffer2{ result.length };
      float* const buffers[]{
         buffer1.get(),
         buffer2.get()
      };
      // Don't allow throw for bad reads
      if (!track->GetFloats(
         0, nChannels, buffers, start, result.length, false,
         FillFormat::fillZero, false))
      {
         using namespace BasicUI;
         ShowMessageBox(
            XO("Audio could not be analyzed. This may be due to a stretched or pitch-shifted clip.\nTry resetting any stretched clips, or mixing and rendering the tracks before analyzing"),
            MessageBoxOptions{}.Caption(XO("Error")).IconStyle(Icon::Error));

         result.data.reset();
         result.length = 0;

         return std::nullopt;
      }

      size_t iChannel = 0;
      if (selcount == 0)
      {
         // First channel -- assign into mData
         for (size_t i = 0; i < result.length; i++)
            result.data[i] = buffers[0][i];
         ++iChannel;
      }
      // Later channels -- accumulate
      for (; iChannel < nChannels; ++iChannel)
      {
         const auto buffer = buffers[iChannel];
         for (size_t i = 0; i < result.length; i++)
            result.data[i] += buffer[i];
      }
      ++selcount;
   }

   if (selcount == 0)
      return std::nullopt;

   if (selcount > 0) {
      float divisor = static_cast<float>(selcount+1);
      for (size_t i = 0; i < result.length; i++)
         result.data[i] /= divisor;
   }

   if (maxDataLenWarning)
   {
      auto msg =
         XO("Too much audio was selected. Only the first %.1f seconds of audio will be analyzed.")
         .Format(result.length / result.rate);
      BasicUI::ShowMessageBox(msg);
   }
   return result;
}

size_t SuperSpectrogramAudioExtractor::ComputeMaxSamples(double rate)
{
   size_t maxSamplesAfterDecimation = static_cast<size_t>(maxProcessingTime * maxTargetRate);
   double minDecimationRatio = rate / maxTargetRate;
   size_t maxNumberOfSamples = static_cast<size_t>(maxSamplesAfterDecimation * minDecimationRatio);

   return maxNumberOfSamples;
}

// At least one window should be computed for a decimated rate of 8820 Hz;
// That would represent 2 windows at 4410 Hz, 4 windows at 2205 Hz and so on.
size_t SuperSpectrogramAudioExtractor::ComputeMinSamples(double rate)
{
   double minDecimationRatio = rate / maxTargetRate;
   size_t minNumberOfSamples = static_cast<size_t>(maxWindowSize * minDecimationRatio);

   return minNumberOfSamples;
}
