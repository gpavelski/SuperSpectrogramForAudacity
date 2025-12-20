/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.cpp

  Tony Bee

*******************************************************************//**

\class SuperSpectrogramPlotDialog
\brief Displays a more detail spectrum plot of the waveform.
Has options for selecting parameters of the plot.

Has a feature that finds peaks and reports their value as you move
the mouse around.

*//****************************************************************//**

\class SuperSpectrogramPlot
\brief Works with SuperSpectrogramPlotDialog to display a more detailed
spectrum plot of the waveform.
This class actually does the graph display.

Has a feature that finds peaks and reports their value as you move
the mouse around.

*//*******************************************************************/


#include "SuperSpectrogramWindow.h"
#include <wx/wx.h>
#include "SpectrogramPanel.h"
#include "STFTProcessor.h"     

#define SuperSpectrogramTitle XO("Super Spectrogram")

//-----------------------------------------------------------------
// Event table for the dialog
//-----------------------------------------------------------------
BEGIN_EVENT_TABLE(SuperSpectrogramPlotDialog, wxDialogWrapper)
EVT_CLOSE(SuperSpectrogramPlotDialog::OnCloseWindow)
EVT_CHOICE(wxID_ANY, SuperSpectrogramPlotDialog::OnThresholdChanged)
END_EVENT_TABLE()

//-----------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------
SuperSpectrogramPlotDialog::SuperSpectrogramPlotDialog(
   wxWindow* parent,
   wxWindowID id,
   AudacityProject& project,
   const TranslatableString& title,
   const wxPoint& pos)
   : PlotSuperSpectrogramBase{ project }
   , wxDialogWrapper(parent, id, title, pos, wxSize(1000, 600),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
   SetName();

   auto* mainSizer = new wxBoxSizer(wxVERTICAL);

   CreateControls(mainSizer);

   mSpectrogramPanel = std::make_unique<SpectrogramPanel>(this);
   mSpectrogramPanel->EnableNoteLines(true);
   mainSizer->Add(mSpectrogramPanel.get(), 1, wxEXPAND | wxALL, 5);

   SetSizer(mainSizer);
}

SuperSpectrogramPlotDialog::~SuperSpectrogramPlotDialog() = default;

//-----------------------------------------------------------------
// Show / Hide dialog
//-----------------------------------------------------------------
bool SuperSpectrogramPlotDialog::Show(bool show)
{
   if (show && !IsShown()) {
      if (!GetAudio())
         return false;

      // 1) Compute spectrogram (this fills the matrix)
      Recalc();

      // 2) Apply data-driven size constraints
      ApplyDataDrivenMinSize();

      // 3) Finalize layout
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

//-----------------------------------------------------------------
// Plot a 2D STFT matrix
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::PlotSTFTMatrix(
   const std::vector<std::vector<double>>& matrix)
{
   if (!mSpectrogramPanel)
      return;

   mMatrix = matrix;
   double maxFreq = mRate / (2.0 * mDecimationLevel);  // Nyquist frequency after decimation
   mSpectrogramPanel->SetMatrix(mMatrix, maxFreq);
   mSpectrogramPanel->ResetView();
}

//-----------------------------------------------------------------
// Recalculate the spectrogram from current selection
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::Recalc()
{
   if (!mData)
      return;

   mAnalyst->Calculate(mData.get(),
      mDataLen,
      mDetailLevel,
      mDecimationLevel,
      mLowerThreshold);
   PlotSTFTMatrix(mAnalyst->GetMatrix());
}

void SuperSpectrogramPlotDialog::CreateControls(wxBoxSizer* mainSizer)
{
   auto* controlSizer = new wxBoxSizer(wxHORIZONTAL);

   controlSizer->Add(
      new wxStaticText(this, wxID_ANY, _("Noise floor (dB):")),
      0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

   static const wxString choices[] = {
      "-120", "-100", "-85", "-70", "-55"
   };

   mThresholdChoice = new wxChoice(
      this, wxID_ANY,
      wxDefaultPosition, wxDefaultSize,
      WXSIZEOF(choices), choices);

   // Default selection matches mLowerThreshold initial value
   mThresholdChoice->SetStringSelection(
      wxString::Format("%d", static_cast<int>(mLowerThreshold)));

   controlSizer->Add(mThresholdChoice, 0);

   mainSizer->Add(controlSizer, 0, wxLEFT | wxTOP | wxRIGHT, 8);
}

void SuperSpectrogramPlotDialog::OnThresholdChanged(wxCommandEvent& event)
{
   if (!mThresholdChoice)
      return;

   long value = 0;
   if (!mThresholdChoice->GetStringSelection().ToLong(&value))
      return;

   if (mLowerThreshold == value)
      return; // no-op

   mLowerThreshold = static_cast<int>(value);

   // Recompute spectrogram with new threshold
   Recalc();
}


//-----------------------------------------------------------------
// PrefsListener interface
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::UpdatePrefs()
{
   Layout();
   if (mSpectrogramPanel)
      mSpectrogramPanel->UpdatePrefs();
}

// Remaining code hooks this add-on into the application
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
// Event handlers
//-----------------------------------------------------------------
void SuperSpectrogramPlotDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
   gSpectrogramDialog = nullptr;
   Destroy();
}
