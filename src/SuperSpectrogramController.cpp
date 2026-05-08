/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramController.h"
#include <fstream>

SuperSpectrogramController::SuperSpectrogramController(
   SuperSpectrogramAudioExtractor& extractor,
   SuperSpectrogramConfig& config,
   SuperSpectrogramExportService& exportService,
   SuperSpectrogramModel& model,
   SuperSpectrogramView& view)
   : mExtractor(extractor)
   , mConfig(config)
   , mExportService(exportService)
   , mModel(model)
   , mView(view)
{
}

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------
bool SuperSpectrogramController::Initialize()
{
   mConfig.Load();

   UpdateModelParameters();
   mView.ApplyConfig(mConfig);

   LoadAudioFromProject();

   if (!mCurrentData || mCurrentLen == 0)
      return false;

   Recompute(mCurrentData, mCurrentLen, mCurrentRate);

   return true;
}

void SuperSpectrogramController::LoadAudioFromProject()
{
   auto audio = mExtractor.Extract();

   if (!audio)
      return;

   // Take ownership of the buffer
   mOwnedData = std::move(audio->data);

   mCurrentData = mOwnedData.get();
   mCurrentLen = audio->length;
   mCurrentRate = audio->rate;
}

//------------------------------------------------------------
// Internal helpers
//------------------------------------------------------------
void SuperSpectrogramController::UpdateModelParameters()
{
   SuperSpectrogramModel::Parameters p;
   p.noiseFloor = mConfig.noiseFloor;
   p.detailLevel = mConfig.detailLevel;

   mModel.SetParameters(p);
}

//------------------------------------------------------------
// Recompute (core orchestration)
//------------------------------------------------------------
void SuperSpectrogramController::Recompute(
   const float* data,
   size_t len,
   double rate)
{
   if (!data || len == 0)
      return;

   UpdateModelParameters();

   mModel.Compute(data, len, rate);

   auto matrix = mModel.GetMatrix();
   auto maxFreq = mModel.GetMaxFreq();
   auto numSamples = mModel.GetNumSamples();

   mView.SetSpectrogramData(matrix, maxFreq, numSamples);
}

void SuperSpectrogramController::UpdateView()
{
   mView.ApplyConfig(mConfig);
}

//------------------------------------------------------------
// Event Handlers
//------------------------------------------------------------
void SuperSpectrogramController::ApplyConfigChange(
   const SuperSpectrogramConfig& newConfig)
{
   ConfigDiff diff = ComputeDiff(mConfig, newConfig);

   mConfig = newConfig;
   mConfig.Save();

   if (diff.needsRecompute)
      Recompute(mCurrentData, mCurrentLen, mCurrentRate);
      UpdateLayoutPreservingState();

   if (diff.needsViewUpdate)
      UpdateView();
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
   mExportService.ExportMatrixAsText(mModel.GetMatrix(), path);
}

void SuperSpectrogramController::ExportCurrentView(const std::string& path)
{
   wxBitmap bmp = mView.RenderToBitmap();
   mExportService.ExportViewAsPNG(bmp, path);
}

void SuperSpectrogramController::SetAudioData(const float* data, size_t len, double rate)
{
   mCurrentData = data;
   mCurrentLen = len;
   mCurrentRate = rate;
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
