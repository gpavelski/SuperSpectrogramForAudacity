/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramWindow.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
#define __AUDACITY_SUPER_SPECTROGRAM_WINDOW__

#include <vector>
#include <memory>
#include <unordered_map>
#include <wx/font.h>
#include <wx/choice.h>
#include <wx/config.h>
#include <wx/display.h>
#include <wx/wx.h> 
#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramFrame.h"
#include "SuperSpectrogramPanel.h"
#include "wxPanelWrapper.h"

class AudacityProject;
class SuperSpectrogramPanel;
class SuperSpectrogramConfig;

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

   void Render(
      const SuperSpectrogramFrame& frame
   );

   void ApplyConfig(
      const SuperSpectrogramConfig& config
   );

   wxBitmap RenderToBitmap() const;

   SuperSpectrogramConfig BuildConfigFromUI() const;
   void ApplyDataDrivenMinSize();
   // Controller bindings
   std::function<void(const SuperSpectrogramConfig&)> NotifyConfigChanged;
   std::function<void(int format, const std::string& path)> NotifyExportRequested;

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

   void CreateControls(wxSizer* parentSizer);

   wxChoice* CreateChoice(
      wxWindow* parent,
      wxWindowID id,
      const std::vector<ChoiceOption>& options,
      int defaultValue,
      std::unordered_map<int, int>& outIndexMap,
      std::unordered_map<int, int>& outValueMap
   );

   void SetChoiceByValue(
      wxChoice* choice,
      const std::unordered_map<int, int>& indexMap,
      int value
   );

   int GetValueFromChoice(
      wxChoice* choice,
      const std::unordered_map<int, int>& valueMap
   ) const;

   void ApplyConfigToControls(
      const SuperSpectrogramConfig& config
   );

   void ApplyConfigToPanel(
      const SuperSpectrogramConfig& config
   );
   void OnNoiseFloorChanged(wxCommandEvent& event);
   void OnHighestNoteChanged(wxCommandEvent&);
   void OnColormapChanged(wxCommandEvent&);
   void OnNoteNamingChanged(wxCommandEvent&);
   void OnShowNoteLinesChanged(wxCommandEvent& event);
   void OnTimeTickChanged(wxCommandEvent&);
   void OnExport(wxCommandEvent& event);

   wxChoice* mNoiseFloorChoice = nullptr;
   wxChoice* mHighestNoteChoice = nullptr;
   wxChoice* mColormapChoice = nullptr;
   wxChoice* mNoteNamingChoice = nullptr;
   wxCheckBox* mShowNoteLinesCheck = nullptr;
   wxChoice* mTimeTickChoice = nullptr;
   wxButton* mExportButton = nullptr;

   std::unordered_map<int, int> mNoiseFloorIndexMap;
   std::unordered_map<int, int> mHighestNoteIndexMap;
   std::unordered_map<int, int> mColormapIndexMap;
   std::unordered_map<int, int> mNoteNamingIndexMap;
   std::unordered_map<int, int> mTimeTickIndexMap;

   std::unordered_map<int, int> mNoiseFloorValueMap;
   std::unordered_map<int, int> mHighestNoteValueMap;
   std::unordered_map<int, int> mColormapValueMap;
   std::unordered_map<int, int> mNoteNamingValueMap;
   std::unordered_map<int, int> mTimeTickValueMap;

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
   { "Jet",     static_cast<int>(SuperSpectrogramConfig::Colormap::Jet) },
   { "Gray",    static_cast<int>(SuperSpectrogramConfig::Colormap::Gray) },
   { "Hot",     static_cast<int>(SuperSpectrogramConfig::Colormap::Hot) },
   { "Viridis", static_cast<int>(SuperSpectrogramConfig::Colormap::Viridis) },
   { "Inferno",  static_cast<int>(SuperSpectrogramConfig::Colormap::Inferno) },
   { "Magma",  static_cast<int>(SuperSpectrogramConfig::Colormap::Magma) },
   { "Cividis",  static_cast<int>(SuperSpectrogramConfig::Colormap::Cividis) },
   { "Parula",  static_cast<int>(SuperSpectrogramConfig::Colormap::Parula) }
   };

   inline static const std::vector<ChoiceOption> kNoteNamingOptions = {
   { "Sharps (C#)", static_cast<int>(SuperSpectrogramConfig::NoteNaming::Sharps) },
   { "Flats (Db)",  static_cast<int>(SuperSpectrogramConfig::NoteNaming::Flats) },
   { "Mixed",       static_cast<int>(SuperSpectrogramConfig::NoteNaming::Mixed) }
   };

   static constexpr int kDefaultColormap =
      static_cast<int>(SuperSpectrogramConfig::Colormap::Jet);

   static constexpr int kDefaultNoteNaming =
      static_cast<int>(SuperSpectrogramConfig::NoteNaming::Mixed);

   inline static const std::vector<ChoiceOption> kTimeTickOptions = {
   { "Seconds", static_cast<int>(SuperSpectrogramConfig::TimeTickMode::Seconds) },
   { "Samples", static_cast<int>(SuperSpectrogramConfig::TimeTickMode::Samples) },
   { "None",    static_cast<int>(SuperSpectrogramConfig::TimeTickMode::None) }
   };

   static constexpr int kDefaultTimeTick =
      static_cast<int>(SuperSpectrogramConfig::TimeTickMode::Seconds);

   // Font for optional overlays (timestamps, peak labels, etc.)
   wxFont mFreqFont;

   // The panel that draws the spectrogram
   std::unique_ptr<SuperSpectrogramPanel> mPanel;

   DECLARE_EVENT_TABLE()
};

#endif // __AUDACITY_SUPER_SPECTROGRAM_WINDOW__
