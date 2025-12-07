#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/geometry.h>
#include <vector>
#include <memory>
#include <limits>

class SpectrogramPanel : public wxPanel
{
public:
   SpectrogramPanel(wxWindow* parent);

   void SetMatrix(const std::vector<std::vector<double>>& matrix);
   void ResetView();
   void UpdatePrefs();

   // Note display functionality
   void EnableNoteLines(bool enable = true) { m_showNoteLines = enable; Refresh(); }
   void SetNoteFrequencyRange(double minFreq, double maxFreq);

private:
   // Event handlers
   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMouse(wxMouseEvent& event);
   void OnWheel(wxMouseEvent& event);
   void OnEraseBackground(wxEraseEvent& event) { /* Prevent flicker */ }
   void DrawNoteLabel(wxDC& dc, const wxString& label, double widgetY, const wxSize& widgetSize);
   void DrawNoteLines(wxDC& dc);
   void ClampOffsets();
   void Clear();

   // Helper methods
   void RebuildBitmap();
   void FitImageToWidget();

   double GetScaledWidth() const {
      return m_bitmap.IsOk() ? m_bitmap.GetWidth() * m_zoom : 0.0;
   }

   double GetScaledHeight() const {
      return m_bitmap.IsOk() ? m_bitmap.GetHeight() * m_zoom : 0.0;
   }

   wxSize GetScaledSize() const {
      return wxSize(static_cast<int>(GetScaledWidth()),
         static_cast<int>(GetScaledHeight()));
   }

   // Data
   std::vector<std::vector<double>> m_matrix;
   wxBitmap m_bitmap;
   bool m_dirty;

   // Mouse handling
   wxPoint m_lastMouse;

   // Note display
   bool m_showNoteLines;

   // Static note data matching original Qt version
   static const std::vector<wxString> s_noteLabels;
   static const std::vector<double> s_noteFrequencies;

   // Display frequency range (should match your matrix data)
   double m_minDisplayFreq;
   double m_maxDisplayFreq;

   double m_zoom;
   double m_offsetX;
   double m_offsetY;

   // Color map
   static std::vector<wxColour> MakeJetColormap();
   static const std::vector<wxColour> s_jet;

   DECLARE_EVENT_TABLE()
};
