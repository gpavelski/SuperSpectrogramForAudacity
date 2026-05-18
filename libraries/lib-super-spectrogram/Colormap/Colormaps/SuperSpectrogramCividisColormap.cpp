/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramCividisColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   // ID and Name for the template
   constexpr wxChar idCividis[] = L"cividis";
   constexpr wxChar nameCividis[] = L"Cividis";

   // Anchor definition
   std::vector<wxColour> CividisAnchors()
   {
      return {
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
   }

   // LUT generator from anchors
   std::vector<wxColour> CividisColormap()
   {
      return ColormapUtils::BuildFromAnchors(CividisAnchors());
   }

   // Instantiate colormap using template
   using Cividis = LUTColormapGenerator<idCividis, nameCividis, CividisColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idCividis, nameCividis, CividisColormap> sCividisRegistrar;
}
