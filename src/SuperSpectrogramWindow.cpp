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

   // 1) Create toolbar (export + noise floor)
   CreateControls(mainSizer);

   // 2) Create spectrogram panel ONCE
   mSuperSpectrogramPanel =
      std::make_unique<SuperSpectrogramPanel>(this);

   // Apply settings to UI controls
   if (NotifyApplySettingsToView)
      NotifyApplySettingsToView();

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
      if (OnRecomputeRequested)
         OnRecomputeRequested();
      ApplyDataDrivenMinSize();
      Layout();
      Fit();
      Centre();
   }

   return wxDialogWrapper::Show(show);
}

void SuperSpectrogramPlotDialog::ApplySettings(
   int noiseFloor,
   int detailLevel,
   int colormap,
   int noteNaming,
   bool showNoteLines,
   int timeTickMode)
{
   SetChoiceByValue(mNoiseFloorChoice, noiseFloor);
   SetChoiceByValue(mHighestNoteChoice, detailLevel);
   SetChoiceByValue(mColormapChoice, colormap);
   SetChoiceByValue(mNoteNamingChoice, noteNaming);
   SetChoiceByValue(mTimeTickChoice, timeTickMode);
   mShowNoteLinesCheck->SetValue(showNoteLines);
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
// Model: Data -> View Binding
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::PlotSTFTMatrix(
   const std::vector<std::vector<double>>& matrix,
   double maxFreq,
   size_t numSamples)
{
   mSuperSpectrogramPanel->SetData(
      matrix,
      maxFreq,
      numSamples
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

   if (NotifyNoiseFloorChanged)
      NotifyNoiseFloorChanged(value);

   if (OnRecomputeRequested)
      OnRecomputeRequested();
}

void SuperSpectrogramPlotDialog::OnHighestNoteChanged(wxCommandEvent&)
{
   int sel = mHighestNoteChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   int value = static_cast<int>(
      reinterpret_cast<intptr_t>(
         mHighestNoteChoice->GetClientData(sel)));

   if (NotifyHighestNoteChanged)
      NotifyHighestNoteChanged(value);

   if (OnRecomputeRequested)
      OnRecomputeRequested();

   UpdateLayoutPreservingState();
}

void SuperSpectrogramPlotDialog::OnColormapChanged(wxCommandEvent&)
{
   int sel = mColormapChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   int value = static_cast<int>(
      reinterpret_cast<intptr_t>(
         mColormapChoice->GetClientData(sel)));

   if (NotifyColormapChanged)
      NotifyColormapChanged(value);
}

void SuperSpectrogramPlotDialog::OnNoteNamingChanged(wxCommandEvent&)
{
   if (!mSuperSpectrogramPanel || !mNoteNamingChoice)
      return;

   int sel = mNoteNamingChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   int value = static_cast<int>(
      reinterpret_cast<intptr_t>(
         mNoteNamingChoice->GetClientData(sel)));

   if (NotifyNoteNamingChanged)
      NotifyNoteNamingChanged(value);

   auto style = static_cast<SuperSpectrogramPanel::NoteNamingStyle>(value);
   mSuperSpectrogramPanel->SetNoteNamingStyle(style);
}

void SuperSpectrogramPlotDialog::OnShowNoteLinesChanged(wxCommandEvent& event)
{
   if (!mSuperSpectrogramPanel)
      return;

   bool value = event.IsChecked();

   if (NotifyShowNoteLinesChanged)
      NotifyNoteNamingChanged(value);

   mSuperSpectrogramPanel->SetShowNoteLines(value);
}

void SuperSpectrogramPlotDialog::OnTimeTickChanged(wxCommandEvent&)
{
   if (!mSuperSpectrogramPanel || !mTimeTickChoice)
      return;

   int sel = mTimeTickChoice->GetSelection();
   if (sel == wxNOT_FOUND)
      return;

   int value = static_cast<int>(
      reinterpret_cast<intptr_t>(
         mTimeTickChoice->GetClientData(sel)));

   if (NotifyTimeTickChanged)
      NotifyTimeTickChanged(value);

   auto mode = static_cast<SuperSpectrogramPanel::TimeTickMode>(value);
   mSuperSpectrogramPanel->SetTimeTickMode(mode);
}

//-----------------------------------------------------------------
// Export: Data and rendering output
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::OnExport(wxCommandEvent&)
{
   if (NotifyExportRequested)
      NotifyExportRequested(this);
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

SuperSpectrogramPanel& SuperSpectrogramPlotDialog::GetPanel()
{
   return *mSuperSpectrogramPanel;
}

//-----------------------------------------------------------------
// Application integration & command registration
//-----------------------------------------------------------------
#include "CommandContext.h"
#include "CommandManager.h"
#include "ProjectWindows.h"
#include "CommonCommandFlags.h"
#include "SuperSpectrogramController.h"

namespace {
   SuperSpectrogramPlotDialog* gSpectrogramDialog = nullptr;
   void ClearSpectrogramDialog()
   {
      gSpectrogramDialog = nullptr;
   }

   static std::unique_ptr<SuperSpectrogramController> gController;
   static std::unique_ptr<SuperSpectrogramModel> gModel;
   static std::unique_ptr<SuperSpectrogramSettings> gSettings;

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

      // create dependencies
      gModel = std::make_unique<SuperSpectrogramModel>();
      gSettings = std::make_unique<SuperSpectrogramSettings>();

      gSettings->Load();

      gController = std::make_unique<SuperSpectrogramController>(
         *gModel,
         *gSettings,
         gSpectrogramDialog->GetPanel(),
         *gSpectrogramDialog
      );

      gController->BindView();
      gController->Initialize();

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
