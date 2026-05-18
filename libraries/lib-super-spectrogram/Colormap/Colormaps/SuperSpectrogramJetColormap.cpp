/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramJetColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include <algorithm>

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   constexpr wxChar idJet[] = L"jet";
   constexpr wxChar nameJet[] = L"Jet";

   std::vector<wxColour> JetColormap()
   {
      std::vector<wxColour> cmap(256);
      for (int i = 0; i < 256; ++i) {
         double x = i / 255.0;
         double r = std::clamp(1.5 - std::abs(4 * x - 3.0), 0.0, 1.0);
         double g = std::clamp(1.5 - std::abs(4 * x - 2.0), 0.0, 1.0);
         double b = std::clamp(1.5 - std::abs(4 * x - 1.0), 0.0, 1.0);

         cmap[i] = wxColour(
            static_cast<unsigned char>(r * 255),
            static_cast<unsigned char>(g * 255),
            static_cast<unsigned char>(b * 255)
         );
      }
      return cmap;
   }

   using Jet = LUTColormapGenerator<idJet, nameJet, JetColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idJet, nameJet, JetColormap> sJetRegistrar;
}
