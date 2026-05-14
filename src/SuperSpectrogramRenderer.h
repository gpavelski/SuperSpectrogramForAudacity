/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderer.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_RENDERER__
#define __SUPER_SPECTROGRAM_RENDERER__

#include <wx/bitmap.h>
#include <wx/gdicmn.h>

#include "SuperSpectrogramRenderContext.h"

class SuperSpectrogramRenderer
{
public:
   wxBitmap Render(
      const SuperSpectrogramRenderContext& ctx,
      const wxSize& targetSize
   ) const;
};

#endif