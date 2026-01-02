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

   void SetMatrix(const std::vector<std::vector<double>>& matrix,
      double maxFreq
   );
   void ResetView();
   void UpdatePrefs();

   size_t GetColumnCount() const
   {
      if (m_matrix.empty()) return 0;
      return m_matrix[0].size();
   }

   // Note display functionality
   void EnableNoteLines(bool enable = true) { m_showNoteLines = enable; Refresh(); }
   void SetNoteFrequencyRange(double minFreq, double maxFreq);
   wxBitmap RenderCurrentViewToBitmap() const;
   void Render(wxDC& dc, const wxSize& target) const;
   void Clear();

private:
   // Event handlers
   void OnPaint(wxPaintEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMouse(wxMouseEvent& event);
   void OnRightClick(wxMouseEvent& event);
   void OnWheel(wxMouseEvent& event);
   void OnEraseBackground(wxEraseEvent& event) { /* Prevent flicker */ }
   void DrawNoteLines(wxDC& dc, const wxSize& targetSize) const;
   void ClampViewRanges();
   double FreqToWidgetY(double freq, int widgetHeight) const;
   int valueToColorIndex(double v) const;

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

   size_t m_maxFreq;

   // Note display
   bool m_showNoteLines;

   // Static note data matching original Qt version
   static const std::vector<wxString> s_noteLabels;
   static const std::vector<double> s_noteFrequencies;

   // Display frequency range (should match your matrix data)
   double m_minDisplayFreq;
   double m_maxDisplayFreq;

   double m_zoom;

   // Color map
   static std::vector<wxColour> MakeJetColormap();
   static const std::vector<wxColour> s_jet;

   // These replace m_zoom + m_offsetY for frequency zoom
   double m_viewTopBin = 0;        // highest frequency bin in view
   double m_viewBottomBin = 512;   // lowest freq bin in view (rows)

   // Horizontal (time) axis
   double m_viewLeftFrame = 0;
   double m_viewRightFrame = 1000;

   double m_minValue;
   double m_maxValue;

   DECLARE_EVENT_TABLE()
};
