#ifndef __SUPER_SPECTROGRAM_CONTROLLER__
#define __SUPER_SPECTROGRAM_CONTROLLER__

#include <memory>

class SuperSpectrogramModel;
class SuperSpectrogramSettings;
class SuperSpectrogramPanel;

class SuperSpectrogramController
{
public:
   SuperSpectrogramController(
      SuperSpectrogramModel& model,
      SuperSpectrogramSettings& settings,
      SuperSpectrogramPanel& panel);

   void Initialize();

   // Event handlers (called by dialog)
   void OnNoiseFloorChanged(int value);
   void OnDetailLevelChanged(int value);
   void OnColormapChanged(int value);
   void OnNoteNamingChanged(int value);
   void OnShowNoteLinesChanged(bool value);
   void OnTimeTickModeChanged(int value);

   // External trigger (dialog Show)
   void Recompute(const float* data, size_t len, double rate);

private:
   void ApplySettingsToView();
   void UpdateModelParameters();

   SuperSpectrogramModel& mModel;
   SuperSpectrogramSettings& mSettings;
   SuperSpectrogramPanel& mPanel;
};

#endif