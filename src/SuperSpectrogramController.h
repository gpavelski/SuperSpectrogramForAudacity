/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_CONTROLLER__
#define __SUPER_SPECTROGRAM_CONTROLLER__

#include <memory>
#include <future>
#include <string>

#include "SuperSpectrogramAudioExtractor.h"
#include "SuperSpectrogramExportService.h"
#include "SuperSpectrogramModel.h"
#include "SuperSpectrogramSession.h"
#include "SuperSpectrogramView.h"
#include "SuperSpectrogramConfig.h"

class SuperSpectrogramController
{
public:
   SuperSpectrogramController(
      SuperSpectrogramAudioExtractor& extractor,
      SuperSpectrogramExportService& exportService,
      SuperSpectrogramModel& model,
      SuperSpectrogramSession& session,
      SuperSpectrogramView& view
   );

   // Lifecycle
   bool Initialize();
   void BindView();

   // Core orchestration entry point
   void Recompute();

   // Configuration update entry point
   void ApplyConfigChange(const SuperSpectrogramConfig& newConfig);

   struct ConfigDiff
   {
      bool needsRecompute{ false };
      bool needsViewUpdate{ false };
   };

   ConfigDiff ComputeDiff(
      const SuperSpectrogramConfig& oldCfg,
      const SuperSpectrogramConfig& newCfg
   ) const;

private:
   // Core workflows
   void RunAudioExtraction();
   void UpdateModelAndRender();

   // Export workflows
   void ExportMatrix(const std::string& path);
   void ExportCurrentView(const std::string& path);

   // View orchestration helpers
   void UpdateViewLayout();

private:
   SuperSpectrogramAudioExtractor& m_extractor;
   SuperSpectrogramExportService& m_exportService;

   SuperSpectrogramModel& m_model;
   SuperSpectrogramSession& m_session;
   SuperSpectrogramView& m_view;

   SuperSpectrogramConfig m_lastAppliedConfig;

   std::future<void> m_exportTask;
};

#endif
