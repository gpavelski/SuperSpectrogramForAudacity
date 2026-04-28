/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.cpp

  Guilherme Pavelski

*******************************************************************//**

\class SuperSpectrogramPlotDialog
\brief Displays a detailed spectrogram of the waveform.
* Responsibilities:
 *  - Acts as the top-level UI controller for the Super Spectrogram analyzer.
 *  - Owns and manages the SuperSpectrogramPanel used to render the STFT matrix.
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
#include "SuperSpectrogramPanel.h"
#include "STFTProcessor.h"
#include <fstream>
#include <wx/config.h>
#include <wx/display.h>
#include <wx/wx.h> 

#define SuperSpectrogramTitle XO("Super Spectrogram")

static const wxString kConfigPath = "/SuperSpectrogram";

//-----------------------------------------------------------------
// Event table for the dialog
//-----------------------------------------------------------------
BEGIN_EVENT_TABLE(SuperSpectrogramPlotDialog, wxDialogWrapper)
   EVT_CLOSE(SuperSpectrogramPlotDialog::OnCloseWindow)
   EVT_CHOICE(ID_NoiseFloorChoice, SuperSpectrogramPlotDialog::OnNoiseFloorChanged)
   EVT_CHOICE(ID_HighestNoteChoice, SuperSpectrogramPlotDialog::OnHighestNoteChanged)
   EVT_CHOICE(ID_ColormapChoice, SuperSpectrogramPlotDialog::OnColormapChanged)
   EVT_CHOICE(ID_NoteNamingChoice, SuperSpectrogramPlotDialog::OnNoteNamingChanged)
   EVT_CHECKBOX(ID_ShowNoteLinesCheck, SuperSpectrogramPlotDialog::OnShowNoteLinesChanged)
   EVT_CHOICE(ID_TimeTickChoice, SuperSpectrogramPlotDialog::OnTimeTickChanged)
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

   mModel = std::make_unique<SuperSpectrogramModel>();

   SuperSpectrogramModel::Parameters params;
   params.noiseFloor = mNoiseFloor;
   params.detailLevel = mDetailLevel;

   mModel->SetParameters(params);

   // 1) Create toolbar (export + noise floor)
   CreateControls(mainSizer);

   // 2) Create spectrogram panel ONCE
   mSuperSpectrogramPanel = std::make_unique<SuperSpectrogramPanel>(this);

   LoadSettings();

   mSuperSpectrogramPanel->SetShowNoteLines(
      mShowNoteLinesCheck->GetValue());

   mSuperSpectrogramPanel->SetColormap(
      static_cast<SuperSpectrogramPanel::ColormapType>(
         reinterpret_cast<intptr_t>(
            mColormapChoice->GetClientData(
               mColormapChoice->GetSelection()))));

   mSuperSpectrogramPanel->SetNoteNamingStyle(
      static_cast<SuperSpectrogramPanel::NoteNamingStyle>(
         reinterpret_cast<intptr_t>(
            mNoteNamingChoice->GetClientData(
               mNoteNamingChoice->GetSelection()))));

   mSuperSpectrogramPanel->SetTimeTickMode(
      static_cast<SuperSpectrogramPanel::TimeTickMode>(
         reinterpret_cast<intptr_t>(
            mTimeTickChoice->GetClientData(
               mTimeTickChoice->GetSelection()))));

   mainSizer->Add(mSuperSpectrogramPanel.get(), 1, wxEXPAND | wxALL, 5);

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
   constexpr int MAX_VISIBLE_COLUMNS = 800;
   constexpr int PIXELS_PER_COLUMN = 1;

   int columns = mSuperSpectrogramPanel
      ? mSuperSpectrogramPanel->GetColumnCount()
      : 0;

   if (columns == 0)
      return;

   // -----------------------------
   // Width: data-driven 
   // -----------------------------
   int visibleColumns = std::min(columns, MAX_VISIBLE_COLUMNS);
   int minWidth = visibleColumns * PIXELS_PER_COLUMN;

   // -----------------------------------
   // Height: derived from screen size
   // -----------------------------------
   wxDisplay display(GetParent() ? GetParent() : this);
   wxRect clientArea = display.GetClientArea();

   // Use a conservative fraction of usable screen height
   constexpr double HEIGHT_RATIO = 0.75; // 75% of available height

   int minHeight = static_cast<int>(clientArea.GetHeight() * HEIGHT_RATIO);

   // enforce a reasonable lower bound
   constexpr int MIN_HEIGHT_FALLBACK = 400;
   minHeight = std::max(minHeight, MIN_HEIGHT_FALLBACK);

   SetMinSize(wxSize(minWidth, minHeight));
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
   if (!mData || !mModel)
      return;

   SuperSpectrogramModel::Parameters params;
   params.noiseFloor = mNoiseFloor;
   params.detailLevel = mDetailLevel;

   mModel->SetParameters(params);

   mModel->Compute(mData.get(), mDataLen, mRate);

   PlotSTFTMatrix(mModel->GetMatrix());
}

//-----------------------------------------------------------------
// Model: Data -> View Binding
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::PlotSTFTMatrix(
   const std::vector<std::vector<double>>& matrix)
{
   if (!mSuperSpectrogramPanel || !mModel)
      return;

   mMatrix = matrix;
   mMaxFreq = mModel->GetMaxFreq();
   mNumSamples = mModel->GetNumSamples();

   mSuperSpectrogramPanel->SetData(
      mMatrix,
      mMaxFreq,
      mNumSamples
   );

   mSuperSpectrogramPanel->ResetView();
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

   mNoiseFloorChoice = CreateChoice(
      this,
      ID_NoiseFloorChoice,
      kNoiseFloorOptions,
      kDefaultNoiseFloor);

   mNoiseFloor = kDefaultNoiseFloor;

   toolbarSizer->Add(
      mNoiseFloorChoice,
      0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

   // Highest note selector
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Highest Note:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mHighestNoteChoice = CreateChoice(
      this,
      ID_HighestNoteChoice,
      kHighestNoteOptions,
      kDefaultHighestNote);

   mDetailLevel = kDefaultHighestNote;

   toolbarSizer->Add(
      mHighestNoteChoice,
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 15);

   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Note naming:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mNoteNamingChoice = CreateChoice(
      this,
      ID_NoteNamingChoice,
      kNoteNamingOptions,
      kDefaultNoteNaming);

   toolbarSizer->Add(
      mNoteNamingChoice,
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Colormap:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mColormapChoice = CreateChoice(
      this,
      ID_ColormapChoice,
      kColormapOptions,
      kDefaultColormap);

   toolbarSizer->Add(
      mColormapChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT |wxRIGHT,
      10);

   mShowNoteLinesCheck = new wxCheckBox(
      this,
      ID_ShowNoteLinesCheck,
      _("Show notes"));

   mShowNoteLinesCheck->SetValue(true);

   toolbarSizer->Add(
      mShowNoteLinesCheck,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      10);

   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Time scale:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mTimeTickChoice = CreateChoice(
      this,
      ID_TimeTickChoice,
      kTimeTickOptions,
      kDefaultTimeTick);

   toolbarSizer->Add(
      mTimeTickChoice,
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);

   // Export button
   mExportButton = new wxButton(this, wxID_SAVE, _("Export…"));
   toolbarSizer->Add(
      mExportButton,
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   toolbarSizer->AddStretchSpacer();

   parentSizer->Add(
      toolbarSizer,
      0, wxEXPAND | wxALL, 5);
}

wxChoice* SuperSpectrogramPlotDialog::CreateChoice(
   wxWindow* parent,
   wxWindowID id,
   const std::vector<ChoiceOption>& options,
   int defaultValue)
{
   auto* choice = new wxChoice(parent, id);

   int defaultIndex = wxNOT_FOUND;

   for (size_t i = 0; i < options.size(); ++i) {
      choice->Append(
         options[i].label,
         reinterpret_cast<void*>(static_cast<intptr_t>(options[i].value)));

      if (options[i].value == defaultValue)
         defaultIndex = static_cast<int>(i);
   }

   if (defaultIndex != wxNOT_FOUND)
      choice->SetSelection(defaultIndex);

   return choice;
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
   SaveSettings();
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
   SaveSettings();
   Recalc();
   UpdateLayoutPreservingState();
}

void SuperSpectrogramPlotDialog::OnColormapChanged(wxCommandEvent&)
{
   int sel = mColormapChoice->GetSelection();
   if (sel == wxNOT_FOUND || !mSuperSpectrogramPanel)
      return;

   auto type = static_cast<SuperSpectrogramPanel::ColormapType>(
      reinterpret_cast<intptr_t>(
         mColormapChoice->GetClientData(sel)));

   SaveSettings();
   mSuperSpectrogramPanel->SetColormap(type);
}

void SuperSpectrogramPlotDialog::OnNoteNamingChanged(wxCommandEvent&)
{
   if (!mSuperSpectrogramPanel || !mNoteNamingChoice)
      return;

   int sel = mNoteNamingChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   auto style = static_cast<SuperSpectrogramPanel::NoteNamingStyle>(
      reinterpret_cast<intptr_t>(
         mNoteNamingChoice->GetClientData(sel)));

   SaveSettings();
   mSuperSpectrogramPanel->SetNoteNamingStyle(style);
}

void SuperSpectrogramPlotDialog::OnShowNoteLinesChanged(wxCommandEvent& event)
{
   if (!mSuperSpectrogramPanel)
      return;

   SaveSettings();
   mSuperSpectrogramPanel->SetShowNoteLines(event.IsChecked());
}

void SuperSpectrogramPlotDialog::OnTimeTickChanged(wxCommandEvent&)
{
   if (!mSuperSpectrogramPanel || !mTimeTickChoice)
      return;

   int sel = mTimeTickChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   auto mode = static_cast<SuperSpectrogramPanel::TimeTickMode>(
      reinterpret_cast<intptr_t>(
         mTimeTickChoice->GetClientData(sel)));

   mSuperSpectrogramPanel->SetTimeTickMode(mode);
}

//-----------------------------------------------------------------
// Export: Data and rendering output
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::OnExport(wxCommandEvent&)
{
   if (!mSuperSpectrogramPanel || mMatrix.empty())
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

   wxBitmap bmp = mSuperSpectrogramPanel->RenderCurrentViewToBitmap();
   if (!bmp.IsOk())
      return;

   bmp.SaveFile(dlg.GetPath(), wxBITMAP_TYPE_PNG);
}

void SuperSpectrogramPlotDialog::LoadSettings()
{
   wxConfigBase* cfg = wxConfigBase::Get(false);
   if (!cfg)
      return;

   cfg->SetPath(kConfigPath);

   long value;

   // -------------------------
   // Noise floor
   // -------------------------
   if (cfg->Read("NoiseFloor", &value))
      mNoiseFloor = static_cast<int>(value);

   // -------------------------
   // Highest note
   // -------------------------
   if (cfg->Read("HighestNote", &value))
      mDetailLevel = static_cast<int>(value);

   // -------------------------
   // Colormap
   // -------------------------
   if (cfg->Read("Colormap", &value))
      SetChoiceByValue(mColormapChoice, static_cast<int>(value));

   // -------------------------
   // Note naming
   // -------------------------
   if (cfg->Read("NoteNaming", &value))
      SetChoiceByValue(mNoteNamingChoice, static_cast<int>(value));

   // -------------------------
   // Show note lines
   // -------------------------
   bool showNotes = true;
   cfg->Read("ShowNoteLines", &showNotes);
   mShowNoteLinesCheck->SetValue(showNotes);

   // -------------------------
   // Time Tick Mode
   // -------------------------
   if (cfg->Read("TimeTickMode", &value))
      SetChoiceByValue(mTimeTickChoice, static_cast<int>(value));

   // -------------------------
   // Apply to UI-dependent state
   // -------------------------
   SetChoiceByValue(mNoiseFloorChoice, mNoiseFloor);
   SetChoiceByValue(mHighestNoteChoice, mDetailLevel);
}

void SuperSpectrogramPlotDialog::SetChoiceByValue(wxChoice* choice, int value)
{
   if (!choice)
      return;

   for (unsigned i = 0; i < choice->GetCount(); ++i) {
      int v = static_cast<int>(
         reinterpret_cast<intptr_t>(choice->GetClientData(i)));

      if (v == value) {
         choice->SetSelection(i);
         return;
      }
   }
}

void SuperSpectrogramPlotDialog::SaveSettings()
{
   wxConfigBase* cfg = wxConfigBase::Get(false);
   if (!cfg)
      return;

   cfg->SetPath(kConfigPath);

   cfg->Write("NoiseFloor", (long)mNoiseFloor);
   cfg->Write("HighestNote", (long)mDetailLevel);

   // Colormap
   if (mColormapChoice) {
      int sel = mColormapChoice->GetSelection();
      if (sel != wxNOT_FOUND) {
         int value = static_cast<int>(
            reinterpret_cast<intptr_t>(
               mColormapChoice->GetClientData(sel)));
         cfg->Write("Colormap", (long)value);
      }
   }

   // Note naming
   if (mNoteNamingChoice) {
      int sel = mNoteNamingChoice->GetSelection();
      if (sel != wxNOT_FOUND) {
         int value = static_cast<int>(
            reinterpret_cast<intptr_t>(
               mNoteNamingChoice->GetClientData(sel)));
         cfg->Write("NoteNaming", (long)value);
      }
   }

   // Show note lines
   if (mShowNoteLinesCheck)
      cfg->Write("ShowNoteLines", mShowNoteLinesCheck->GetValue());

   //Time ticks selection
   if (mTimeTickChoice) {
      int sel = mTimeTickChoice->GetSelection();
      if (sel != wxNOT_FOUND) {
         int value = static_cast<int>(
            reinterpret_cast<intptr_t>(
               mTimeTickChoice->GetClientData(sel)));
         cfg->Write("TimeTickMode", (long)value);
      }
   }
   cfg->Flush();
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
   SaveSettings();
   Destroy();
}
