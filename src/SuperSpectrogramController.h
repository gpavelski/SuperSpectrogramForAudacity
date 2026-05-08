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
#include "SuperSpectrogramView.h"

class SuperSpectrogramConfig;
class SuperSpectrogramModel;
class SuperSpectrogramView;

class SuperSpectrogramController
{
public:
   SuperSpectrogramController(
      SuperSpectrogramAudioExtractor& extractor,
      SuperSpectrogramConfig& config,
      SuperSpectrogramExportService& exportService,
      SuperSpectrogramModel& model,
      SuperSpectrogramView& view);

   bool Initialize();
   void LoadAudioFromProject();
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
   void Recompute(const float* data, size_t len, double rate);

   void SetAudioData(const float* data, size_t len, double rate);

private:
   void UpdateView();
   void UpdateModelParameters();
   void UpdateLayoutPreservingState();
   void ExportMatrix(const std::string& path);
   void ExportCurrentView(const std::string& path);

   SuperSpectrogramAudioExtractor& mExtractor;
   SuperSpectrogramConfig& mConfig;
   SuperSpectrogramExportService& mExportService;
   SuperSpectrogramConfig mLastAppliedConfig;
   SuperSpectrogramModel& mModel;
   SuperSpectrogramView& mView;

   ArrayOf<float> mOwnedData;
   const float* mCurrentData = nullptr;
   size_t mCurrentLen = 0;
   double mCurrentRate = 0.0;
};

#endif
