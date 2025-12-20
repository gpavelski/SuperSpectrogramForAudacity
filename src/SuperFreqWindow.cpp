/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperFreqWindow.cpp

  Tony Bee

*******************************************************************//**

\class SuperFrequencyPlotDialog
\brief Displays a more detail spectrum plot of the waveform.
Has options for selecting parameters of the plot.

Has a feature that finds peaks and reports their value as you move
the mouse around.

*//****************************************************************//**

\class SuperFreqPlot
\brief Works with SuperFrequencyPlotDialog to display a more detailed
spectrum plot of the waveform.
This class actually does the graph display.

Has a feature that finds peaks and reports their value as you move
the mouse around.

*//*******************************************************************/


#include "SuperFreqWindow.h"
#include <wx/wx.h>
#include "SpectrogramPanel.h"  // your panel class
#include "STFTProcessor.h"     // your STFT / SuperSpectrum logic

#define FrequencyAnalysisTitle XO("Super Spectrogram")

//-----------------------------------------------------------------
// Event table for the dialog
//-----------------------------------------------------------------
BEGIN_EVENT_TABLE(SuperFrequencyPlotDialog, wxDialogWrapper)
EVT_CLOSE(SuperFrequencyPlotDialog::OnCloseWindow)
END_EVENT_TABLE()

//-----------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------
SuperFrequencyPlotDialog::SuperFrequencyPlotDialog(
   wxWindow* parent,
   wxWindowID id,
   AudacityProject& project,
   const TranslatableString& title,
   const wxPoint& pos)
   : PlotSuperSpectrumBase{ project }
   , wxDialogWrapper(parent, id, title, pos, wxSize(1000, 600),  // Larger initial size
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
   SetName();

   auto* mainSizer = new wxBoxSizer(wxVERTICAL);

   mSpectrogramPanel = std::make_unique<SpectrogramPanel>(this);
   mSpectrogramPanel->EnableNoteLines(true);

   mainSizer->Add(mSpectrogramPanel.get(), 1, wxEXPAND | wxALL, 5);
   SetSizer(mainSizer);
}

SuperFrequencyPlotDialog::~SuperFrequencyPlotDialog() = default;

//-----------------------------------------------------------------
// Show / Hide dialog
//-----------------------------------------------------------------
bool SuperFrequencyPlotDialog::Show(bool show)
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

void SuperFrequencyPlotDialog::ApplyDataDrivenMinSize()
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
void SuperFrequencyPlotDialog::PlotSTFTMatrix(
   const std::vector<std::vector<double>>& matrix)
{
   if (!mSpectrogramPanel)
      return;

   mMatrix = matrix;
   mSpectrogramPanel->SetMatrix(mMatrix, mRate, mDecimationLevel);
   mSpectrogramPanel->ResetView();
}

//-----------------------------------------------------------------
// Recalculate the spectrogram from current selection
//-----------------------------------------------------------------
void SuperFrequencyPlotDialog::Recalc()
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

//-----------------------------------------------------------------
// Event handlers
//-----------------------------------------------------------------
void SuperFrequencyPlotDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
   Show(false);
}

//-----------------------------------------------------------------
// PrefsListener interface
//-----------------------------------------------------------------
void SuperFrequencyPlotDialog::UpdatePrefs()
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
   // Define our extra menu item
   void OnPlotSuperSpectrum(const CommandContext& context)
   {
      auto& project = context.project;
      CommandManager::Get(project).RegisterLastAnalyzer(context);

      // Create a NEW dialog instance each time
      auto* dialog = new SuperFrequencyPlotDialog(
         &GetProjectFrame(project),
         wxID_ANY,
         project,
         FrequencyAnalysisTitle,
         wxPoint{ 150, 150 }
      );

      // Show it modally or non-modally
      dialog->Show(true);
      dialog->Raise();
      dialog->SetFocus();
   }

   // Register that menu item
   using namespace MenuRegistry;
   AttachedItem sAttachment{
       Command(wxT("PlotSuperSpectrum"), XXO("Plot Super Spectrum..."),
           OnPlotSuperSpectrum,
           AudioIONotBusyFlag() | WaveTracksSelectedFlag() | TimeSelectedFlag()),
       wxT("Analyze/Analyzers/Windows")
   };
}
