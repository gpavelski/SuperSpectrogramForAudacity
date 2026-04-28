#include "SuperSpectrogramController.h"
#include "SuperSpectrogramModel.h"
#include "SuperSpectrogramSettings.h"
#include "SuperSpectrogramPanel.h"

SuperSpectrogramController::SuperSpectrogramController(
   SuperSpectrogramModel& model,
   SuperSpectrogramSettings& settings,
   SuperSpectrogramPanel& panel)
   : mModel(model)
   , mSettings(settings)
   , mPanel(panel)
{
}

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------
void SuperSpectrogramController::Initialize()
{
   mSettings.Load();

   UpdateModelParameters();
   ApplySettingsToView();
}

//------------------------------------------------------------
// Internal helpers
//------------------------------------------------------------
void SuperSpectrogramController::UpdateModelParameters()
{
   SuperSpectrogramModel::Parameters p;
   p.noiseFloor = mSettings.noiseFloor;
   p.detailLevel = mSettings.detailLevel;

   mModel.SetParameters(p);
}

void SuperSpectrogramController::ApplySettingsToView()
{
   mPanel.SetColormap(
      static_cast<SuperSpectrogramPanel::ColormapType>(
         mSettings.colormap));

   mPanel.SetNoteNamingStyle(
      static_cast<SuperSpectrogramPanel::NoteNamingStyle>(
         mSettings.noteNaming));

   mPanel.SetShowNoteLines(
      mSettings.showNoteLines);

   mPanel.SetTimeTickMode(
      static_cast<SuperSpectrogramPanel::TimeTickMode>(
         mSettings.timeTickMode));
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

   mPanel.SetData(
      mModel.GetMatrix(),
      mModel.GetMaxFreq(),
      mModel.GetNumSamples());

   mPanel.ResetView();
}

//------------------------------------------------------------
// Event Handlers
//------------------------------------------------------------
void SuperSpectrogramController::OnNoiseFloorChanged(int value)
{
   if (mSettings.noiseFloor == value)
      return;

   mSettings.noiseFloor = value;
   mSettings.Save();
}

void SuperSpectrogramController::OnDetailLevelChanged(int value)
{
   if (mSettings.detailLevel == value)
      return;

   mSettings.detailLevel = value;
   mSettings.Save();
}

void SuperSpectrogramController::OnColormapChanged(int value)
{
   mSettings.colormap = value;
   mSettings.Save();

   mPanel.SetColormap(
      static_cast<SuperSpectrogramPanel::ColormapType>(value));
}

void SuperSpectrogramController::OnNoteNamingChanged(int value)
{
   mSettings.noteNaming = value;
   mSettings.Save();

   mPanel.SetNoteNamingStyle(
      static_cast<SuperSpectrogramPanel::NoteNamingStyle>(value));
}

void SuperSpectrogramController::OnShowNoteLinesChanged(bool value)
{
   mSettings.showNoteLines = value;
   mSettings.Save();

   mPanel.SetShowNoteLines(value);
}

void SuperSpectrogramController::OnTimeTickModeChanged(int value)
{
   mSettings.timeTickMode = value;
   mSettings.Save();

   mPanel.SetTimeTickMode(
      static_cast<SuperSpectrogramPanel::TimeTickMode>(value));
}