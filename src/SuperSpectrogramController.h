#ifndef __SUPER_SPECTROGRAM_CONTROLLER__
#define __SUPER_SPECTROGRAM_CONTROLLER__

#include <memory>
#include "SuperSpectrogramModel.h"
#include "SuperSpectrogramSettings.h"
#include "SuperSpectrogramPanel.h"
#include "SuperSpectrogramWindow.h"

class SuperSpectrogramModel;
class SuperSpectrogramSettings;
class SuperSpectrogramPanel;
class SuperSpectrogramPlotDialog; // forward

class SuperSpectrogramController
{
public:
   SuperSpectrogramController(
      SuperSpectrogramModel& model,
      SuperSpectrogramSettings& settings,
      SuperSpectrogramPanel& panel,
      SuperSpectrogramPlotDialog& view);

   void Initialize();
   void BindView();

   // Event handlers (called by dialog)
   void OnNoiseFloorChanged(int value);
   void OnDetailLevelChanged(int value);
   void OnColormapChanged(int value);
   void OnNoteNamingChanged(int value);
   void OnShowNoteLinesChanged(bool value);
   void OnTimeTickModeChanged(int value);
   void OnExportRequested(wxWindow* parent);

   // External trigger (dialog Show)
   void Recompute(const float* data, size_t len, double rate);

   void SetAudioData(const float* data, size_t len, double rate);

private:
   void ApplySettingsToView();
   void UpdateModelParameters();
   void PushSettingsToView();
   void ExportMatrixAsText(wxWindow* parent);
   void ExportViewAsPNG(wxWindow* parent);

   SuperSpectrogramModel& mModel;
   SuperSpectrogramSettings& mSettings;
   SuperSpectrogramPanel& mPanel;
   SuperSpectrogramPlotDialog& mView;

   const float* mCurrentData = nullptr;
   size_t mCurrentLen = 0;
   double mCurrentRate = 0.0;
};

#endif
