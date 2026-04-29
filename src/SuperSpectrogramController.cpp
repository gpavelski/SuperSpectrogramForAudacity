/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramController.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramController.h"
#include <fstream>

SuperSpectrogramController::SuperSpectrogramController(
   SuperSpectrogramAudioExtractor& extractor,
   SuperSpectrogramConfig& config,
   SuperSpectrogramModel& model,
   SuperSpectrogramView& view)
   : mExtractor(extractor)
   , mConfig(config)
   , mModel(model)
   , mView(view)
{
}

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------
bool SuperSpectrogramController::Initialize()
{
   mConfig.Load();

   UpdateModelParameters();
   mView.ApplyConfig(mConfig);

   LoadAudioFromProject();

   if (!mCurrentData || mCurrentLen == 0)
      return false;

   Recompute(mCurrentData, mCurrentLen, mCurrentRate);

   return true;
}

void SuperSpectrogramController::LoadAudioFromProject()
{
   auto audio = mExtractor.Extract();

   if (!audio)
      return;

   // Take ownership of the buffer
   mOwnedData = std::move(audio->data);

   mCurrentData = mOwnedData.get();
   mCurrentLen = audio->length;
   mCurrentRate = audio->rate;
}

//------------------------------------------------------------
// Internal helpers
//------------------------------------------------------------
void SuperSpectrogramController::UpdateModelParameters()
{
   SuperSpectrogramModel::Parameters p;
   p.noiseFloor = mConfig.noiseFloor;
   p.detailLevel = mConfig.detailLevel;

   mModel.SetParameters(p);
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

   mView.SetSpectrogramData(matrix, maxFreq, numSamples);
}

void SuperSpectrogramController::UpdateView()
{
   mView.ApplyConfig(mConfig);
}

//------------------------------------------------------------
// Event Handlers
//------------------------------------------------------------
void SuperSpectrogramController::OnNoiseFloorChanged(int value)
{
   if (mConfig.noiseFloor == value)
      return;

   mConfig.noiseFloor = value;
   mConfig.Save();

   Recompute(mCurrentData, mCurrentLen, mCurrentRate);
}

void SuperSpectrogramController::OnDetailLevelChanged(int value)
{
   if (mConfig.detailLevel == value)
      return;

   mConfig.detailLevel = value;
   mConfig.Save();

   Recompute(mCurrentData, mCurrentLen, mCurrentRate);
}

void SuperSpectrogramController::OnColormapChanged(SuperSpectrogramConfig::Colormap value)
{
   if (mConfig.colormap == value)
      return;

   mConfig.colormap = value;
   mConfig.Save();

   UpdateView();
}

void SuperSpectrogramController::OnNoteNamingChanged(SuperSpectrogramConfig::NoteNaming value)
{
   if (mConfig.noteNaming == value)
      return;

   mConfig.noteNaming = value;
   mConfig.Save();

   UpdateView();
}

void SuperSpectrogramController::OnShowNoteLinesChanged(bool value)
{
   if (mConfig.showNoteLines == value)
      return;

   mConfig.showNoteLines = value;
   mConfig.Save();

   UpdateView();
}

void SuperSpectrogramController::OnTimeTickModeChanged(SuperSpectrogramConfig::TimeTickMode value)
{
   if (mConfig.timeTickMode == value)
      return;

   mConfig.timeTickMode = value;
   mConfig.Save();

   UpdateView();
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

   wxBitmap bmp = mView.RenderToBitmap();
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

   mView.NotifyColormapChanged = [this](SuperSpectrogramConfig::Colormap value) {
      OnColormapChanged(value);
   };

   mView.NotifyNoteNamingChanged = [this](SuperSpectrogramConfig::NoteNaming value) {
      OnNoteNamingChanged(value);
   };

   mView.NotifyShowNoteLinesChanged = [this](bool value) {
      OnShowNoteLinesChanged(value);
   };

   mView.NotifyTimeTickChanged = [this](SuperSpectrogramConfig::TimeTickMode value) {
      OnTimeTickModeChanged(value);
   };

   mView.NotifyExportRequested = [this](wxWindow* parent)
   {
      OnExportRequested(parent);
   };
}
