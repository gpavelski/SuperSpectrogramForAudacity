/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderContext.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_RENDER_CONTEXT__
#define __SUPER_SPECTROGRAM_RENDER_CONTEXT__

#include <vector>
#include <cstddef>

class IColormap;
class SuperSpectrogramViewport;

struct SuperSpectrogramRenderContext
{
   const std::vector<float>& normalized;
   size_t rows{ 0 };
   size_t cols{ 0 };

   const IColormap& colormap;
   const SuperSpectrogramViewport& viewport;
};

#endif
