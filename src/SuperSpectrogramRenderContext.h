/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderContext.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_RENDER_CONTEXT__
#define __SUPER_SPECTROGRAM_RENDER_CONTEXT__

#include <vector>
#include <cstddef>

// Forward declarations
class IColormap;
class SuperSpectrogramViewport;

struct SuperSpectrogramRenderContext
{
   const std::vector<float>& normalized;
   size_t rows;
   size_t cols;

   const IColormap& colormap;
   const SuperSpectrogramViewport& viewport;
};

#endif