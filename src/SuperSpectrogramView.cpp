/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.cpp

  Guilherme Pavelski

*******************************************************************//**

 * @class SuperSpectrogramView
 * @brief Dialog-based view responsible for presenting the Super Spectrogram UI.
 *
 * This class implements the "View" in the MVC architecture. It owns the
 * SuperSpectrogramPanel, which performs the actual rendering of the
 * spectrogram matrix.
 *
 * Responsibilities:
 *  - Create and manage all UI controls (toolbar, selectors, export button).
 *  - Own and update the SuperSpectrogramPanel for visualization.
 *  - Forward user interactions to the controller via callback functions.
 *  - Apply configuration and data received from the controller.
 *  - Manage layout and sizing based on spectrogram content.
 *  - Provide rendering output (e.g., bitmap export) to the controller.
 *
 * 
*//*******************************************************************/

#include "SuperSpectrogramView.h"

#define SuperSpectrogramTitle XO("Super Spectrogram")

static const wxString kConfigPath = "/SuperSpectrogram";

//-----------------------------------------------------------------
// wxWidgets event table: maps UI events to view handlers.
// Handlers forward user actions to the controller.
//-----------------------------------------------------------------
BEGIN_EVENT_TABLE(SuperSpectrogramView, wxDialogWrapper)
   EVT_CHOICE(ID_NoiseFloorChoice, SuperSpectrogramView::OnNoiseFloorChanged)
   EVT_CHOICE(ID_HighestNoteChoice, SuperSpectrogramView::OnHighestNoteChanged)
   EVT_CHOICE(ID_ColormapChoice, SuperSpectrogramView::OnColormapChanged)
   EVT_CHOICE(ID_NoteNamingChoice, SuperSpectrogramView::OnNoteNamingChanged)
   EVT_CHECKBOX(ID_ShowNoteLinesCheck, SuperSpectrogramView::OnShowNoteLinesChanged)
   EVT_CHOICE(ID_TimeTickChoice, SuperSpectrogramView::OnTimeTickChanged)
   EVT_BUTTON(wxID_SAVE, SuperSpectrogramView::OnExport)
END_EVENT_TABLE()

//-----------------------------------------------------------------
// Construction
//-----------------------------------------------------------------
SuperSpectrogramView::SuperSpectrogramView(
   wxWindow* parent,
   wxWindowID id,
   const TranslatableString& title,
   const wxPoint& pos)
   : wxDialogWrapper(
      parent,
      id,
      title,
      pos,
      wxSize(1000, 600),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
   SetName();

   // Create top-level layout
   auto* mainSizer = new wxBoxSizer(wxVERTICAL);

   // Build toolbar controls (selectors, export button)
   CreateControls(mainSizer);

   // Create the rendering panel (owned exclusively by the view)
   mPanel = std::make_unique<SuperSpectrogramPanel>(this);

   // Panel occupies remaining space
   mainSizer->Add(mPanel.get(), 1, wxEXPAND | wxALL, 5);

   SetSizer(mainSizer);
}

SuperSpectrogramView::~SuperSpectrogramView() = default;

//-----------------------------------------------------------------
// Visibility lifecycle
//-----------------------------------------------------------------
bool SuperSpectrogramView::Show(bool show)
{
   // Trigger recomputation on first show and adjust layout dynamically
   if (show && !IsShown()) {
      ApplyDataDrivenMinSize();
      Layout();
      Fit();
      Centre();
   }

   return wxDialogWrapper::Show(show);
}

//-----------------------------------------------------------------
// Data & configuration application (Controller -> View)
//-----------------------------------------------------------------
void SuperSpectrogramView::SetSpectrogramData(
   const std::vector<std::vector<double>>& matrix,
   double maxFreq,
   size_t numSamples)
{
   if (!mPanel)
      return;

   mPanel->SetData(matrix, maxFreq, numSamples);
   mPanel->ResetView();
}

void SuperSpectrogramView::ApplyConfig(
   const SuperSpectrogramConfig& config)
{
   ApplyConfigToControls(config);
   ApplyConfigToPanel(config);
}

void SuperSpectrogramView::ApplyConfigToControls(
   const SuperSpectrogramConfig& config)
{
   SetChoiceByValue(mNoiseFloorChoice, mNoiseFloorIndexMap, config.noiseFloor);
   SetChoiceByValue(mHighestNoteChoice, mHighestNoteIndexMap, config.detailLevel);
   SetChoiceByValue(mColormapChoice, mColormapIndexMap, static_cast<int>(config.colormap));
   SetChoiceByValue(mNoteNamingChoice, mNoteNamingIndexMap, static_cast<int>(config.noteNaming));
   SetChoiceByValue(mTimeTickChoice, mTimeTickIndexMap, static_cast<int>(config.timeTickMode));

   if (mShowNoteLinesCheck)
      mShowNoteLinesCheck->SetValue(config.showNoteLines);
}

void SuperSpectrogramView::ApplyConfigToPanel(
   const SuperSpectrogramConfig& config)
{
   if (!mPanel)
      return;

   mPanel->SetColormap(config.colormap);
   mPanel->SetNoteNamingStyle(config.noteNaming);
   mPanel->SetShowNoteLines(config.showNoteLines);
   mPanel->SetTimeTickMode(config.timeTickMode);
}

void SuperSpectrogramView::SetChoiceByValue(
   wxChoice* choice,
   const std::unordered_map<int, int>& indexMap,
   int value)
{
   if (!choice)
      return;

   auto it = indexMap.find(value);
   if (it != indexMap.end())
      choice->SetSelection(it->second);
}

wxBitmap SuperSpectrogramView::RenderToBitmap() const
{
   // Render current panel contents to a bitmap (used for export)
   if (!mPanel)
      return {};

   return mPanel->RenderCurrentViewToBitmap();
}

//-----------------------------------------------------------------
// Layout management
//-----------------------------------------------------------------
void SuperSpectrogramView::ApplyDataDrivenMinSize()
{
   // Adjust minimum dialog size based on spectrogram dimensions.
   // Width scales with number of columns (capped), height scales with screen size.
   constexpr int MAX_VISIBLE_COLUMNS = 800;
   constexpr int PIXELS_PER_COLUMN = 1;

   int columns = mPanel
      ? mPanel->GetColumnCount()
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

//-----------------------------------------------------------------
// UI construction helpers
//-----------------------------------------------------------------
void SuperSpectrogramView::CreateControls(wxSizer* parentSizer)
{
   // Build toolbar containing all user-adjustable parameters
   auto* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Noise floor:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mNoiseFloorChoice = CreateChoice(
      this,
      ID_NoiseFloorChoice,
      kNoiseFloorOptions,
      kDefaultNoiseFloor,
      mNoiseFloorIndexMap,
      mNoiseFloorValueMap
   );

   toolbarSizer->Add(
      mNoiseFloorChoice,
      0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Highest Note:")),
      0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

   mHighestNoteChoice = CreateChoice(
      this,
      ID_HighestNoteChoice,
      kHighestNoteOptions,
      kDefaultHighestNote,
      mHighestNoteIndexMap,
      mHighestNoteValueMap
   );

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
      kDefaultNoteNaming,
      mNoteNamingIndexMap,
      mNoteNamingValueMap
   );

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
      kDefaultColormap,
      mColormapIndexMap,
      mColormapValueMap
   );

   toolbarSizer->Add(
      mColormapChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT |wxRIGHT,
      10);

   mShowNoteLinesCheck = new wxCheckBox(
      this,
      ID_ShowNoteLinesCheck,
      _("Show notes"));

   mShowNoteLinesCheck->SetValue(mInitialShowNoteLines);

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
      kDefaultTimeTick,
      mTimeTickIndexMap,
      mTimeTickValueMap
   );

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

wxChoice* SuperSpectrogramView::CreateChoice(
   wxWindow* parent,
   wxWindowID id,
   const std::vector<ChoiceOption>& options,
   int defaultValue,
   std::unordered_map<int, int>& outIndexMap,
   std::unordered_map<int, int>& outValueMap)
{
   auto* choice = new wxChoice(parent, id);

   int defaultIndex = wxNOT_FOUND;

   for (size_t i = 0; i < options.size(); ++i)
   {
      choice->Append(options[i].label);

      outIndexMap[options[i].value] = static_cast<int>(i);
      outValueMap[static_cast<int>(i)] = options[i].value;

      if (options[i].value == defaultValue)
         defaultIndex = static_cast<int>(i);
   }

   if (defaultIndex != wxNOT_FOUND)
      choice->SetSelection(defaultIndex);

   return choice;
}

int SuperSpectrogramView::GetValueFromChoice(
   wxChoice* choice,
   const std::unordered_map<int, int>& valueMap
) const
{
   if (!choice)
      return 0;

   int sel = choice->GetSelection();
   if (sel == wxNOT_FOUND)
      return 0;

   auto it = valueMap.find(sel);
   if (it != valueMap.end())
      return it->second;

   return 0;
}

SuperSpectrogramConfig SuperSpectrogramView::BuildConfigFromUI() const
{
   SuperSpectrogramConfig cfg;

   cfg.noiseFloor = GetValueFromChoice(mNoiseFloorChoice, mNoiseFloorValueMap);
   cfg.detailLevel = GetValueFromChoice(mHighestNoteChoice, mHighestNoteValueMap);

   cfg.colormap = static_cast<SuperSpectrogramConfig::Colormap>(
      GetValueFromChoice(mColormapChoice, mColormapValueMap));

   cfg.noteNaming = static_cast<SuperSpectrogramConfig::NoteNaming>(
      GetValueFromChoice(mNoteNamingChoice, mNoteNamingValueMap));

   cfg.showNoteLines = mShowNoteLinesCheck
      ? mShowNoteLinesCheck->GetValue()
      : true;

   cfg.timeTickMode = static_cast<SuperSpectrogramConfig::TimeTickMode>(
      GetValueFromChoice(mTimeTickChoice, mTimeTickValueMap));

   return cfg;
}
//-----------------------------------------------------------------
// UI event handlers (View -> Controller)
//-----------------------------------------------------------------
void SuperSpectrogramView::OnNoiseFloorChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

void SuperSpectrogramView::OnHighestNoteChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

void SuperSpectrogramView::OnColormapChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

void SuperSpectrogramView::OnNoteNamingChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

void SuperSpectrogramView::OnShowNoteLinesChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

void SuperSpectrogramView::OnTimeTickChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

//-----------------------------------------------------------------
// Export
//-----------------------------------------------------------------
void SuperSpectrogramView::OnExport(wxCommandEvent&)
{
   if (!NotifyExportRequested)
      return;

   wxArrayString choices;
   choices.Add("Export matrix as text (.txt)");
   choices.Add("Export current view as image (.png)");

   wxSingleChoiceDialog dlg(
      this,
      "Choose export format",
      "Export Spectrogram",
      choices);

   if (dlg.ShowModal() != wxID_OK)
      return;

   const int selection = dlg.GetSelection();

   wxFileDialog fileDlg(
      this,
      "Save file",
      "",
      selection == 0 ? "spectrogram.txt" : "spectrogram.png",
      selection == 0
      ? "Text files (*.txt)|*.txt"
      : "PNG files (*.png)|*.png",
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

   if (fileDlg.ShowModal() != wxID_OK)
      return;

   NotifyExportRequested(selection, fileDlg.GetPath().ToStdString());
}

//-----------------------------------------------------------------
// Application integration (command registration)
//-----------------------------------------------------------------
#include "CommandContext.h"
#include "CommandManager.h"
#include "ProjectWindows.h"
#include "CommonCommandFlags.h"
#include "SuperSpectrogramSession.h"

namespace {
   std::unique_ptr<SuperSpectrogramSession> gSession;

   void OnPlotSuperSpectrogram(const CommandContext& context)
   {
      auto& project = context.project;
      CommandManager::Get(project).RegisterLastAnalyzer(context);

      if (!gSession)
         gSession = std::make_unique<SuperSpectrogramSession>(project);

      gSession->Show();
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
