/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderer.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

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
