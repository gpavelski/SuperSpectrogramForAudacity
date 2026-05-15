/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramViewport.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_VIEWPORT__
#define __SUPER_SPECTROGRAM_VIEWPORT__

#include <algorithm>
#include <wx/gdicmn.h>

class SuperSpectrogramViewport
{
public:
   // Initializes viewport to dataset bounds
   void SetBounds(size_t rows, size_t cols);

   // Resets viewport to full dataset view
   void Reset();

   // Zoom around a normalized pivot point [0..1]
   void Zoom(double factor, double fx, double fy);

   // Pan in screen-space pixels
   void Pan(double dxPixels, double dyPixels, const wxSize& widgetSize);

   // Ensures viewport remains valid within dataset bounds
   void Clamp();

   // --------------------------
   // Accessors (data-space coordinates)
   // --------------------------
   double Left()   const { return m_left; }
   double Right()  const { return m_right; }
   double Top()    const { return m_top; }
   double Bottom() const { return m_bottom; }

private:
   size_t m_rows{ 0 };
   size_t m_cols{ 0 };

   double m_left{ 0.0 };
   double m_right{ 1.0 };
   double m_top{ 0.0 };
   double m_bottom{ 1.0 };
};

#endif
