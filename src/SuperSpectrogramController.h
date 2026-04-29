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
      SuperSpectrogramModel& model,
      SuperSpectrogramView& view);

   bool Initialize();
   void LoadAudioFromProject();
   void BindView();

   // Event handlers (called by dialog)
   void OnNoiseFloorChanged(int value);
   void OnDetailLevelChanged(int value);
   void OnColormapChanged(SuperSpectrogramConfig::Colormap value);
   void OnNoteNamingChanged(SuperSpectrogramConfig::NoteNaming value);
   void OnShowNoteLinesChanged(bool value);
   void OnTimeTickModeChanged(SuperSpectrogramConfig::TimeTickMode value);
   void OnExportRequested(wxWindow* parent);

   // External trigger (dialog Show)
   void Recompute(const float* data, size_t len, double rate);

   void SetAudioData(const float* data, size_t len, double rate);

private:
   void UpdateView();
   void UpdateModelParameters();
   void ExportMatrixAsText(wxWindow* parent);
   void ExportViewAsPNG(wxWindow* parent);

   SuperSpectrogramAudioExtractor& mExtractor;
   SuperSpectrogramConfig& mConfig;
   SuperSpectrogramModel& mModel;
   SuperSpectrogramView& mView;

   ArrayOf<float> mOwnedData;
   const float* mCurrentData = nullptr;
   size_t mCurrentLen = 0;
   double mCurrentRate = 0.0;
};

#endif
