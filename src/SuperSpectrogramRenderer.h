/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderer.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_RENDERER__
#define __SUPER_SPECTROGRAM_RENDERER__

#include <wx/image.h>
#include <vector>

class IColormap;

class SuperSpectrogramRenderer
{
public:
   wxBitmap Render(
      const std::vector<float>& normalized,
      size_t rows,
      size_t cols,
      const IColormap& colormap) const;
};

#endif