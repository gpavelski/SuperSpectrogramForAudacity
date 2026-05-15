/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderer.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_RENDERER__
#define __SUPER_SPECTROGRAM_RENDERER__

#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/defs.h> 
#include <wx/colour.h>

#include "Colormap/SuperSpectrogramColormap.h"
#include "View/SuperSpectrogramRenderContext.h"
#include "View/SuperSpectrogramViewport.h"

class SuperSpectrogramRenderer
{
public:
   wxBitmap Render(
      const SuperSpectrogramRenderContext& ctx,
      const wxSize& targetSize
   ) const;
};

#endif