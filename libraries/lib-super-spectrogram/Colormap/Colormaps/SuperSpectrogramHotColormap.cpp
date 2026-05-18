/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramHotColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include <algorithm>

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   constexpr wxChar idHot[] = L"hot";
   constexpr wxChar nameHot[] = L"Hot";

   std::vector<wxColour> HotColormap()
   {
      std::vector<wxColour> cmap(256);

      for (int i = 0; i < 256; ++i) {
         double x = i / 255.0;

         double r = std::min(1.0, 3 * x);
         double g = std::clamp(3 * x - 1.0, 0.0, 1.0);
         double b = std::clamp(3 * x - 2.0, 0.0, 1.0);

         cmap[i] = wxColour(r * 255, g * 255, b * 255);
      }

      return cmap;
   }

   using Hot = LUTColormapGenerator<idHot, nameHot, HotColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idHot, nameHot, HotColormap> sHotRegistrar;
}
