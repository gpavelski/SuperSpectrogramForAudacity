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

#include "View/SuperSpectrogramView.h"

static const wxString SuperSpectrogramTitle = "Super Spectrogram";

//-----------------------------------------------------------------
// wxWidgets event table: maps UI events to view handlers.
// Handlers forward user actions to the controller.
//-----------------------------------------------------------------
BEGIN_EVENT_TABLE(SuperSpectrogramView, wxDialogWrapper)
  EVT_CHOICE(ID_NoiseFloorChoice, SuperSpectrogramView::OnConfigChanged)
  EVT_CHOICE(ID_HighestNoteChoice, SuperSpectrogramView::OnConfigChanged)
  EVT_CHOICE(ID_ColormapChoice, SuperSpectrogramView::OnConfigChanged)
  EVT_CHOICE(ID_NoteNamingChoice, SuperSpectrogramView::OnConfigChanged)
  EVT_CHOICE(ID_TimeTickChoice, SuperSpectrogramView::OnConfigChanged)

  EVT_CHECKBOX(ID_ShowNoteLinesCheck,
     SuperSpectrogramView::OnConfigChanged)

  EVT_BUTTON(wxID_SAVE, SuperSpectrogramView::OnExport)
END_EVENT_TABLE()

//-----------------------------------------------------------------
// Lifecycle
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
      wxSize(SuperSpectrogramConstants::UI::kDefaultWidth, SuperSpectrogramConstants::UI::kDefaultHeight),
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
// Rendering API
//-----------------------------------------------------------------
void SuperSpectrogramView::Render(const SuperSpectrogramFrame& frame)
{
   if (!mPanel)
      return;

   mPanel->SetData(frame);
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
   SetChoiceByValue(mColormapChoice, mColormapIndexMap, config.colormapId);
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

   mPanel->SetColormap(config.colormapId);
   mPanel->SetNoteNamingStyle(config.noteNaming);
   mPanel->SetShowNoteLines(config.showNoteLines);
   mPanel->SetTimeTickMode(config.timeTickMode);
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

   int columns = mPanel
      ? mPanel->GetColumnCount()
      : 0;

   if (columns == 0)
      return;

   // -----------------------------
   // Width: data-driven 
   // -----------------------------
   int visibleColumns = std::min(columns, SuperSpectrogramConstants::UI::kMaxVisibleColumns);
   int minWidth = visibleColumns * SuperSpectrogramConstants::UI::kPixelsPerColumn;

   // -----------------------------------
   // Height: derived from screen size
   // -----------------------------------
   wxDisplay display(GetParent() ? GetParent() : this);
   wxRect clientArea = display.GetClientArea();

   int minHeight = static_cast<int>(clientArea.GetHeight() * SuperSpectrogramConstants::UI::kHeightRatio);

   minHeight = std::max(minHeight, SuperSpectrogramConstants::UI::kMinHeightFallback);

   SetMinSize(wxSize(minWidth, minHeight));
}

//-----------------------------------------------------------------
// UI construction helpers
//-----------------------------------------------------------------
void SuperSpectrogramView::CreateControls(wxSizer* parentSizer)
{
   // Build toolbar containing all user-adjustable parameters
   auto* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

   // ---------------------------------------------------------
   // Noise floor
   // ---------------------------------------------------------
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Noise floor:")),
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      5);

   mNoiseFloorChoice = CreateChoice(
      this,
      ID_NoiseFloorChoice,
      SuperSpectrogramUIOptions::NoiseFloorOptions<int>(),
      SuperSpectrogramUIOptions::DefaultNoiseFloor<int>(),
      mNoiseFloorIndexMap,
      mNoiseFloorValueMap
   );

   toolbarSizer->Add(
      mNoiseFloorChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxRIGHT,
      10);

   // ---------------------------------------------------------
   // Detail level
   // ---------------------------------------------------------
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Highest Note:")),
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      5);

   mHighestNoteChoice = CreateChoice(
      this,
      ID_HighestNoteChoice,
      SuperSpectrogramUIOptions::DetailLevelOptions<int>(),
      SuperSpectrogramUIOptions::DefaultDetailLevel<int>(),
      mHighestNoteIndexMap,
      mHighestNoteValueMap
   );

   toolbarSizer->Add(
      mHighestNoteChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      15);

   // ---------------------------------------------------------
   // Note naming
   // ---------------------------------------------------------
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Note naming:")),
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      5);

   mNoteNamingChoice = CreateChoice(
      this,
      ID_NoteNamingChoice,
      SuperSpectrogramUIOptions::NoteNamingOptions<int>(),
      SuperSpectrogramUIOptions::DefaultNoteNaming<int>(),
      mNoteNamingIndexMap,
      mNoteNamingValueMap
   );

   toolbarSizer->Add(
      mNoteNamingChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      10);

   // ---------------------------------------------------------
   // Colormap
   // ---------------------------------------------------------
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Colormap:")),
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      5);

   mColormapChoice = CreateChoice(
      this,
      ID_ColormapChoice,
      SuperSpectrogramUIOptions::ColormapOptions<wxString>(),
      SuperSpectrogramUIOptions::DefaultColormap<wxString>(),
      mColormapIndexMap,
      mColormapValueMap
   );

   toolbarSizer->Add(
      mColormapChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      10);

   // ---------------------------------------------------------
   // Note overlay toggle
   // ---------------------------------------------------------
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

   // ---------------------------------------------------------
   // Time scale
   // ---------------------------------------------------------
   toolbarSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Time scale:")),
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      5);

   mTimeTickChoice = CreateChoice(
      this,
      ID_TimeTickChoice,
      SuperSpectrogramUIOptions::TimeTickOptions<int>(),
      SuperSpectrogramUIOptions::DefaultTimeTick<int>(),
      mTimeTickIndexMap,
      mTimeTickValueMap
   );

   toolbarSizer->Add(
      mTimeTickChoice,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      10);

   // ---------------------------------------------------------
   // Export
   // ---------------------------------------------------------
   mExportButton = new wxButton(this, wxID_SAVE, _("Export…"));

   toolbarSizer->Add(
      mExportButton,
      0,
      wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
      5);

   toolbarSizer->AddStretchSpacer();

   parentSizer->Add(
      toolbarSizer,
      0,
      wxEXPAND | wxALL,
      5);
}

template<typename T>
wxChoice* SuperSpectrogramView::CreateChoice(
   wxWindow* parent,
   int id,
   const std::vector<SuperSpectrogramOption<T>>& options,
   const T& defaultValue,
   std::unordered_map<T, int>& indexMap,
   std::unordered_map<int, T>& valueMap)
{
   wxArrayString labels;
   labels.reserve(options.size());

   for (size_t i = 0; i < options.size(); ++i)
   {
      labels.push_back(options[i].label);
      indexMap[options[i].value] = static_cast<int>(i);
      valueMap[static_cast<int>(i)] = options[i].value;
   }

   wxChoice* choice = new wxChoice(parent, id, wxDefaultPosition, wxDefaultSize, labels);

   // select default value
   auto it = indexMap.find(defaultValue);
   if (it != indexMap.end())
      choice->SetSelection(it->second);

   return choice;
}

template<typename T>
T SuperSpectrogramView::GetValueFromChoice(
   wxChoice* choice,
   const std::unordered_map<int, T>& valueMap) const
{
   if (!choice)
      return T{};

   int sel = choice->GetSelection();

   if (sel == wxNOT_FOUND)
      return T{};

   auto it = valueMap.find(sel);

   if (it != valueMap.end())
      return it->second;

   return T{};
}

template<typename T>
void SuperSpectrogramView::SetChoiceByValue(
   wxChoice* choice,
   const std::unordered_map<T, int>& indexMap,
   const T& value)
{
   if (!choice)
      return;

   auto it = indexMap.find(value);
   if (it != indexMap.end())
      choice->SetSelection(it->second);
}

SuperSpectrogramConfig SuperSpectrogramView::BuildConfigFromUI() const
{
   SuperSpectrogramConfig cfg;

   cfg.noiseFloor = GetValueFromChoice(mNoiseFloorChoice, mNoiseFloorValueMap);
   cfg.detailLevel = GetValueFromChoice(mHighestNoteChoice, mHighestNoteValueMap);

   cfg.colormapId = GetValueFromChoice(mColormapChoice, mColormapValueMap);

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
void SuperSpectrogramView::OnConfigChanged(wxCommandEvent&)
{
   if (NotifyConfigChanged)
      NotifyConfigChanged(BuildConfigFromUI());
}

void SuperSpectrogramView::ShowError(const wxString& message)
{
   wxMessageBox(message, SuperSpectrogramTitle, wxOK | wxICON_ERROR, this);
}

void SuperSpectrogramView::ShowWarning(const wxString& message)
{
   wxMessageBox(message, SuperSpectrogramTitle, wxOK | wxICON_WARNING, this);
}

void SuperSpectrogramView::SetExportEnabled(bool enabled)
{
   if (mExportButton)
      mExportButton->Enable(enabled);
}

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