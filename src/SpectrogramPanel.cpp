#include "SpectrogramPanel.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <algorithm>
#include <cmath>

wxBEGIN_EVENT_TABLE(SpectrogramPanel, wxPanel)
EVT_PAINT(SpectrogramPanel::OnPaint)
EVT_SIZE(SpectrogramPanel::OnSize)
EVT_LEFT_DOWN(SpectrogramPanel::OnMouse)
EVT_LEFT_UP(SpectrogramPanel::OnMouse)
EVT_MOTION(SpectrogramPanel::OnMouse)
EVT_RIGHT_DOWN(SpectrogramPanel::OnRightClick)
EVT_MOUSEWHEEL(SpectrogramPanel::OnWheel)
wxEND_EVENT_TABLE()

// Update the constructor to use the new static data:
SpectrogramPanel::SpectrogramPanel(wxWindow* parent)
   : wxPanel(parent), m_showNoteLines(false)
{
   SetBackgroundStyle(wxBG_STYLE_PAINT);
   SetDoubleBuffered(true);
}

std::vector<wxColour> SpectrogramPanel::MakeJetColormap()
{
   std::vector<wxColour> cmap(256);
   for (int i = 0; i < 256; ++i) {
      double x = i / 255.0;
      double r = std::clamp(1.5 - std::abs(4 * x - 3.0), 0.0, 1.0);
      double g = std::clamp(1.5 - std::abs(4 * x - 2.0), 0.0, 1.0);
      double b = std::clamp(1.5 - std::abs(4 * x - 1.0), 0.0, 1.0);
      cmap[i] = wxColour(
         static_cast<unsigned char>(r * 255),
         static_cast<unsigned char>(g * 255),
         static_cast<unsigned char>(b * 255)
      );
   }
   return cmap;
}

void SpectrogramPanel::SetMatrix(const std::vector<std::vector<double>>& m, double maxFreq)
{
   m_matrix = m;
   m_maxFreq = maxFreq;

   RebuildBitmap();  // min/max computed inside RebuildBitmap
   ResetView();
}

void SpectrogramPanel::Clear()
{
   // Clear the matrix
   m_matrix.clear();

   // Create empty bitmap
   m_bitmap = wxBitmap(100, 100);
   wxMemoryDC memDC(m_bitmap);
   memDC.SetBackground(*wxWHITE_BRUSH);
   memDC.Clear();
   memDC.SetTextForeground(*wxBLACK);
   memDC.DrawText("No Data", 30, 45);

   Refresh();
}

void SpectrogramPanel::OnPaint(wxPaintEvent&)
{
   wxAutoBufferedPaintDC dc(this);
   Render(dc, GetClientSize());
}

void SpectrogramPanel::DrawNoteLines(wxDC& dc, const wxSize& size) const
{
   if (m_matrix.empty()) return;

   dc.SetPen(wxPen(*wxWHITE, 1));
   dc.SetTextForeground(*wxWHITE);
   dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

   double lastLabelY = -1e9;

   for (size_t i = 0; i < s_noteFrequencies.size(); ++i) {
      double freq = s_noteFrequencies[i];
      double y = FreqToWidgetY(freq, size.GetHeight());
      if (y < 0) continue;

      // Draw the horizontal line across full widget width
      dc.DrawLine(0, y, size.GetWidth(), y);

      // Compute label position
      const wxString& label = s_noteLabels[i];
      wxCoord tw, th;
      dc.GetTextExtent(label, &tw, &th);
      double labelY = y - th - 2;

      if (labelY < lastLabelY + th + 4)
         continue; // avoid overlaps

      // Draw black background for readability
      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(2, labelY - 1, tw + 6, th + 2);

      // Draw text
      dc.SetPen(*wxWHITE_PEN);
      dc.DrawText(label, 5, labelY);

      lastLabelY = labelY;
   }
}

void SpectrogramPanel::OnSize(wxSizeEvent& event)
{
   if (!m_matrix.empty() && m_bitmap.IsOk()) {
         Refresh();
   }
   event.Skip();
}

void SpectrogramPanel::OnWheel(wxMouseEvent& event)
{
   if (m_matrix.empty()) return;

   double factor = (event.GetWheelRotation() > 0) ? 0.8 : 1.25; // 20% zoom

   int mouseY = event.GetY();
   int mouseX = event.GetX();
   wxSize size = GetClientSize();

   // Mouse percentage in window
   double fy = (double)mouseY / size.GetHeight();
   double fx = (double)mouseX / size.GetWidth();

   // Current ranges
   double height = m_viewBottomBin - m_viewTopBin;
   double width = m_viewRightFrame - m_viewLeftFrame;

   // New ranges
   double newHeight = height * factor;
   double newWidth = width * factor;

   // Anchor at mouse position
   m_viewTopBin += (height - newHeight) * fy;
   m_viewBottomBin = m_viewTopBin + newHeight;

   m_viewLeftFrame += (width - newWidth) * fx;
   m_viewRightFrame = m_viewLeftFrame + newWidth;

   ClampViewRanges();
   Refresh();
}

void SpectrogramPanel::ClampViewRanges()
{
   if (m_matrix.empty()) return;

   const int totalBins = (int)m_matrix.size();
   const int totalFrames = (int)m_matrix[0].size();

   // Vertical clamp (frequency bins)
   if (m_viewTopBin < 0) m_viewTopBin = 0;
   if (m_viewBottomBin > totalBins) m_viewBottomBin = totalBins;
   if (m_viewBottomBin - m_viewTopBin < 2) {
      m_viewBottomBin = m_viewTopBin + 2; // minimum 2 bins
   }

   // Horizontal clamp (time frames)
   if (m_viewLeftFrame < 0) m_viewLeftFrame = 0;
   if (m_viewRightFrame > totalFrames) m_viewRightFrame = totalFrames;
   if (m_viewRightFrame - m_viewLeftFrame < 2) {
      m_viewRightFrame = m_viewLeftFrame + 2; // minimum 2 frames
   }
}

void SpectrogramPanel::OnMouse(wxMouseEvent& event)
{
   if (!m_bitmap.IsOk()) {
      event.Skip();
      return;
   }

   if (event.LeftDown()) {
      m_lastMouse = event.GetPosition();
      CaptureMouse();
   }
   else if (event.LeftUp()) {
      if (HasCapture()) ReleaseMouse();
   }
      else if (event.Dragging() && event.LeftIsDown())
      {
         wxPoint pos = event.GetPosition();
         wxPoint delta = pos - m_lastMouse;
         m_lastMouse = pos;

         wxSize size = GetClientSize();

         // Current ranges
         double height = m_viewBottomBin - m_viewTopBin;
         double width = m_viewRightFrame - m_viewLeftFrame;

         // Convert pixel delta to data delta
         double dx = (double)delta.x / size.GetWidth() * width;
         double dy = (double)delta.y / size.GetHeight() * height;

         m_viewLeftFrame -= dx;
         m_viewRightFrame -= dx;

         m_viewTopBin -= dy;
         m_viewBottomBin -= dy;

         ClampViewRanges();
         Refresh();
      }
}

double SpectrogramPanel::FreqToWidgetY(double freq, int widgetHeight) const
{
   const int rows = m_bitmap.GetHeight();
   const double fNyq = m_maxFreq;

   if (freq < 0.0 || freq > fNyq)
      return -1;

   // Frequency -> bin center
   const double binIndex = (freq / fNyq) * rows + 0.5;

   // Clip to valid bin range
   if (binIndex < m_viewTopBin || binIndex > m_viewBottomBin)
      return -1;

   // Map visible bins -> widget Y
   const double binRel =
      (binIndex - m_viewTopBin) /
      (m_viewBottomBin - m_viewTopBin);

   return binRel * widgetHeight;
}

void SpectrogramPanel::RebuildBitmap()
{
   if (m_matrix.empty() || m_matrix[0].empty()) {
      // Create a placeholder bitmap
      m_bitmap = wxBitmap(100, 100);
      wxMemoryDC memDC(m_bitmap);
      memDC.SetBackground(*wxWHITE_BRUSH);
      memDC.Clear();
      memDC.SetTextForeground(*wxWHITE);
      memDC.DrawText("No Data", 30, 45);
      return;
   }

   const size_t rows = m_matrix.size();
   const size_t cols = m_matrix[0].size();

   wxImage img(static_cast<int>(cols), static_cast<int>(rows), false); // RGB only

   // Find min/max values
   double minv = std::numeric_limits<double>::infinity();
   double maxv = -std::numeric_limits<double>::infinity();

   for (const auto& row : m_matrix) {
      for (double v : row) {
         if (std::isfinite(v)) {
            minv = std::min(minv, v);
            maxv = std::max(maxv, v);
         }
      }
   }

   if (!std::isfinite(minv) || minv == maxv) {
      minv = 0.0;
      maxv = 1.0;
   }

   unsigned char* data = img.GetData();

   for (int y = 0; y < static_cast<int>(rows); ++y) {
      for (int x = 0; x < static_cast<int>(cols); ++x) {
         double v = m_matrix[y][x];
         int idx;

         if (!std::isfinite(v)) {
            idx = 0; // Use first colormap color for invalid values
         }
         else {
            idx = static_cast<int>(255 * (v - minv) / (maxv - minv + 1e-9));
            idx = std::clamp(idx, 0, 255);
         }

         const wxColour& c = s_jet[idx];
         int offset = 3 * (y * cols + x);
         data[offset + 0] = c.Red();
         data[offset + 1] = c.Green();
         data[offset + 2] = c.Blue();
      }
   }

   m_bitmap = wxBitmap(img);
}

wxBitmap SpectrogramPanel::RenderCurrentViewToBitmap() const
{
   wxSize size = GetClientSize();
   wxBitmap bmp(size.GetWidth(), size.GetHeight(), 24);

   wxMemoryDC dc(bmp);
   Render(dc, size);
   dc.SelectObject(wxNullBitmap);

   return bmp;
}

void SpectrogramPanel::Render(wxDC& dc, const wxSize& target) const
{
   // ---- CLEAR THE BACK BUFFER ----
   dc.SetBackground(*wxBLACK_BRUSH);   // or *wxWHITE_BRUSH if preferred
   dc.Clear();

   if (!m_bitmap.IsOk())
      return;

   const int srcX = (int)m_viewLeftFrame;
   const int srcY = (int)m_viewTopBin;
   const int srcW = (int)(m_viewRightFrame - m_viewLeftFrame);
   const int srcH = (int)(m_viewBottomBin - m_viewTopBin);

   wxBitmap sub = m_bitmap.GetSubBitmap(
      wxRect(srcX, srcY, std::max(1, srcW), std::max(1, srcH))
   );

   wxMemoryDC srcDC;
   srcDC.SelectObject(sub);

   dc.StretchBlit(
      0, 0,
      target.GetWidth(), target.GetHeight(),
      &srcDC,
      0, 0,
      srcW, srcH
   );

   srcDC.SelectObject(wxNullBitmap);

   if (m_showNoteLines)
      DrawNoteLines(dc, target);
}

void SpectrogramPanel::OnRightClick(wxMouseEvent& event)
{
   ResetView();
   event.Skip(); 
}

void SpectrogramPanel::ResetView()
{
   if (m_matrix.empty())
      return;

   const int totalBins = (int)m_matrix.size();
   const int totalFrames = (int)m_matrix[0].size();

   m_viewTopBin = 0.0;
   m_viewBottomBin = (double)totalBins;
   m_viewLeftFrame = 0.0;
   m_viewRightFrame = (double)totalFrames;

   Refresh(false);
}
