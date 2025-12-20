#ifndef __AUDACITY_SUPER_FREQ_WINDOW__
#define __AUDACITY_SUPER_FREQ_WINDOW__

#include "PlotSuperSpectrumBase.h"
#include <vector>
#include <memory>
#include <wx/font.h>
#include "wxPanelWrapper.h"  // for wxDialogWrapper

class AudacityProject;
class SpectrogramPanel;

//=================================================================
// SuperFrequencyPlotDialog: hosts the SpectrogramPanel
//=================================================================
class SuperFrequencyPlotDialog final :
   public PlotSuperSpectrumBase,   // used for spectrum data computation
   public wxDialogWrapper,
   public PrefsListener
{
public:
   // Constructor / Destructor
   SuperFrequencyPlotDialog(wxWindow* parent, wxWindowID id,
      AudacityProject& project,
      const TranslatableString& title,
      const wxPoint& pos = wxDefaultPosition);
   virtual ~SuperFrequencyPlotDialog();

   // Override Show() to display dialog
   bool Show(bool show = true) override;

   // Feed a 2D STFT / spectrogram matrix to the panel
   void PlotSTFTMatrix(const std::vector<std::vector<double>>& matrix);

   // Recalculate the spectrogram from the current selection
   void Recalc();

private:

   void ApplyDataDrivenMinSize();
   // Event handlers
   void OnCloseWindow(wxCloseEvent& event);

   // PrefsListener interface
   void UpdatePrefs() override;

private:
#ifdef __WXMSW__
   static const int fontSize = 8;
#else
   static const int fontSize = 10;
#endif

   // Font for optional overlays (timestamps, peak labels, etc.)
   wxFont mFreqFont;

   // The panel that draws the spectrogram
   std::unique_ptr<SpectrogramPanel> mSpectrogramPanel;

   // Current STFT / spectrogram data (optional cache)
   std::vector<std::vector<double>> mMatrix;

   DECLARE_EVENT_TABLE()
};

#endif // __AUDACITY_SUPER_FREQ_WINDOW__
