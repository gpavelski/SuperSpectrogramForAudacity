/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramPanel.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_PANEL__
#define __SUPER_SPECTROGRAM_PANEL__

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <limits>
#include <memory>
#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramConstants.h"
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramFrame.h"
#include "SuperSpectrogramNotesLinesOverlay.h"
#include "SuperSpectrogramViewport.h"
#include "SuperSpectrogramTimeTicksOverlay.h"


class ISpectrogramOverlay;

class SuperSpectrogramPanel : public wxPanel
{
public:
   // --------------------------
   // Constructor
   // --------------------------
   explicit SuperSpectrogramPanel(wxWindow* parent);

   // --------------------------
   // Public API
   // --------------------------
   void SetData(const SuperSpectrogramFrame& frame);
   void ResetView();
   void Clear();

   size_t GetColumnCount() const {
      if (m_data.GetNormalized().empty()) return 0;
      return m_data.Cols();
   }

   // Render
   wxBitmap RenderCurrentViewToBitmap() const;
   void Render(wxDC& dc, const wxSize& target) const;

   void SetColormap(SuperSpectrogramConfig::Colormap type);

   void SetNoteNamingStyle(
      SuperSpectrogramConfig::NoteNaming style
   );

   void SetShowNoteLines(bool show);

   void SetTimeTickMode(
      SuperSpectrogramConfig::TimeTickMode mode
   );

   SuperSpectrogramConfig::TimeTickMode GetTimeTickMode() const;

private:
   // --------------------------
   // Event handlers
   // --------------------------
   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMouse(wxMouseEvent& event);
   void OnRightClick(wxMouseEvent& event);
   void OnWheel(wxMouseEvent& event);

   // --------------------------
   // Data
   // --------------------------
   wxPoint m_lastMouse;

   // Colormap
   std::unique_ptr<IColormap> m_colormap;
   SuperSpectrogramDataAdapter m_data;
   std::vector<std::unique_ptr<ISpectrogramOverlay>> m_overlays;
   SuperSpectrogramViewport m_viewport;

   SuperSpectrogramNotesLinesOverlay* m_notesOverlay = nullptr;
   SuperSpectrogramTimeTicksOverlay* m_timeOverlay = nullptr;

 // --------------------------
// Event table declaration
// --------------------------
   wxDECLARE_EVENT_TABLE();
};

#endif //__SUPER_SPECTROGRAM_PANEL__
