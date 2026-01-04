/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
#define __AUDACITY_SUPER_SPECTROGRAM_WINDOW__

#include "PlotSuperSpectrogramBase.h"
#include <vector>
#include <memory>
#include <wx/font.h>
#include <wx/choice.h>
#include "wxPanelWrapper.h"

class AudacityProject;
class SpectrogramPanel;

//=================================================================
// SuperSpectrogramPlotDialog: hosts the SpectrogramPanel
//=================================================================
class SuperSpectrogramPlotDialog final :
   public PlotSuperSpectrogramBase,   // used for spectrum data computation
   public wxDialogWrapper
{
public:
   // Constructor / Destructor
   SuperSpectrogramPlotDialog(wxWindow* parent, wxWindowID id,
      AudacityProject& project,
      const TranslatableString& title,
      const wxPoint& pos = wxDefaultPosition);
   virtual ~SuperSpectrogramPlotDialog();

   // Override Show() to display dialog
   bool Show(bool show = true) override;

   bool IsAudioSelectionValid();

   // Feed a 2D STFT / spectrogram matrix to the panel
   void PlotSTFTMatrix(const std::vector<std::vector<double>>& matrix);

   // Recalculate the spectrogram from the current selection
   void Recalc();

private:

   enum {
      ID_NoiseFloorChoice = wxID_HIGHEST + 200,
      ID_HighestNoteChoice
   };

   struct ChoiceOption
   {
      wxString label;
      int value;
   };

   void ApplyDataDrivenMinSize();
   void UpdateLayoutPreservingState();
   // Event handlers
   void OnCloseWindow(wxCloseEvent& event);

   void CreateControls(wxSizer* parentSizer);
   wxChoice* CreateChoice(
      wxWindow* parent,
      wxWindowID id,
      const std::vector<ChoiceOption>& options,
      int defaultValue);
   void OnNoiseFloorChanged(wxCommandEvent& event);
   void OnHighestNoteChanged(wxCommandEvent&);

   void OnExport(wxCommandEvent& event);
   void ExportMatrixAsText();
   void ExportViewAsPNG();

   wxChoice* mNoiseFloorChoice = nullptr;
   wxChoice* mHighestNoteChoice = nullptr;
   wxButton* mExportButton = nullptr;

   size_t mDetailLevel = 7;
   size_t mNumSamples = 0;
   double mMaxFreq = 1.0;
   size_t mNoiseFloor = -70;

   const std::vector<ChoiceOption> kNoiseFloorOptions{
      { "-120 dB", -120 },
      { "-100 dB", -100 },
      {  "-85 dB",  -85 },
      {  "-70 dB",  -70 },
      {  "-55 dB",  -55 }
   };

   const int kDefaultNoiseFloor = -70;

   const std::vector<ChoiceOption> kHighestNoteOptions{
      { "C4", 4 },
      { "C5", 5 },
      { "C6", 6 },
      { "C7", 7 },
      { "C8", 8 }
   };

   const int kDefaultHighestNote = 7;

   // Font for optional overlays (timestamps, peak labels, etc.)
   wxFont mFreqFont;

   // The panel that draws the spectrogram
   std::unique_ptr<SpectrogramPanel> mSpectrogramPanel;

   // Current STFT / spectrogram data (optional cache)
   std::vector<std::vector<double>> mMatrix;

   DECLARE_EVENT_TABLE()
};

#endif // __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
