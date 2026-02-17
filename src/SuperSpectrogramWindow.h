/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
#define __AUDACITY_SUPER_SPECTROGRAM_WINDOW__

#include "PlotSuperSpectrogramBase.h"
#include "SpectrogramPanel.h"
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
      ID_HighestNoteChoice,
      ID_ColormapChoice,
      ID_ShowNoteLinesCheck,
      ID_TimeTickChoice,
      ID_NoteNamingChoice
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
   void OnColormapChanged(wxCommandEvent&);
   void OnNoteNamingChanged(wxCommandEvent&);
   void OnShowNoteLinesChanged(wxCommandEvent& event);
   void OnTimeTickChanged(wxCommandEvent&);

   void OnExport(wxCommandEvent& event);
   void ExportMatrixAsText();
   void ExportViewAsPNG();
   void LoadSettings();
   void SaveSettings();
   void SetChoiceByValue(wxChoice* choice, int value);

   wxChoice* mNoiseFloorChoice = nullptr;
   wxChoice* mHighestNoteChoice = nullptr;
   wxChoice* mColormapChoice = nullptr;
   wxChoice* mNoteNamingChoice = nullptr;
   wxCheckBox* mShowNoteLinesCheck = nullptr;
   wxChoice* mTimeTickChoice = nullptr;
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

   inline static const std::vector<ChoiceOption> kColormapOptions = {
   { "Jet",     static_cast<int>(SpectrogramPanel::ColormapType::Jet) },
   { "Gray",    static_cast<int>(SpectrogramPanel::ColormapType::Gray) },
   { "Hot",     static_cast<int>(SpectrogramPanel::ColormapType::Hot) },
   { "Viridis", static_cast<int>(SpectrogramPanel::ColormapType::Viridis) },
   { "Inferno",  static_cast<int>(SpectrogramPanel::ColormapType::Inferno) },
   { "Magma",  static_cast<int>(SpectrogramPanel::ColormapType::Magma) },
   { "Cividis",  static_cast<int>(SpectrogramPanel::ColormapType::Cividis) },
   { "Parula",  static_cast<int>(SpectrogramPanel::ColormapType::Parula) }
   };

   inline static const std::vector<ChoiceOption> kNoteNamingOptions = {
   { "Sharps (C#)", static_cast<int>(SpectrogramPanel::NoteNamingStyle::Sharps) },
   { "Flats (Db)",  static_cast<int>(SpectrogramPanel::NoteNamingStyle::Flats) },
   { "Mixed",       static_cast<int>(SpectrogramPanel::NoteNamingStyle::Mixed) }
   };

   static constexpr int kDefaultColormap =
      static_cast<int>(SpectrogramPanel::ColormapType::Jet);

   static constexpr int kDefaultNoteNaming =
      static_cast<int>(SpectrogramPanel::NoteNamingStyle::Mixed);

   inline static const std::vector<ChoiceOption> kTimeTickOptions = {
   { "Seconds", static_cast<int>(SpectrogramPanel::TimeTickMode::Seconds) },
   { "Samples", static_cast<int>(SpectrogramPanel::TimeTickMode::Samples) },
   { "None",    static_cast<int>(SpectrogramPanel::TimeTickMode::None) }
   };

   static constexpr int kDefaultTimeTick =
      static_cast<int>(SpectrogramPanel::TimeTickMode::Seconds);

   // Font for optional overlays (timestamps, peak labels, etc.)
   wxFont mFreqFont;

   // The panel that draws the spectrogram
   std::unique_ptr<SpectrogramPanel> mSpectrogramPanel;

   // Current STFT / spectrogram data (optional cache)
   std::vector<std::vector<double>> mMatrix;

   DECLARE_EVENT_TABLE()
};

#endif // __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
