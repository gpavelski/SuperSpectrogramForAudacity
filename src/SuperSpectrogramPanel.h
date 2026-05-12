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
#include "SuperSpectrogramConfig.h"
#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramConstants.h"
#include "SuperSpectrogramDataAdapter.h"
#include "SuperSpectrogramFrame.h"
#include "SuperSpectrogramViewport.h"

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

   // Note line display
   void EnableNoteLines(bool enable = true) { m_showNoteLines = enable; Refresh(); }
   void EnableTimeTicks(bool enable = true) { m_showTimeTicks = enable; Refresh(); }

   // Render
   wxBitmap RenderCurrentViewToBitmap() const;
   void Render(wxDC& dc, const wxSize& target) const;

   void SetColormap(SuperSpectrogramConfig::Colormap type);

   void SetNoteNamingStyle(SuperSpectrogramConfig::NoteNaming style);
   void SetShowNoteLines(bool show);

   void SetTimeTickMode(SuperSpectrogramConfig::TimeTickMode mode);
   SuperSpectrogramConfig::TimeTickMode GetTimeTickMode() const { return m_timeTickMode; }

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
   // Internal helpers
   // --------------------------
   void DrawNoteLines(wxDC& dc, const wxSize& targetSize) const;
   void DrawTimeTicks(wxDC& dc, const wxSize& targetSize) const;
   void DrawTickLabel(
      wxDC& dc,
      const wxSize& size,
      double fx,
      const wxString& label) const;

   double FreqToWidgetY(double freq, int widgetHeight) const;

   // --------------------------
   // Data
   // --------------------------
   wxPoint m_lastMouse;

   // Optional note lines overlay
   bool m_showNoteLines = true;

   // Min/max values in the current matrix
   double m_minValue = 0.0;
   double m_maxValue = 1.0;

   // Time tick data
   bool m_showTimeTicks = false;

   // Colormap
   std::unique_ptr<IColormap> m_colormap;

   SuperSpectrogramDataAdapter m_data;
   SuperSpectrogramViewport m_viewport;

   SuperSpectrogramConfig::TimeTickMode m_timeTickMode{ SuperSpectrogramConfig::TimeTickMode::Seconds };

   // --------------------------
   // Musical note reference
   // --------------------------
  std::vector<wxString> MakeNoteLabels(SuperSpectrogramConfig::NoteNaming style,
     int minNote,
     int maxNote);

  SuperSpectrogramConfig::NoteNaming mNoteNamingStyle{ SuperSpectrogramConfig::NoteNaming::Mixed };
  std::vector<wxString> mNoteLabels;

 // --------------------------
// Event table declaration
// --------------------------
   wxDECLARE_EVENT_TABLE();
};

#endif //__SUPER_SPECTROGRAM_PANEL__
