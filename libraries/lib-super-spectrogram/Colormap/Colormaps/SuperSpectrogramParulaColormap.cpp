/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramParulaColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   // ID and Name for the template
   constexpr wxChar idParula[] = L"parula";
   constexpr wxChar nameParula[] = L"Parula";

   // Anchor definition
   std::vector<wxColour> ParulaAnchors()
   {
      return {
         wxColour(53, 42,135),
         wxColour(15, 92,221),
         wxColour(18,125,216),
         wxColour(7, 156,207),
         wxColour(21,177,180),
         wxColour(89,189,140),
         wxColour(165,190,107),
         wxColour(225,185,82),
         wxColour(252,206,46),
         wxColour(249,251,14)
      };
   }

   std::vector<wxColour> ParulaColormap()
   {
      return ColormapUtils::BuildFromAnchors(ParulaAnchors());
   }

   using Parula = LUTColormapGenerator<idParula, nameParula, ParulaColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idParula, nameParula, ParulaColormap> sParulaRegistrar;
}
