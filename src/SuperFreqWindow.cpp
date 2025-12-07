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

/*
  Salvo Ventura - November 2006
  Extended range check for additional FFT windows
*/

#include "SuperFreqWindow.h"
#include <wx/wx.h>
#include "SpectrogramPanel.h"  // your panel class
#include "STFTProcessor.h"     // your STFT / SuperSpectrum logic

#define FrequencyAnalysisTitle XO("Frequency Analysis")

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
   , wxDialogWrapper(parent, id, title, pos, wxSize(1000, 400),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
   SetName();

   // Create sizer FIRST
   wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

   // Create and add the panel
   mSpectrogramPanel = std::make_unique<SpectrogramPanel>(this);
   mSpectrogramPanel->EnableNoteLines(true);
   mainSizer->Add(mSpectrogramPanel.get(), 1, wxEXPAND | wxALL, 5);

   // Add a status label for debugging
   wxStaticText* statusLabel = new wxStaticText(this, wxID_ANY,
      "Ready to display spectrogram...");
   mainSizer->Add(statusLabel, 0, wxALIGN_CENTER | wxALL, 5);

   SetSizer(mainSizer);

   // IMPORTANT: Call Fit() and Layout()
   Layout();
   Fit();
   Centre();

   // Force initial size
   SetSize(1000, 400);
}

SuperFrequencyPlotDialog::~SuperFrequencyPlotDialog() = default;

//-----------------------------------------------------------------
// Show / Hide dialog
//-----------------------------------------------------------------
bool SuperFrequencyPlotDialog::Show(bool show)
{
   if (show) {
      // Force a minimum size
      SetMinSize(wxSize(800, 600));

      if (!IsShown()) {
         if (!GetAudio()) {
            return false;  // Show error but don't crash
         }

         // Layout and center
         Layout();
         Fit();
         Centre();

         // Force an initial paint
         Recalc();
      }
   }

   return wxDialogWrapper::Show(show);
}

//-----------------------------------------------------------------
// Plot a 2D STFT matrix
//-----------------------------------------------------------------
void SuperFrequencyPlotDialog::PlotSTFTMatrix(const std::vector<std::vector<double>>& matrix)
{
   if (!mSpectrogramPanel)
      return;

   // Cache the matrix
   mMatrix = matrix;

   // Update the panel
   mSpectrogramPanel->SetMatrix(mMatrix);
   mSpectrogramPanel->ResetView();
}

//-----------------------------------------------------------------
// Recalculate the spectrogram from current selection
//-----------------------------------------------------------------
void SuperFrequencyPlotDialog::Recalc()
{
   // Ensure there is data
   if (!mData)
      return;

   // Calculate the spectrogram using SuperSpectrumAnalyst
   std::vector<std::vector<double>> matrix;
   SuperSpectrumAnalyst::Algorithm alg = SuperSpectrumAnalyst::Spectrum; // default
   int windowFunc = 0; // default window function

   float mYMax = 15.0;

   mAnalyst->Calculate(mWindowSize, mRate,
      mData.get(), mDataLen,
      0, &mYMax);  

   // Retrieve computed matrix
   matrix = mAnalyst->GetMatrix();  // assume you have a getter returning 2D vector

   // Plot it
   PlotSTFTMatrix(matrix);
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
   // Remove the singleton factory registration
   // AttachedWindows::RegisteredFactory sFrequencyWindowKey{...}

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
