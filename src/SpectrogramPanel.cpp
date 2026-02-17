/**********************************************************************

  Audacity: A Digital Audio Editor

  SpectrogramPanel.cpp

  Guilherme Pavelski

*******************************************************************//**

\class SpectrogramPanel
\brief Responsible for the steps of computing an STFT.

wxPanel responsible for rendering a spectrogram bitmap, handling
user interaction (pan, zoom, reset), and drawing auxiliary overlays
such as note lines and time ticks.

*//*******************************************************************/

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

//----------------------------------------------------------------------
// Constructs the spectrogram panel and initializes rendering defaults
//----------------------------------------------------------------------
SpectrogramPanel::SpectrogramPanel(wxWindow* parent)
   : wxPanel(parent), m_showNoteLines(false)
{
   SetBackgroundStyle(wxBG_STYLE_PAINT);
   SetDoubleBuffered(true);
   m_cmap = MakeJetColormap();
}

void SpectrogramPanel::SetColormap(SpectrogramPanel::ColormapType type)
{
   if (m_colormap == type)
      return;

   m_colormap = type;
   m_cmap = BuildColormap(type);

   if (!m_matrix.empty())
      BuildBitmap();

   Refresh();
}

//----------------------------------------------------------------------
// Generates a 256-entry Jet-style colormap for value-to-color mapping
//----------------------------------------------------------------------
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

std::vector<wxColour> SpectrogramPanel::MakeGrayColormap()
{
   std::vector<wxColour> cmap(256);
   for (int i = 0; i < 256; ++i)
      cmap[i] = wxColour(i, i, i);
   return cmap;
}

std::vector<wxColour> SpectrogramPanel::MakeViridisColormap()
{
   static const unsigned char viridis[256][3] = {
      {68,1,84}, {68,2,85}, {69,4,87}, /* ... truncated ... */ {253,231,37}
   };

   std::vector<wxColour> cmap(256);
   for (int i = 0; i < 256; ++i)
      cmap[i] = wxColour(viridis[i][0], viridis[i][1], viridis[i][2]);

   return cmap;
}


std::vector<wxColour> SpectrogramPanel::MakeHotColormap()
{
   std::vector<wxColour> cmap(256);

   for (int i = 0; i < 256; ++i) {
      double x = i / 255.0;

      double r = std::min(1.0, 3 * x);
      double g = std::clamp(3 * x - 1.0, 0.0, 1.0);
      double b = std::clamp(3 * x - 2.0, 0.0, 1.0);

      cmap[i] = wxColour(r * 255, g * 255, b * 255);
   }

   return cmap;
}

std::vector<wxColour> SpectrogramPanel::MakeInfernoColormap()
{
   const std::vector<wxColour> anchors = {
      wxColour(0,   0,   4),
      wxColour(31, 12, 72),
      wxColour(85, 15,109),
      wxColour(136,34,106),
      wxColour(186,54, 85),
      wxColour(227,89, 51),
      wxColour(249,140,10),
      wxColour(252,195,65),
      wxColour(252,255,164)
   };

   return BuildColormap(anchors);
}

std::vector<wxColour> SpectrogramPanel::MakeMagmaColormap()
{
   const std::vector<wxColour> anchors = {
      wxColour(0,   0,   4),
      wxColour(28, 16, 68),
      wxColour(79, 18,123),
      wxColour(129,37,129),
      wxColour(181,54,122),
      wxColour(229,80,100),
      wxColour(251,135,97),
      wxColour(254,194,135),
      wxColour(252,253,191)
   };

   return BuildColormap(anchors);
}

std::vector<wxColour> SpectrogramPanel::MakeCividisColormap()
{
   const std::vector<wxColour> anchors = {
      wxColour(0,  32, 76),
      wxColour(0,  55, 90),
      wxColour(39, 77, 95),
      wxColour(73, 99, 98),
      wxColour(109,122,100),
      wxColour(146,145,99),
      wxColour(184,168,94),
      wxColour(221,192,85),
      wxColour(255,221,70)
   };

   return BuildColormap(anchors);
}

std::vector<wxColour> SpectrogramPanel::MakeParulaColormap()
{
   const std::vector<wxColour> anchors = {
      wxColour(53, 42,135),
      wxColour(15, 92,221),
      wxColour(18,125,216),
      wxColour(7, 156,207),
      wxColour(21,177,180),
      wxColour(89,189,140),
      wxColour(165,190,107),
      wxColour(225,185,82),
      wxColour(252,206,46),
      wxColour(249,251,14)
   };

   return BuildColormap(anchors);
}

wxColour SpectrogramPanel::Lerp(const wxColour& a,
   const wxColour& b,
   double t)
{
   return wxColour(
      static_cast<unsigned char>(a.Red() + t * (b.Red() - a.Red())),
      static_cast<unsigned char>(a.Green() + t * (b.Green() - a.Green())),
      static_cast<unsigned char>(a.Blue() + t * (b.Blue() - a.Blue()))
   );
}

std::vector<wxColour> SpectrogramPanel::BuildColormap(
   const std::vector<wxColour>& anchors,
   size_t resolution)
{
   std::vector<wxColour> cmap;
   cmap.reserve(resolution);

   const size_t segments = anchors.size() - 1;

   for (size_t i = 0; i < resolution; ++i)
   {
      double x = static_cast<double>(i) / (resolution - 1);
      double scaled = x * segments;

      size_t idx = static_cast<size_t>(scaled);
      double t = scaled - idx;

      if (idx >= segments)
      {
         cmap.push_back(anchors.back());
      }
      else
      {
         cmap.push_back(Lerp(anchors[idx], anchors[idx + 1], t));
      }
   }

   return cmap;
}

//----------------------------------------------------------------------
// Sets the spectrogram data matrix and associated metadata, then
// rebuilds the backing bitmap and resets the view extents
//----------------------------------------------------------------------
void SpectrogramPanel::SetData(const std::vector<std::vector<double>>& m,
       double maxFreq,
       size_t numSamples
)
{
   m_matrix = m;
   m_maxFreq = maxFreq;
   m_signalLength = numSamples;

   BuildBitmap();
   ResetView();
}

//----------------------------------------------------------------------
// Clears all spectrogram data and replaces it with a placeholder bitmap
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Handles paint events by rendering the current view to the panel
//----------------------------------------------------------------------
void SpectrogramPanel::OnPaint(wxPaintEvent&)
{
   wxAutoBufferedPaintDC dc(this);
   Render(dc, GetClientSize());
}

//----------------------------------------------------------------------
// Handles resize events by triggering a repaint when data is present
//----------------------------------------------------------------------
void SpectrogramPanel::OnSize(wxSizeEvent& event)
{
   if (!m_matrix.empty() && m_bitmap.IsOk()) {
      Refresh();
   }
   event.Skip();
}

//----------------------------------------------------------------------
// Renders horizontal musical note reference lines and labels
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Converts a frequency value (Hz) to a Y coordinate in widget space,
// accounting for the current vertical view range
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Renders the spectrogram view (including overlays) into the target DC
//----------------------------------------------------------------------
void SpectrogramPanel::Render(wxDC& dc, const wxSize& target) const
{
   dc.SetBackground(*wxBLACK_BRUSH);
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

   if (m_showTimeTicks)
      DrawTimeTicks(dc, target);
}

//----------------------------------------------------------------------
// Renders the currently visible spectrogram region into a bitmap,
// used primarily for exporting the view as an image
//----------------------------------------------------------------------
wxBitmap SpectrogramPanel::RenderCurrentViewToBitmap() const
{
   wxSize size = GetClientSize();
   wxBitmap bmp(size.GetWidth(), size.GetHeight(), 24);

   wxMemoryDC dc(bmp);
   Render(dc, size);
   dc.SelectObject(wxNullBitmap);

   return bmp;
}

//----------------------------------------------------------------------
// Builds the full-resolution backing bitmap from the spectrogram matrix,
// performing value normalization and colormap mapping
//----------------------------------------------------------------------
void SpectrogramPanel::BuildBitmap()
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

         const wxColour& c = m_cmap[idx];
         int offset = 3 * (y * cols + x);
         data[offset + 0] = c.Red();
         data[offset + 1] = c.Green();
         data[offset + 2] = c.Blue();
      }
   }

   m_bitmap = wxBitmap(img);
}

//----------------------------------------------------------------------
// Handles mouse wheel input to perform zooming centered at the cursor
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Handles mouse press, release, and drag events to support panning
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Clamps the current view extents to valid spectrogram bounds and
// enforces minimum visible ranges
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Resets the view extents to show the entire spectrogram
//----------------------------------------------------------------------
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

//----------------------------------------------------------------------
// Handles right-click events by resetting the spectrogram view
//----------------------------------------------------------------------
void SpectrogramPanel::OnRightClick(wxMouseEvent& event)
{
   ResetView();
   event.Skip();
}

//----------------------------------------------------------------------
// Draws time tick labels along the bottom edge of the panel, selecting
// an appropriate tick spacing based on zoom level
//----------------------------------------------------------------------
void SpectrogramPanel::DrawTimeTicks(wxDC& dc, const wxSize& size) const
{
   if (!m_showTimeTicks || m_signalLength == 0 || m_matrix.empty())
      return;

   dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
   dc.SetTextForeground(*wxWHITE);

   const double totalDuration = static_cast<double>(m_signalLength) / (2 * m_maxFreq);

   const double viewLeftTime = (m_viewLeftFrame / m_matrix[0].size()) * totalDuration;
   const double viewRightTime = (m_viewRightFrame / m_matrix[0].size()) * totalDuration;

   const double viewWidthSec = viewRightTime - viewLeftTime;

   // --- Compute a nice tick interval ---
   static const double tickSteps[] = {0.05, 0.1, 0.5, 1, 2, 5, 10, 20, 30, 60, 120, 300, 600, 1800 }; // in seconds
   double targetPixelsPerTick = 80.0; // aim for ~80 px between ticks
   double secondsPerPixel = viewWidthSec / size.GetWidth();
   double bestTick = tickSteps[0];

   for (double t : tickSteps)
      if (t / secondsPerPixel >= targetPixelsPerTick) {
         bestTick = t;
         break;
      }

   // Compute first tick >= viewLeftTime
   double tick = std::ceil(viewLeftTime / bestTick) * bestTick;

   while (tick <= viewRightTime) {
      // Convert time to pixel x
      double fx = (tick - viewLeftTime) / viewWidthSec * size.GetWidth();

      // Draw label only (no line)
      wxString label;
      if (tick < 60) {
         label = wxString::Format("%g", tick); // %g automatically trims
      }
      else {
         label.Printf("%.0f:%02.0f", std::floor(tick / 60.0), std::fmod(tick, 60.0));
      }

      wxCoord tw, th;
      dc.GetTextExtent(label, &tw, &th);

      int x = static_cast<int>(fx - tw / 2);
      int y = size.GetHeight() - th - 2;

      // Draw black background for readability
      dc.SetBrush(*wxBLACK_BRUSH);
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(x - 2, y - 1, tw + 4, th + 2);

      // Draw text
      dc.SetPen(*wxWHITE_PEN);
      dc.DrawText(label, x, y);

      tick += bestTick;
   }
}
