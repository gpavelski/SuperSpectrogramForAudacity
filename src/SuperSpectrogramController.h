/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_CONTROLLER__
#define __SUPER_SPECTROGRAM_CONTROLLER__

#include <memory>
#include "SuperSpectrogramAudioExtractor.h"
#include "SuperSpectrogramModel.h"
#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramExportService.h"
#include "SuperSpectrogramSession.h"
#include "SuperSpectrogramView.h"

class SuperSpectrogramConfig;
class SuperSpectrogramModel;
class SuperSpectrogramView;

class SuperSpectrogramController
{
public:
   SuperSpectrogramController(
      SuperSpectrogramAudioExtractor& extractor,
      SuperSpectrogramExportService& exportService,
      SuperSpectrogramModel& model,
      SuperSpectrogramSession& session,
      SuperSpectrogramView& view);

   bool Initialize();
   void BindView();

   struct ConfigDiff
   {
      bool needsRecompute = false;
      bool needsViewUpdate = false;
   };

   void ApplyConfigChange(const SuperSpectrogramConfig& newConfig);
   SuperSpectrogramController::ConfigDiff ComputeDiff(
      const SuperSpectrogramConfig& oldCfg,
      const SuperSpectrogramConfig& newCfg);

   // External trigger (dialog Show)
   void Recompute();


private:
   void UpdateLayoutPreservingState();
   void ExportMatrix(const std::string& path);
   void ExportCurrentView(const std::string& path);
   static bool RequiresRecompute(
      const SuperSpectrogramConfig& oldCfg,
      const SuperSpectrogramConfig& newCfg
   );

   SuperSpectrogramAudioExtractor& mExtractor;
   SuperSpectrogramExportService& mExportService;
   SuperSpectrogramConfig mLastAppliedConfig;
   SuperSpectrogramModel& mModel;
   SuperSpectrogramSession& mSession;
   SuperSpectrogramView& mView;
};

#endif
