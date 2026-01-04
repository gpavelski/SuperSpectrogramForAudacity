/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.cpp

  Guilherme Pavelski

*******************************************************************//**

\class SuperSpectrogramPlotDialog
\brief Displays a detailed spectrogram of the waveform.
* Responsibilities:
 *  - Acts as the top-level UI controller for the Super Spectrogram analyzer.
 *  - Owns and manages the SpectrogramPanel used to render the STFT matrix.
 *  - Coordinates audio extraction, STFT computation, and visualization updates.
 *  - Translates user interactions (noise floor, highest note, export actions)
 *    into recalculation or rendering changes.
 *  - Enforces data-driven layout constraints based on spectrogram dimensions.
 *  - Provides export facilities for both raw spectrogram data and rendered views.
 *
*//****************************************************************//**

\class SuperSpectrogramPlot
\brief Works with SuperSpectrogramPlotDialog to display a more detailed
spectrum plot of the waveform.
This class actually does the graph display.

*//*******************************************************************/

#include "SuperSpectrogramWindow.h"
#include "SpectrogramPanel.h"
#include "STFTProcessor.h"
#include <fstream>
#include <wx/wx.h> 

#define SuperSpectrogramTitle XO("Super Spectrogram")

//-----------------------------------------------------------------
// Event table for the dialog
//-----------------------------------------------------------------
BEGIN_EVENT_TABLE(SuperSpectrogramPlotDialog, wxDialogWrapper)
   EVT_CLOSE(SuperSpectrogramPlotDialog::OnCloseWindow)
   EVT_CHOICE(ID_NoiseFloorChoice, SuperSpectrogramPlotDialog::OnNoiseFloorChanged)
   EVT_CHOICE(ID_HighestNoteChoice, SuperSpectrogramPlotDialog::OnHighestNoteChanged)
   EVT_BUTTON(wxID_SAVE, SuperSpectrogramPlotDialog::OnExport)
END_EVENT_TABLE()

//-----------------------------------------------------------------
// View: Dialog construction & teardown
//-----------------------------------------------------------------
SuperSpectrogramPlotDialog::SuperSpectrogramPlotDialog(
   wxWindow* parent,
   wxWindowID id,
   AudacityProject& project,
   const TranslatableString& title,
   const wxPoint& pos)
   : PlotSuperSpectrogramBase{ project }
   , wxDialogWrapper(
      parent,
      id,
      title,
      pos,
      wxSize(1000, 600),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
   SetName();

   auto* mainSizer = new wxBoxSizer(wxVERTICAL);

   // 1) Create toolbar (export + noise floor)
   CreateControls(mainSizer);

   // 2) Create spectrogram panel ONCE
   mSpectrogramPanel = std::make_unique<SpectrogramPanel>(this);
   mSpectrogramPanel->EnableNoteLines(true);
   mSpectrogramPanel->EnableTimeTicks(true);

   mainSizer->Add(mSpectrogramPanel.get(), 1, wxEXPAND | wxALL, 5);

   SetSizer(mainSizer);
}

SuperSpectrogramPlotDialog::~SuperSpectrogramPlotDialog() = default;

//-----------------------------------------------------------------
// View: Visibility and layout
//-----------------------------------------------------------------
bool SuperSpectrogramPlotDialog::Show(bool show)
{
   if (show && !IsShown()) {
      Recalc();
      ApplyDataDrivenMinSize();
      Layout();
      Fit();
      Centre();
   }

   return wxDialogWrapper::Show(show);
}

void SuperSpectrogramPlotDialog::ApplyDataDrivenMinSize()
{
   constexpr int MAX_VISIBLE_COLUMNS = 800;  // Increased for better initial view
   constexpr int PIXELS_PER_COLUMN = 1;
   constexpr int INITIAL_HEIGHT = 800;

   int columns = mSpectrogramPanel
      ? mSpectrogramPanel->GetColumnCount()
      : 0;

   if (columns == 0)
      return;

   int visibleColumns = std::min(columns, MAX_VISIBLE_COLUMNS);
   int minWidth = visibleColumns * PIXELS_PER_COLUMN;
   int initialHeight = INITIAL_HEIGHT;
   SetMinSize(wxSize(minWidth, initialHeight));
}

void SuperSpectrogramPlotDialog::UpdateLayoutPreservingState()
{
   const bool wasMaximized = IsMaximized();

   if (!wasMaximized) {
      ApplyDataDrivenMinSize();
      Layout();
      Fit();
      Centre();
   }
   else {
      Layout();
      // Explicitly re-maximize to guard against platform quirks
      Maximize(true);
   }
}

//-----------------------------------------------------------------
// Model: Audio validity & spectrogram data
//-----------------------------------------------------------------
bool SuperSpectrogramPlotDialog::IsAudioSelectionValid()
{
   return GetAudio();
}

//-----------------------------------------------------------------
// Model: Spectrogram computation
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::Recalc()
{
   if (!mData)
      return;

   mAnalyst->Calculate(mData.get(),
      mDataLen,
      mDetailLevel,
      mRate,
      mNoiseFloor);

   PlotSTFTMatrix(mAnalyst->GetMatrix());
}

//-----------------------------------------------------------------
// Model: Data -> View Binding
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::PlotSTFTMatrix(
   const std::vector<std::vector<double>>& matrix)
{
   if (!mSpectrogramPanel)
      return;

   mMatrix = matrix;
   mMaxFreq = mAnalyst->GetTargetRate() / 2.0;
   mNumSamples = mAnalyst->GetSignalLength();

   mSpectrogramPanel->SetData(
      mMatrix,
      mMaxFreq,
      mNumSamples
   );

   mSpectrogramPanel->ResetView();
}

//-----------------------------------------------------------------
// Controller: UI controls & bindings
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::CreateControls(wxSizer* parentSizer)
{
   auto* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

   // Noise floor selector
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Noise floor:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mNoiseFloorChoice = new wxChoice(this, ID_NoiseFloorChoice);
   mNoiseFloorChoice->Append("-120 dB", reinterpret_cast<void*>(-120));
   mNoiseFloorChoice->Append("-100 dB", reinterpret_cast<void*>(-100));
   mNoiseFloorChoice->Append("-85 dB", reinterpret_cast<void*>(-85));
   mNoiseFloorChoice->Append("-70 dB", reinterpret_cast<void*>(-70));
   mNoiseFloorChoice->Append("-55 dB", reinterpret_cast<void*>(-55));

   mNoiseFloorChoice->SetSelection(3); // -70 dB default
   mNoiseFloor = -70;

   toolbarSizer->Add(mNoiseFloorChoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

   // Highest note selector
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Highest Note:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mHighestNoteChoice = new wxChoice(this, ID_HighestNoteChoice);
   mHighestNoteChoice->Append("C4", reinterpret_cast<void*>(4));
   mHighestNoteChoice->Append("C5", reinterpret_cast<void*>(5));
   mHighestNoteChoice->Append("C6", reinterpret_cast<void*>(6));
   mHighestNoteChoice->Append("C7", reinterpret_cast<void*>(7));
   mHighestNoteChoice->Append("C8", reinterpret_cast<void*>(8));

   mHighestNoteChoice->SetSelection(3); // C7 default
   mDetailLevel = 7;

   toolbarSizer->Add(mHighestNoteChoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 15);

   // Export button
   mExportButton = new wxButton(this, wxID_SAVE, _("Export…"));
   toolbarSizer->Add(mExportButton, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

   toolbarSizer->AddStretchSpacer();

   parentSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
}

//-----------------------------------------------------------------
// Controller: Parameter Change Handlers
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::OnNoiseFloorChanged(wxCommandEvent&)
{
   int sel = mNoiseFloorChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   int value = static_cast<int>(
      reinterpret_cast<intptr_t>(
         mNoiseFloorChoice->GetClientData(sel)));

   if (mNoiseFloor == value)
      return;

   mNoiseFloor = value;
   Recalc();
}

void SuperSpectrogramPlotDialog::OnHighestNoteChanged(wxCommandEvent&)
{
   int sel = mHighestNoteChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   int value = static_cast<int>(
      reinterpret_cast<intptr_t>(
         mHighestNoteChoice->GetClientData(sel)));

   if (mDetailLevel == value)
      return;

   mDetailLevel = value;
   Recalc();
   UpdateLayoutPreservingState();
}

//-----------------------------------------------------------------
// Export: Data and rendering output
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::OnExport(wxCommandEvent&)
{
   if (!mSpectrogramPanel || mMatrix.empty())
      return;

   wxArrayString choices;
   choices.Add(_("Export matrix as text (.txt)"));
   choices.Add(_("Export current view as image (.png)"));

   wxSingleChoiceDialog dlg(
      this,
      _("Choose export format"),
      _("Export Spectrogram"),
      choices
   );

   if (dlg.ShowModal() != wxID_OK)
      return;

   if (dlg.GetSelection() == 0)
      ExportMatrixAsText();
   else
      ExportViewAsPNG();
}

void SuperSpectrogramPlotDialog::ExportMatrixAsText()
{
   wxFileDialog dlg(
      this,
      _("Save spectrogram matrix"),
      "",
      "spectrogram.txt",
      "Text files (*.txt)|*.txt",
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT
   );

   if (dlg.ShowModal() != wxID_OK)
      return;

   std::ofstream out(dlg.GetPath().ToStdString());
   if (!out.is_open())
      return;

   for (const auto& column : mMatrix) {
      for (size_t i = 0; i < column.size(); ++i) {
         out << column[i];
         if (i + 1 < column.size())
            out << '\t';
      }
      out << '\n';
   }
}

void SuperSpectrogramPlotDialog::ExportViewAsPNG()
{
   wxFileDialog dlg(
      this,
      _("Save spectrogram image"),
      "",
      "spectrogram.png",
      "PNG files (*.png)|*.png",
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT
   );

   if (dlg.ShowModal() != wxID_OK)
      return;

   wxBitmap bmp = mSpectrogramPanel->RenderCurrentViewToBitmap();
   if (!bmp.IsOk())
      return;

   bmp.SaveFile(dlg.GetPath(), wxBITMAP_TYPE_PNG);
}

//-----------------------------------------------------------------
// Application integration & command registration
//-----------------------------------------------------------------
#include "CommandContext.h"
#include "CommandManager.h"
#include "ProjectWindows.h"
#include "CommonCommandFlags.h"

namespace {
   SuperSpectrogramPlotDialog* gSpectrogramDialog = nullptr;
   void ClearSpectrogramDialog()
   {
      gSpectrogramDialog = nullptr;
   }

   void OnPlotSuperSpectrogram(const CommandContext& context)
   {
      auto& project = context.project;
      CommandManager::Get(project).RegisterLastAnalyzer(context);

      if (gSpectrogramDialog) {
         gSpectrogramDialog->Raise();
         gSpectrogramDialog->SetFocus();
         return;
      }

      gSpectrogramDialog = new SuperSpectrogramPlotDialog(
         &GetProjectFrame(project),
         wxID_ANY,
         project,
         SuperSpectrogramTitle,
         wxPoint{ 150, 150 }
      );

      if (!gSpectrogramDialog->IsAudioSelectionValid())
      {
         gSpectrogramDialog->Destroy();
         gSpectrogramDialog = nullptr;
         return;
      }

      gSpectrogramDialog->Show(true);
   }


   // Register that menu item
   using namespace MenuRegistry;
   AttachedItem sAttachment{
       Command(wxT("PlotSuperSpectrogram"), XXO("Plot Super Spectrogram..."),
           OnPlotSuperSpectrogram,
           AudioIONotBusyFlag() | WaveTracksSelectedFlag() | TimeSelectedFlag()),
       wxT("Analyze/Analyzers/Windows")
   };
}

//-----------------------------------------------------------------
// View: Close handling
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
   gSpectrogramDialog = nullptr;
   Destroy();
}
