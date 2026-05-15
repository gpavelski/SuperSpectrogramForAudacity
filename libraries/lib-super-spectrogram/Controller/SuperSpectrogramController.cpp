/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.cpp

  Guilherme Pavelski

**********************************************************************/

#include "Controller/SuperSpectrogramController.h"
#include "View/SuperSpectrogramSession.h"

SuperSpectrogramController::SuperSpectrogramController(
   SuperSpectrogramAudioExtractor& extractor,
   SuperSpectrogramExportService& exportService,
   SuperSpectrogramModel& model,
   SuperSpectrogramSession& session,
   SuperSpectrogramView& view)
   : m_extractor(extractor)
   , m_exportService(exportService)
   , m_model(model)
   , m_session(session)
   , m_view(view)
{
}

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------
bool SuperSpectrogramController::Initialize()
{
   const auto result = m_extractor.Extract();

   using Status = SuperSpectrogramAudioExtractor::AudioExtractionResult::Status;

   switch (result.status)
   {
   case Status::Success:
      break;

   case Status::RateTooLow:
      m_view.ShowError("Sampling rate too low (min 8820 Hz)");
      return false;

   case Status::TooShort:
      m_view.ShowError("Not enough samples selected.");
      return false;

   case Status::MismatchedSampleRate:
      m_view.ShowError("Tracks must share same sample rate.");
      return false;

   case Status::ReadError:
      m_view.ShowError(
         "Audio analysis failed. Try rendering or resetting processing."
      );
      return false;

   case Status::Truncated:
      m_view.ShowWarning("Audio truncated for analysis.");
      break;

   case Status::NoSelection:
      m_view.ShowWarning("No audio selected.");
      return false;
   }

   if (!m_session.InitializeAudio())
      return false;

   m_view.ApplyConfig(m_session.GetConfig());
   Recompute();

   return true;
}

//------------------------------------------------------------
// Recompute (core orchestration)
//------------------------------------------------------------
void SuperSpectrogramController::Recompute()
{
   const auto& cfg = m_session.GetConfig();
   const auto& audio = m_session.GetAudio();

   SuperSpectrogramModel::Parameters params;
   params.noiseFloor = cfg.noiseFloor;
   params.detailLevel = cfg.detailLevel;

   auto frame = m_model.ComputeFrame(
      audio.ptr(),
      audio.length,
      audio.rate,
      params
   );

   m_view.Render(frame);
}

//------------------------------------------------------------
// Event Handlers
//------------------------------------------------------------
void SuperSpectrogramController::ApplyConfigChange(
   const SuperSpectrogramConfig& newConfig)
{
   auto& current = m_session.GetConfig();

   const auto diff = ComputeDiff(current, newConfig);

   current = newConfig;
   current.Save();

   if (diff.needsRecompute)
      Recompute();

   if (diff.needsViewUpdate)
      m_view.ApplyConfig(current);
}

SuperSpectrogramController::ConfigDiff
SuperSpectrogramController::ComputeDiff(
   const SuperSpectrogramConfig& oldCfg,
   const SuperSpectrogramConfig& newCfg
) const
{
   ConfigDiff d;

   d.needsRecompute =
      oldCfg.noiseFloor != newCfg.noiseFloor ||
      oldCfg.detailLevel != newCfg.detailLevel;

   d.needsViewUpdate =
      oldCfg.colormapId != newCfg.colormapId ||
      oldCfg.noteNaming != newCfg.noteNaming ||
      oldCfg.showNoteLines != newCfg.showNoteLines ||
      oldCfg.timeTickMode != newCfg.timeTickMode;

   return d;
}

void SuperSpectrogramController::UpdateViewLayout()
{
   const bool wasMaximized = m_view.IsMaximized();

   if (!wasMaximized)
   {
      m_view.ApplyDataDrivenMinSize();
      m_view.Layout();
      m_view.Fit();
      m_view.Centre();
   }
   else
   {
      m_view.Layout();
      m_view.Maximize(true);
   }
}

void SuperSpectrogramController::ExportMatrix(const std::string& path)
{
   m_view.SetExportEnabled(false);

   auto matrixCopy = m_model.GetMatrix();

   m_exportTask = std::async(std::launch::async,
      [this, matrixCopy = std::move(matrixCopy), path]()
      {
         m_exportService.ExportMatrixAsText(matrixCopy, path);

         m_view.CallAfter([this]()
            {
               m_view.SetExportEnabled(true);
            });
      }
   );
}

void SuperSpectrogramController::ExportCurrentView(const std::string& path)
{
   // UI thread: disable button
   m_view.SetExportEnabled(false);

   // UI thread: capture bitmap
   wxBitmap bmp = m_view.RenderToBitmap();

   // Copy bitmap (safe for thread use)
   wxBitmap bmpCopy = bmp;

   m_exportTask = std::async(std::launch::async,
      [this, bmpCopy, path]()
      {
         m_exportService.ExportViewAsPNG(bmpCopy, path);

         m_view.CallAfter([this]()
            {
               m_view.SetExportEnabled(true);
            });
      }
   );
}

void SuperSpectrogramController::BindView()
{
   m_view.NotifyConfigChanged = [this](const SuperSpectrogramConfig& cfg)
      {
         ApplyConfigChange(cfg);
      };

   m_view.NotifyExportRequested =
      [this](int format, const std::string& path)
      {
         if (format == 0)
            ExportMatrix(path);
         else
            ExportCurrentView(path);
      };
}
