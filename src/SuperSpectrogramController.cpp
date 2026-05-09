/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramController.h"
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
   auto& config = mSession.GetConfig();
   config.Load();

   mView.ApplyConfig(config);

   auto audio = mExtractor.Extract();
   if (!audio)
      return false;

   mSession.SetAudio(
      std::move(audio->data),
      audio->length,
      audio->rate
   );

   Recompute();

   return true;
}

//------------------------------------------------------------
// Recompute (core orchestration)
//------------------------------------------------------------
void SuperSpectrogramController::Recompute()
{
   const auto& cfg = mSession.GetConfig();

   SuperSpectrogramModel::Parameters p;
   p.noiseFloor = cfg.noiseFloor;
   p.detailLevel = cfg.detailLevel;

   auto frame = mModel.ComputeFrame(
      mSession.GetAudioData(),
      mSession.GetAudioLength(),
      mSession.GetSampleRate(),
      p
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
   mExportService.ExportMatrixAsText(mModel.GetMatrix(), path);
}

void SuperSpectrogramController::ExportCurrentView(const std::string& path)
{
   wxBitmap bmp = mView.RenderToBitmap();
   mExportService.ExportViewAsPNG(bmp, path);
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
