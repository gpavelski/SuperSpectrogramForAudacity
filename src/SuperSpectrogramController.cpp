#include "SuperSpectrogramController.h"
#include <fstream>

SuperSpectrogramController::SuperSpectrogramController(
   SuperSpectrogramModel& model,
   SuperSpectrogramSettings& settings,
   SuperSpectrogramPanel& panel,
   SuperSpectrogramPlotDialog& view)
   : mModel(model)
   , mSettings(settings)
   , mPanel(panel)
   , mView(view)
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

   Recompute(mCurrentData, mCurrentLen, mCurrentRate);
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

void SuperSpectrogramController::PushSettingsToView()
{
    mView.ApplySettings(
      mSettings.noiseFloor,
      mSettings.detailLevel,
      mSettings.colormap,
      mSettings.noteNaming,
      mSettings.showNoteLines,
      mSettings.timeTickMode
    );
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

   mView.PlotSTFTMatrix(matrix, maxFreq, numSamples);
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

void SuperSpectrogramController::OnExportRequested(wxWindow* parent)
{
   if (!parent)
      return;

   wxArrayString choices;
   choices.Add("Export matrix as text (.txt)");
   choices.Add("Export current view as image (.png)");

   wxSingleChoiceDialog dlg(
      parent,
      "Choose export format",
      "Export Spectrogram",
      choices);

   if (dlg.ShowModal() != wxID_OK)
      return;

   if (dlg.GetSelection() == 0)
      ExportMatrixAsText(parent);
   else
      ExportViewAsPNG(parent);
}

void SuperSpectrogramController::ExportMatrixAsText(wxWindow* parent)
{
   wxFileDialog dlg(
      parent,
      "Save spectrogram matrix",
      "",
      "spectrogram.txt",
      "Text files (*.txt)|*.txt",
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

   if (dlg.ShowModal() != wxID_OK)
      return;

   const auto& matrix = mModel.GetMatrix();

   std::ofstream out(dlg.GetPath().ToStdString());
   if (!out.is_open())
      return;

   for (const auto& column : matrix)
   {
      for (size_t i = 0; i < column.size(); ++i)
      {
         out << column[i];
         if (i + 1 < column.size())
            out << '\t';
      }
      out << '\n';
   }
}

void SuperSpectrogramController::ExportViewAsPNG(wxWindow* parent)
{
   wxFileDialog dlg(
      parent,
      "Save spectrogram image",
      "",
      "spectrogram.png",
      "PNG files (*.png)|*.png",
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT
   );

   if (dlg.ShowModal() != wxID_OK)
      return;

   wxBitmap bmp = mPanel.RenderCurrentViewToBitmap();
   if (!bmp.IsOk())
      return;

   bmp.SaveFile(dlg.GetPath(), wxBITMAP_TYPE_PNG);
}

void SuperSpectrogramController::SetAudioData(const float* data, size_t len, double rate)
{
   mCurrentData = data;
   mCurrentLen = len;
   mCurrentRate = rate;
}

void SuperSpectrogramController::BindView()
{
   mView.NotifyNoiseFloorChanged = [this](int value) {
      OnNoiseFloorChanged(value);
   };

   mView.NotifyHighestNoteChanged = [this](int value) {
      OnDetailLevelChanged(value);
   };

   mView.NotifyColormapChanged = [this](int value) {
      OnColormapChanged(value);
   };

   mView.NotifyNoteNamingChanged = [this](int value) {
      OnNoteNamingChanged(value);
   };

   mView.NotifyShowNoteLinesChanged = [this](bool value) {
      OnShowNoteLinesChanged(value);
   };

   mView.NotifyTimeTickChanged = [this](int value) {
      OnTimeTickModeChanged(value);
   };

   mView.OnRecomputeRequested = [this]() {
      Recompute(mCurrentData, mCurrentLen, mCurrentRate);
   };

   mView.NotifyExportRequested = [this](wxWindow* parent)
   {
         OnExportRequested(parent);
   };

   if (mCurrentData && mCurrentLen > 0)
   {
      Recompute(mCurrentData, mCurrentLen, mCurrentRate);
   }
}
