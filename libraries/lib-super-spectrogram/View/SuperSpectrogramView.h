/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramView.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <wx/choice.h>
#include <wx/display.h>
#include <wx/font.h>
#include <wx/wx.h>

#include "Config/SuperSpectrogramConfig.h"
#include "Config/SuperSpectrogramUIOptions.h"
#include "DataAdapter/SuperSpectrogramFrame.h"
#include "View/SuperSpectrogramPanel.h"
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

   template<typename T>
   wxChoice* CreateChoice(
      wxWindow* parent,
      wxWindowID id,
      const std::vector<SuperSpectrogramOption<T>>& options,
      const T& defaultValue,
      std::unordered_map<T, int>& outIndexMap,
      std::unordered_map<int, T>& outValueMap
   );

   template<typename T>
   void SetChoiceByValue(
      wxChoice* choice,
      const std::unordered_map<T, int>& indexMap,
      const T& value
   );

   template<typename T>
   T GetValueFromChoice(
      wxChoice* choice,
      const std::unordered_map<int, T>& valueMap
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
   std::unordered_map<wxString, int> mColormapIndexMap;
   std::unordered_map<int, int> mNoteNamingIndexMap;
   std::unordered_map<int, int> mTimeTickIndexMap;

   std::unordered_map<int, int> mNoiseFloorValueMap;
   std::unordered_map<int, int> mHighestNoteValueMap;
   std::unordered_map<int, wxString> mColormapValueMap;
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
