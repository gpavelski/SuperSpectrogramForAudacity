/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
#define __AUDACITY_SUPER_SPECTROGRAM_WINDOW__

#include <vector>
#include <memory>
#include <wx/font.h>
#include <wx/choice.h>
#include "SuperSpectrogramPanel.h"
#include "wxPanelWrapper.h"

class AudacityProject;
class SuperSpectrogramPanel;

//=================================================================
// SuperSpectrogramView: hosts the SuperSpectrogramPanel
//=================================================================
class SuperSpectrogramView final :
   public wxDialogWrapper
{
public:
   // Constructor / Destructor
   SuperSpectrogramView(
      wxWindow* parent,
      wxWindowID id,
      const TranslatableString& title,
      const wxPoint& pos = wxDefaultPosition);
   virtual ~SuperSpectrogramView();

   // Override Show() to display dialog
   bool Show(bool show = true) override;

   SuperSpectrogramPanel& GetPanel();

   void ApplySettings(
      int noiseFloor,
      int detailLevel,
      int colormap,
      int noteNaming,
      bool showNoteLines,
      int timeTickMode);

   // Feed a 2D STFT / spectrogram matrix to the panel
   void PlotSTFTMatrix(
      const std::vector<std::vector<double>>& matrix,
      double maxFreq,
      size_t numSamples);

  // View -> Controller event interface
   std::function<void(int)> NotifyNoiseFloorChanged;
   std::function<void(int)> NotifyHighestNoteChanged;
   std::function<void(int)> NotifyColormapChanged;
   std::function<void(int)> NotifyNoteNamingChanged;
   std::function<void(bool)> NotifyShowNoteLinesChanged;
   std::function<void(int)> NotifyTimeTickChanged;
   std::function<void()> OnRecomputeRequested;
   std::function<void()> NotifyApplySettingsToView;
   std::function<void(wxWindow*)> NotifyExportRequested;

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
   void SetChoiceByValue(wxChoice* choice, int value);

   wxChoice* mNoiseFloorChoice = nullptr;
   wxChoice* mHighestNoteChoice = nullptr;
   wxChoice* mColormapChoice = nullptr;
   wxChoice* mNoteNamingChoice = nullptr;
   wxCheckBox* mShowNoteLinesCheck = nullptr;
   wxChoice* mTimeTickChoice = nullptr;
   wxButton* mExportButton = nullptr;

   bool mInitialShowNoteLines = true;
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
   { "Jet",     static_cast<int>(SuperSpectrogramPanel::ColormapType::Jet) },
   { "Gray",    static_cast<int>(SuperSpectrogramPanel::ColormapType::Gray) },
   { "Hot",     static_cast<int>(SuperSpectrogramPanel::ColormapType::Hot) },
   { "Viridis", static_cast<int>(SuperSpectrogramPanel::ColormapType::Viridis) },
   { "Inferno",  static_cast<int>(SuperSpectrogramPanel::ColormapType::Inferno) },
   { "Magma",  static_cast<int>(SuperSpectrogramPanel::ColormapType::Magma) },
   { "Cividis",  static_cast<int>(SuperSpectrogramPanel::ColormapType::Cividis) },
   { "Parula",  static_cast<int>(SuperSpectrogramPanel::ColormapType::Parula) }
   };

   inline static const std::vector<ChoiceOption> kNoteNamingOptions = {
   { "Sharps (C#)", static_cast<int>(SuperSpectrogramPanel::NoteNamingStyle::Sharps) },
   { "Flats (Db)",  static_cast<int>(SuperSpectrogramPanel::NoteNamingStyle::Flats) },
   { "Mixed",       static_cast<int>(SuperSpectrogramPanel::NoteNamingStyle::Mixed) }
   };

   static constexpr int kDefaultColormap =
      static_cast<int>(SuperSpectrogramPanel::ColormapType::Jet);

   static constexpr int kDefaultNoteNaming =
      static_cast<int>(SuperSpectrogramPanel::NoteNamingStyle::Mixed);

   inline static const std::vector<ChoiceOption> kTimeTickOptions = {
   { "Seconds", static_cast<int>(SuperSpectrogramPanel::TimeTickMode::Seconds) },
   { "Samples", static_cast<int>(SuperSpectrogramPanel::TimeTickMode::Samples) },
   { "None",    static_cast<int>(SuperSpectrogramPanel::TimeTickMode::None) }
   };

   static constexpr int kDefaultTimeTick =
      static_cast<int>(SuperSpectrogramPanel::TimeTickMode::Seconds);

   // Font for optional overlays (timestamps, peak labels, etc.)
   wxFont mFreqFont;

   // The panel that draws the spectrogram
   std::unique_ptr<SuperSpectrogramPanel> mSuperSpectrogramPanel;

   DECLARE_EVENT_TABLE()
};

#endif // __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
