/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramController.h"
#include "SuperSpectrogramAudioExtractor.h"
#include <fstream>

SuperSpectrogramController::SuperSpectrogramController(
   SuperSpectrogramAudioExtractor& extractor,
   SuperSpectrogramExportService& exportService,
   SuperSpectrogramModel& model,
   SuperSpectrogramSession& session,
   SuperSpectrogramView& view)
   : mExtractor(extractor)
   , mExportService(exportService)
   , mModel(model)
   , mSession(session)
   , mView(view)
{
}

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------
bool SuperSpectrogramController::Initialize()
{
   auto audioResult = mExtractor.Extract();

   switch (audioResult.status)
   {
   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::Success:
      break;

   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::RateTooLow:
      mView.ShowError("The signal sampling rate is too low. Minimum sampling rate: 8820 Hz");
      return false;

   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::TooShort:
      mView.ShowError("To plot the spectrogram, at least the minimum number of samples must be selected.");
      return false;

   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::MismatchedSampleRate:
      mView.ShowError("All selected tracks must have the same sample rate.");
      return false;

   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::ReadError:
      mView.ShowError(
         "Audio could not be analyzed. This may be due to a stretched or pitch-shifted clip.\n"
         "Try resetting any stretched clips, or mixing and rendering the tracks before analyzing."
      );
      return false;

   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::Truncated:
      mView.ShowWarning(
         "Too much audio was selected. Only the first portion will be analyzed."
      );
      break;

   case SuperSpectrogramAudioExtractor::AudioExtractionResult::Status::NoSelection:
      mView.ShowWarning("No tracks are selected for analysis.");
      return false;
   }

   if (!mSession.InitializeAudio()) {
      return false;
   }

   mView.ApplyConfig(mSession.GetConfig());
   Recompute();
   return true;
}

//------------------------------------------------------------
// Recompute (core orchestration)
//------------------------------------------------------------
void SuperSpectrogramController::Recompute()
{
   const auto& cfg = mSession.GetConfig();

   SuperSpectrogramModel::Parameters params;
   params.noiseFloor = cfg.noiseFloor;
   params.detailLevel = cfg.detailLevel;

   const auto& audio = mSession.GetAudio();

   auto frame = mModel.ComputeFrame(
      audio.ptr(),
      audio.length,
      audio.rate,
      params
   );

   mView.Render(frame);
}

//------------------------------------------------------------
// Event Handlers
//------------------------------------------------------------
void SuperSpectrogramController::ApplyConfigChange(
   const SuperSpectrogramConfig& newConfig)
{
   auto& current = mSession.GetConfig();

   const bool needsRecompute =
      RequiresRecompute(current, newConfig);

   current = newConfig;
   current.Save();

   if (needsRecompute)
      Recompute();

   mView.ApplyConfig(current);
}

SuperSpectrogramController::ConfigDiff
SuperSpectrogramController::ComputeDiff(
   const SuperSpectrogramConfig& oldCfg,
   const SuperSpectrogramConfig& newCfg)
{
   ConfigDiff d;

   // compute-affecting
   if (oldCfg.noiseFloor != newCfg.noiseFloor ||
      oldCfg.detailLevel != newCfg.detailLevel)
   {
      d.needsRecompute = true;
   }

   // visual-only
   if (oldCfg.colormap != newCfg.colormap ||
      oldCfg.noteNaming != newCfg.noteNaming ||
      oldCfg.showNoteLines != newCfg.showNoteLines ||
      oldCfg.timeTickMode != newCfg.timeTickMode)
   {
      d.needsViewUpdate = true;
   }

   return d;
}

void SuperSpectrogramController::UpdateLayoutPreservingState()
{
   // Recalculate layout while preserving maximized state.
   // Avoids unexpected resizing when user has maximized the window.
   const bool wasMaximized = mView.IsMaximized();

   if (!wasMaximized) {
      mView.ApplyDataDrivenMinSize();
      mView.Layout();
      mView.Fit();
      mView.Centre();
   }
   else {
      mView.Layout();
      // Explicitly re-maximize to guard against platform quirks
      mView.Maximize(true);
   }
}

void SuperSpectrogramController::ExportMatrix(const std::string& path)
{
   mView.SetExportEnabled(false);

   auto matrixCopy = mModel.GetMatrix();

   mExportTask = std::async(std::launch::async,
      [this, matrixCopy = std::move(matrixCopy), path]()
      {
         mExportService.ExportMatrixAsText(matrixCopy, path);

         mView.CallAfter([this]() {
            mView.SetExportEnabled(true);
         });
      }
   );
}

void SuperSpectrogramController::ExportCurrentView(const std::string& path)
{
   // UI thread: disable button
   mView.SetExportEnabled(false);

   // UI thread: capture bitmap
   wxBitmap bmp = mView.RenderToBitmap();

   // Copy bitmap (safe for thread use)
   wxBitmap bmpCopy = bmp;

   mExportTask = std::async(std::launch::async,
      [this, bmpCopy, path]()
      {
         mExportService.ExportViewAsPNG(bmpCopy, path);

         mView.CallAfter([this]() {
            mView.SetExportEnabled(true);
         });
      }
   );
}

void SuperSpectrogramController::BindView()
{
   mView.NotifyConfigChanged = [this](const SuperSpectrogramConfig& cfg)
      {
         ApplyConfigChange(cfg);
      };

   mView.NotifyExportRequested =
      [this](int format, const std::string& path)
      {
         if (format == 0)
            ExportMatrix(path);
         else
            ExportCurrentView(path);
      };
}

bool SuperSpectrogramController::RequiresRecompute(
   const SuperSpectrogramConfig& oldCfg,
   const SuperSpectrogramConfig& newCfg)
{
   return
      oldCfg.noiseFloor != newCfg.noiseFloor ||
      oldCfg.detailLevel != newCfg.detailLevel;
}
