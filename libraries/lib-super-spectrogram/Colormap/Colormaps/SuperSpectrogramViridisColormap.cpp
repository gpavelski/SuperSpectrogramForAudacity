/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramViridisColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   constexpr wxChar idViridis[] = L"viridis";
   constexpr wxChar nameViridis[] = L"Viridis";

   std::vector<wxColour> ViridisAnchors()
   {
      return {
          wxColour(68,  1,  84),
          wxColour(71, 44, 122),
          wxColour(59, 81, 139),
          wxColour(44,113,142),
          wxColour(33,144,141),
          wxColour(39,173,129),
          wxColour(92,200, 99),
          wxColour(170,220,50),
          wxColour(253,231,37)
      };
   }

   std::vector<wxColour> ViridisColormap()
   {
      return ColormapUtils::BuildFromAnchors(ViridisAnchors());
   }

   using Viridis = LUTColormapGenerator<idViridis, nameViridis, ViridisColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idViridis, nameViridis, ViridisColormap> sViridisRegistrar;
}
