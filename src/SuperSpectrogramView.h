/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramView.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_VIEW__
#define __AUDACITY_SUPER_SPECTROGRAM_VIEW__

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wx/choice.h>
#include <wx/display.h>
#include <wx/font.h>
#include <wx/wx.h>

#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramFrame.h"
#include "SuperSpectrogramPanel.h"
#include "SuperSpectrogramUIOptions.h"
#include "wxPanelWrapper.h"

class AudacityProject;
class SuperSpectrogramPanel;

//=================================================================
// SuperSpectrogramView
//
// Main dialog responsible for:
//
// - Hosting the spectrogram rendering panel
// - Managing user controls
// - Translating UI state into configuration objects
// - Forwarding user actions to the controller
//
// The view owns no DSP or rendering business logic.
//=================================================================

class SuperSpectrogramView final :
   public wxDialogWrapper
{
public:
   // ---------------------------------------------------------
   // Construction
   // ---------------------------------------------------------

   SuperSpectrogramView(
      wxWindow* parent,
      wxWindowID id,
      const TranslatableString& title,
      const wxPoint& pos = wxDefaultPosition
   );

   ~SuperSpectrogramView() override;

   // ---------------------------------------------------------
   // Controller -> View API
   // ---------------------------------------------------------

   void Render(const SuperSpectrogramFrame& frame);

   void ApplyConfig(
      const SuperSpectrogramConfig& config
   );

   wxBitmap RenderToBitmap() const;

   void ApplyDataDrivenMinSize();

   void ShowError(const wxString& message);
   void ShowWarning(const wxString& message);

   void SetExportEnabled(bool enabled);

   // ---------------------------------------------------------
   // Controller callbacks
   // ---------------------------------------------------------

   std::function<void(const SuperSpectrogramConfig&)>
      NotifyConfigChanged;

   std::function<void(int format, const std::string& path)>
      NotifyExportRequested;

private:
   // ---------------------------------------------------------
   // Widget identifiers
   // ---------------------------------------------------------

   enum
   {
      ID_NoiseFloorChoice = wxID_HIGHEST + 200,
      ID_HighestNoteChoice,
      ID_ColormapChoice,
      ID_ShowNoteLinesCheck,
      ID_TimeTickChoice,
      ID_NoteNamingChoice
   };

   // ---------------------------------------------------------
   // UI construction helpers
   // ---------------------------------------------------------

   void CreateControls(wxSizer* parentSizer);

   wxChoice* CreateChoice(
      wxWindow* parent,
      wxWindowID id,
      const std::vector<SuperSpectrogramOption>& options,
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

   // ---------------------------------------------------------
   // Configuration synchronization
   // ---------------------------------------------------------

   void ApplyConfigToControls(
      const SuperSpectrogramConfig& config
   );

   void ApplyConfigToPanel(
      const SuperSpectrogramConfig& config
   );

   SuperSpectrogramConfig BuildConfigFromUI() const;

   // ---------------------------------------------------------
   // UI event handlers
   // ---------------------------------------------------------

   void OnConfigChanged(wxCommandEvent& event);

   void OnExport(wxCommandEvent& event);

private:
   // ---------------------------------------------------------
   // Controls
   // ---------------------------------------------------------

   wxChoice* mNoiseFloorChoice = nullptr;
   wxChoice* mHighestNoteChoice = nullptr;
   wxChoice* mColormapChoice = nullptr;
   wxChoice* mNoteNamingChoice = nullptr;
   wxChoice* mTimeTickChoice = nullptr;

   wxCheckBox* mShowNoteLinesCheck = nullptr;

   wxButton* mExportButton = nullptr;

   // ---------------------------------------------------------
   // Choice mappings
   //
   // value -> index
   // index -> value
   // ---------------------------------------------------------

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

   // ---------------------------------------------------------
   // Initial UI state
   // ---------------------------------------------------------

   bool mInitialShowNoteLines = true;

   // ---------------------------------------------------------
   // Rendering panel
   // ---------------------------------------------------------

   std::unique_ptr<SuperSpectrogramPanel> mPanel;

   wxDECLARE_EVENT_TABLE();
};

#endif // __AUDACITY_SUPER_SPECTROGRAM_VIEW__
