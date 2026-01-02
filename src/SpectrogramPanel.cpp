#include "SpectrogramPanel.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <algorithm>
#include <cmath>

const std::vector<wxString> SpectrogramPanel::s_noteLabels = {
    "sil", "C0", "C#0", "D0", "Eb0", "E0", "F0", "F#0",
    "G0", "Ab0", "A0", "Bb0", "B0", "C1", "C#1", "D1", "Eb1", "E1", "F1", "F#1",
    "G1", "Ab1", "A1", "Bb1", "B1", "C2", "C#2", "D2", "Eb2", "E2", "F2", "F#2",
    "G2", "Ab2", "A2", "Bb2", "B2", "C3", "C#3", "D3", "Eb3", "E3", "F3", "F#3",
    "G3", "Ab3", "A3", "Bb3", "B3", "C4", "C#4", "D4", "Eb4", "E4", "F4", "F#4",
    "G4", "Ab4", "A4", "Bb4", "B4", "C5", "C#5", "D5", "Eb5", "E5", "F5", "F#5",
    "G5", "Ab5", "A5", "Bb5", "B5", "C6", "C#6", "D6", "Eb6", "E6", "F6", "F#6",
    "G6", "Ab6", "A6", "Bb6", "B6", "C7", "C#7", "D7", "Eb7", "E7", "F7", "F#7",
    "G7", "Ab7", "A7", "Bb7", "B7", "C8", "C#8", "D8", "Eb8", "E8", "F8", "F#8",
    "G8", "Ab8", "A8", "Bb8", "B8"
};

const std::vector<double> SpectrogramPanel::s_noteFrequencies = {
      0.0,
      16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12,
      24.50, 25.96, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25,
      49.00, 51.91, 55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50,
      98.00, 103.83, 110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00,
      196.00, 207.65, 220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99,
      392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99,
      783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98,
      1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96,
      3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91,
      6271.93, 6644.88, 7040.00, 7458.62, 7902.13
};

const std::vector<wxColour> SpectrogramPanel::s_jet = SpectrogramPanel::MakeJetColormap();

wxBEGIN_EVENT_TABLE(SpectrogramPanel, wxPanel)
EVT_PAINT(SpectrogramPanel::OnPaint)
EVT_SIZE(SpectrogramPanel::OnSize)
EVT_LEFT_DOWN(SpectrogramPanel::OnMouse)
EVT_LEFT_UP(SpectrogramPanel::OnMouse)
EVT_MOTION(SpectrogramPanel::OnMouse)
EVT_RIGHT_DOWN(SpectrogramPanel::OnRightClick)
EVT_MOUSEWHEEL(SpectrogramPanel::OnWheel)
EVT_ERASE_BACKGROUND(SpectrogramPanel::OnEraseBackground)
wxEND_EVENT_TABLE()

// Update the constructor to use the new static data:
SpectrogramPanel::SpectrogramPanel(wxWindow* parent)
   : wxPanel(parent), m_dirty(true), m_showNoteLines(false),
   m_minDisplayFreq(0.0), m_maxDisplayFreq(8000.0),
   m_zoom(1.0)
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


void SpectrogramPanel::SetNoteFrequencyRange(double minFreq, double maxFreq)
{
   m_minDisplayFreq = minFreq;
   m_maxDisplayFreq = maxFreq;
   Refresh();
}

void SpectrogramPanel::SetMatrix(const std::vector<std::vector<double>>& m, double maxFreq)
{
   m_matrix = m;
   m_maxFreq = maxFreq;

   // compute min/max once
   m_minValue = +std::numeric_limits<double>::infinity();
   m_maxValue = -std::numeric_limits<double>::infinity();
   for (const auto& row : m_matrix)
      for (double v : row)
         if (std::isfinite(v)) {
            m_minValue = std::min(m_minValue, v);
            m_maxValue = std::max(m_maxValue, v);
         }

   if (!std::isfinite(m_minValue) || m_minValue == m_maxValue) {
      m_minValue = 0.0;
      m_maxValue = 1.0;
   }

   RebuildBitmap();  // new function, see below
   ResetView();
}



void SpectrogramPanel::Clear()
{
   // Clear the matrix
   m_matrix.clear();
   m_matrix.shrink_to_fit();

   // Reset view state
   m_zoom = 1.0;

   // Create empty bitmap
   m_bitmap = wxBitmap(100, 100);
   wxMemoryDC memDC(m_bitmap);
   memDC.SetBackground(*wxWHITE_BRUSH);
   memDC.Clear();
   memDC.SetTextForeground(*wxBLACK);
   memDC.DrawText("No Data", 30, 45);

   m_dirty = false;
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
         FitImageToWidget();
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
   Refresh(false);   // false = erase background handled manually
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
   if (!m_bitmap.IsOk())
      return -1;

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



void SpectrogramPanel::FitImageToWidget()
{
   if (m_matrix.empty() || m_matrix[0].empty())
      return;

   if (!m_bitmap.IsOk())
      return;

   const wxSize widgetSize = GetClientSize();
   const wxSize imageSize = m_bitmap.GetSize();

   if (widgetSize.GetHeight() <= 0 || imageSize.GetHeight() <= 0)
      return;

   m_zoom =
      static_cast<double>(widgetSize.GetHeight()) /
      imageSize.GetHeight();

   Refresh();
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

   size_t rows = m_matrix.size();
   size_t cols = m_matrix[0].size();

   // Ensure we have valid dimensions
   if (rows == 0 || cols == 0) {
      return;
   }

   wxImage img(static_cast<int>(cols), static_cast<int>(rows), false);
   img.InitAlpha();

   double minv = std::numeric_limits<double>::infinity();
   double maxv = -std::numeric_limits<double>::infinity();

   // Find min/max values
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

   // Fill image data
   unsigned char* data = img.GetData();
   unsigned char* alpha = img.GetAlpha();

   for (int y = 0; y < static_cast<int>(rows); ++y) {
      for (int x = 0; x < static_cast<int>(cols); ++x) {
         double v = m_matrix[y][x];
         int idx;

         if (!std::isfinite(v)) {
            idx = 0;
            if (alpha) alpha[y * cols + x] = 128; // Semi-transparent for invalid
         }
         else {
            idx = static_cast<int>(255 * (v - minv) / (maxv - minv + 1e-9));
            idx = std::clamp(idx, 0, 255);
            if (alpha) alpha[y * cols + x] = 255; // Fully opaque
         }

         wxColour c = s_jet[idx];
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

int SpectrogramPanel::valueToColorIndex(double v) const
{
   if (!std::isfinite(v)) return 0;
   const double t = (v - m_minValue) / (m_maxValue - m_minValue + 1e-12);
   return std::clamp((int)(t * 255.0), 0, 255);
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

void SpectrogramPanel::UpdatePrefs()
{
   m_dirty = true;
   Refresh();
}
