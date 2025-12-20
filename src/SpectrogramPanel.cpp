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
EVT_MOUSEWHEEL(SpectrogramPanel::OnWheel)
EVT_ERASE_BACKGROUND(SpectrogramPanel::OnEraseBackground)
wxEND_EVENT_TABLE()

// Update the constructor to use the new static data:
SpectrogramPanel::SpectrogramPanel(wxWindow* parent)
   : wxPanel(parent), m_dirty(true), m_showNoteLines(false),
   m_minDisplayFreq(0.0), m_maxDisplayFreq(8000.0),
   m_zoom(1.0), m_offsetX(0.0), m_offsetY(0.0) // Add these
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

void SpectrogramPanel::SetMatrix(const std::vector<std::vector<double>>& matrix)
{
   if (matrix.empty()) {
      // Clear everything when receiving empty matrix
      Clear();
   }
   else {
      m_matrix = matrix;
      m_dirty = true;

      Refresh();
   }
}

void SpectrogramPanel::Clear()
{
   // Clear the matrix
   m_matrix.clear();
   m_matrix.shrink_to_fit();

   // Reset view state
   m_zoom = 1.0;
   m_offsetX = 0.0;
   m_offsetY = 0.0;

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

void SpectrogramPanel::OnPaint(wxPaintEvent& event)
{
   wxAutoBufferedPaintDC dc(this);

   // Clear background
   wxColour customBackground(15, 15, 84);
   dc.SetBackground(wxBrush(customBackground));
   dc.Clear();

   if (!m_bitmap.IsOk()) {
      dc.DrawText("No spectrogram data available",
         GetSize().GetWidth() / 2 - 100,
         GetSize().GetHeight() / 2);
      return;
   }

   // Use wxGraphicsContext for GPU-accelerated transformations
   wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
   if (gc) {
      // Apply transformation (similar to Qt's QTransform)
      gc->Scale(m_zoom, m_zoom);
      gc->Translate(m_offsetX / m_zoom, m_offsetY / m_zoom);

      // Draw the bitmap with transformation applied by GPU
      gc->DrawBitmap(m_bitmap, 0, 0,
         m_bitmap.GetWidth(), m_bitmap.GetHeight());

      delete gc;
   }
   else {
      // Fallback to normal DC if graphics context not available
      dc.SetUserScale(m_zoom, m_zoom);
      dc.DrawBitmap(m_bitmap, m_offsetX / m_zoom, m_offsetY / m_zoom, false);
   }

   // Draw note lines (in widget coordinates, not transformed)
   if (m_showNoteLines && !m_matrix.empty()) {
      DrawNoteLines(dc);  // Extract note drawing to separate method
   }
}

void SpectrogramPanel::DrawNoteLines(wxDC& dc)
{
   if (!m_showNoteLines || m_matrix.empty()) return;

   double minFreq = s_noteFrequencies[0];
   double maxFreq = 2205.0;
   int imageHeight = static_cast<int>(m_matrix.size());

   wxSize widgetSize = GetSize();

   // Set up drawing
   dc.SetPen(wxPen(*wxWHITE, 1, wxPENSTYLE_SOLID)); // Thinner lines
   dc.SetTextForeground(*wxWHITE);
   wxFont font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
   dc.SetFont(font);

   // Separate tracking for labels vs lines
   std::vector<double> visibleLinePositions;
   std::vector<wxString> visibleLineLabels;

   // First pass: collect all visible lines
   for (size_t i = 0; i < s_noteFrequencies.size(); ++i) {
      double freq = s_noteFrequencies[i];
      if (freq < minFreq || freq > maxFreq) continue;

      // Calculate position in image coordinates
      double norm = (freq - minFreq) / (maxFreq - minFreq);
      double imageY = norm * (imageHeight - 1);

      // Convert to widget coordinates
      double widgetY = m_offsetY + (imageY * m_zoom);

      // Check if line is visible
      if (widgetY >= 0 && widgetY <= widgetSize.GetHeight()) {
         visibleLinePositions.push_back(widgetY);
         visibleLineLabels.push_back(s_noteLabels[i]);
      }
   }

   // Draw all lines first
   for (double widgetY : visibleLinePositions) {
      dc.DrawLine(0, static_cast<int>(widgetY),
         widgetSize.GetWidth(), static_cast<int>(widgetY));
   }

   // Now draw labels with intelligent spacing
   double lastLabelY = -1000.0;
   for (size_t i = 0; i < visibleLinePositions.size(); ++i) {
      double widgetY = visibleLinePositions[i];
      wxString label = visibleLineLabels[i];

      // Get text dimensions
      wxCoord textWidth, textHeight;
      dc.GetTextExtent(label, &textWidth, &textHeight);

      // Check for overlap - more lenient spacing
      const double MIN_LABEL_SPACING = 8.0; // Reduced from 15

      if (widgetY > lastLabelY + MIN_LABEL_SPACING) {
         // Position label ABOVE the line
         double labelX = 5.0;
         double labelY = widgetY - textHeight - 2; // Above line

         // Ensure label doesn't go off screen
         if (labelY < 2.0) {
            labelY = widgetY + 4.0; // Below line if too close to top
         }

         // Check if this would overlap with next line's label
         if (i + 1 < visibleLinePositions.size()) {
            double nextLineY = visibleLinePositions[i + 1];
            if (labelY + textHeight + 2 > nextLineY) {
               // Skip this label to avoid overlap
               continue;
            }
         }

         // Draw background for readability
         dc.SetPen(*wxTRANSPARENT_PEN);
         dc.SetBrush(wxBrush(wxColour(0, 0, 0, 180)));
         dc.DrawRectangle(static_cast<int>(labelX - 2),
            static_cast<int>(labelY - 1),
            textWidth + 4,
            textHeight + 2);

         // Draw the label
         dc.DrawText(label, static_cast<int>(labelX), static_cast<int>(labelY));

         // Update last label Y position (use bottom of label, not line)
         lastLabelY = labelY + textHeight;
      }
   }
}

void SpectrogramPanel::DrawNoteLabel(wxDC& dc, const wxString& label, double widgetY, const wxSize& widgetSize)
{
   // Calculate text dimensions
   wxCoord textWidth, textHeight;
   dc.GetTextExtent(label, &textWidth, &textHeight);

   // Calculate X position: we want it to follow with panning
   // Base position is 5px from left edge, adjusted by horizontal panning
   double xPos = 5 - m_offsetX;

   // Clamp to keep label visible
   if (xPos < 5) xPos = 5; // Don't go too far left
   if (xPos + textWidth > widgetSize.GetWidth() - 5) {
      xPos = widgetSize.GetWidth() - textWidth - 5;
   }

   // Calculate Y position: above the line
   double yPos = widgetY - textHeight - 2;

   // Optional: Draw background for better readability
   dc.SetPen(*wxTRANSPARENT_PEN);
   dc.SetBrush(wxBrush(wxColour(0, 0, 0, 180))); // Semi-transparent black
   dc.DrawRectangle(xPos - 2, yPos - 1, textWidth + 4, textHeight + 2);

   // Draw the text
   dc.DrawText(label, xPos, yPos);
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
   if (!m_bitmap.IsOk()) {
      event.Skip();
      return;
   }

   const wxPoint mousePos = event.GetPosition();

   const double factor =
      (event.GetWheelRotation() > 0) ? 1.15 : (1.0 / 1.15);

   const double oldZoom = m_zoom;
   m_zoom *= factor;

   const wxSize widgetSize = GetClientSize();
   const wxSize imageSize = m_bitmap.GetSize();

   // ---- CORRECT MIN ZOOM (Y-anchored) ----
   const double minZoom =
      static_cast<double>(widgetSize.GetHeight()) /
      imageSize.GetHeight();

   const double maxZoom = 10.0;

   m_zoom = std::clamp(m_zoom, minZoom, maxZoom);

   // ---- Zoom around mouse position ----
   const double imageX = (mousePos.x - m_offsetX) / oldZoom;
   const double imageY = (mousePos.y - m_offsetY) / oldZoom;

   m_offsetX = mousePos.x - imageX * m_zoom;
   m_offsetY = mousePos.y - imageY * m_zoom;

   ClampOffsets();
   Refresh();
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
   else if (event.Dragging() && event.LeftIsDown()) {
      wxPoint pos = event.GetPosition();
      wxPoint delta = pos - m_lastMouse;
      m_lastMouse = pos;

      m_offsetX += delta.x;
      m_offsetY += delta.y;

      ClampOffsets();
      Refresh();
   }
}

void SpectrogramPanel::ClampOffsets()
{
   if (!m_bitmap.IsOk()) return;

   wxSize widgetSize = GetSize();
   double scaledWidth = GetScaledWidth();
   double scaledHeight = GetScaledHeight();

   // Qt-style clamping: if scaled image is smaller than widget, center it
   if (scaledWidth <= widgetSize.GetWidth()) {
      m_offsetX = (widgetSize.GetWidth() - scaledWidth) / 2.0;
   }
   else {
      // Image larger than widget - don't show empty space
      double maxOffsetX = 0;
      double minOffsetX = widgetSize.GetWidth() - scaledWidth;
      m_offsetX = std::max(minOffsetX, std::min(maxOffsetX, m_offsetX));
   }

   if (scaledHeight <= widgetSize.GetHeight()) {
      m_offsetY = (widgetSize.GetHeight() - scaledHeight) / 2.0;
   }
   else {
      double maxOffsetY = 0;
      double minOffsetY = widgetSize.GetHeight() - scaledHeight;
      m_offsetY = std::max(minOffsetY, std::min(maxOffsetY, m_offsetY));
   }
}

void SpectrogramPanel::FitImageToWidget()
{
   if (m_matrix.empty() || m_matrix[0].empty())
      return;

   if (m_dirty) {
      RebuildBitmap();
      m_dirty = false;
   }

   if (!m_bitmap.IsOk())
      return;

   const wxSize widgetSize = GetClientSize();
   const wxSize imageSize = m_bitmap.GetSize();

   if (widgetSize.GetHeight() <= 0 || imageSize.GetHeight() <= 0)
      return;

   // Y-anchored zoom: always show all frequency bins
   m_zoom =
      static_cast<double>(widgetSize.GetHeight()) /
      imageSize.GetHeight();

   // Left-align time axis
   m_offsetX = 0.0;
   m_offsetY = 0.0;

   ClampOffsets();
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

void SpectrogramPanel::ResetView()
{
   FitImageToWidget();
   Refresh();
}

void SpectrogramPanel::UpdatePrefs()
{
   m_dirty = true;
   Refresh();
}
