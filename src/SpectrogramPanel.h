/**********************************************************************

  Audacity: A Digital Audio Editor

  SpectrogramPanel.h

  Guilherme Pavelski

**********************************************************************/
#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <limits>

class SpectrogramPanel : public wxPanel
{
public:
   // --------------------------
   // Constructor
   // --------------------------
   explicit SpectrogramPanel(wxWindow* parent);

   // --------------------------
   // Public API
   // --------------------------
   void SetData(const std::vector<std::vector<double>>& matrix,
          double maxFreq,
          size_t numSamples);
   void ResetView();
   void Clear();

   size_t GetColumnCount() const {
      if (m_matrix.empty()) return 0;
      return m_matrix[0].size();
   }

   // Note line display
   void EnableNoteLines(bool enable = true) { m_showNoteLines = enable; Refresh(); }
   void EnableTimeTicks(bool enable = true) { m_showTimeTicks = enable; Refresh(); }

   // Render
   wxBitmap RenderCurrentViewToBitmap() const;
   void Render(wxDC& dc, const wxSize& target) const;

   enum class ColormapType
   {
      Jet,
      Gray,
      Hot,
      Viridis,
      Inferno,
      Magma,
      Cividis,
      Parula
   };

   enum class NoteNamingStyle
   {
      Sharps,
      Flats,
      Mixed
   };

   void SetColormap(ColormapType type);

   static wxColour Lerp(const wxColour& a,
      const wxColour& b,
      double t
   );

   static std::vector<wxColour> BuildColormap(
      const std::vector<wxColour>& anchors,
      size_t resolution = 256
   );

   void SetNoteNamingStyle(NoteNamingStyle style);

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
   void ClampViewRanges();
   double FreqToWidgetY(double freq, int widgetHeight) const;
   void BuildBitmap();

   static std::vector<wxColour> MakeJetColormap();
   static std::vector<wxColour> MakeGrayColormap();
   static std::vector<wxColour> MakeHotColormap();
   static std::vector<wxColour> MakeViridisColormap();
   static std::vector<wxColour> MakeInfernoColormap();
   static std::vector<wxColour> MakeMagmaColormap();
   static std::vector<wxColour> MakeCividisColormap();
   static std::vector<wxColour> MakeParulaColormap();

   // --------------------------
   // Data
   // --------------------------
   std::vector<std::vector<double>> m_matrix;
   wxBitmap m_bitmap;
   wxPoint m_lastMouse;

   double m_maxFreq = 0.0;

   // Viewport in matrix coordinates
   double m_viewTopBin = 0.0;
   double m_viewBottomBin = 512.0;
   double m_viewLeftFrame = 0.0;
   double m_viewRightFrame = 1000.0;

   // Optional note lines overlay
   bool m_showNoteLines = false;

   // Precomputed color map
   const std::vector<wxColour> s_jet = MakeJetColormap();

   // Min/max values in the current matrix
   double m_minValue = 0.0;
   double m_maxValue = 1.0;

   // Time tick data
   bool m_showTimeTicks = false;
   size_t m_signalLength = 0;       // length of the resampled signal

   // Colormap
   ColormapType m_colormap = ColormapType::Jet;
   std::vector<wxColour> m_cmap;

   std::vector<wxColour> BuildColormap(ColormapType type)
   {
      switch (type) {
         case ColormapType::Jet:     return MakeJetColormap();
         case ColormapType::Gray:    return MakeGrayColormap();
         case ColormapType::Hot:     return MakeHotColormap();
         case ColormapType::Viridis: return MakeViridisColormap();
         case ColormapType::Inferno: return MakeInfernoColormap();
         case ColormapType::Magma: return MakeMagmaColormap();
         case ColormapType::Cividis: return MakeCividisColormap();
         case ColormapType::Parula: return MakeParulaColormap();
      }
      return MakeJetColormap();
   }

   // --------------------------
   // Musical note reference
   // --------------------------

  const std::array<const char*, 12> kSharpNames = {
      "C", "C#", "D", "D#", "E", "F",
      "F#", "G", "G#", "A", "A#", "B"
   };

   const std::array<const char*, 12> kFlatNames = {
         "C", "Db", "D", "Eb", "E", "F",
         "Gb", "G", "Ab", "A", "Bb", "B"
   };

  const std::array<const char*, 12> kMixedNames = {
         "C", "C#", "D", "Eb", "E", "F",
         "F#", "G", "Ab", "A", "Bb", "B"
   };

  std::vector<wxString> MakeNoteLabels(NoteNamingStyle style,
     int minNote,
     int maxNote);

  NoteNamingStyle mNoteNamingStyle{ NoteNamingStyle::Mixed };
  std::vector<wxString> mNoteLabels;

  int mMinNote = 0;
  int mMaxNote = 108;

   const std::vector<double> s_noteFrequencies = {
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

 // --------------------------
// Event table declaration
// --------------------------
   wxDECLARE_EVENT_TABLE();
};
