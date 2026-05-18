/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramMagmaColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   // ID and Name for the template
   constexpr wxChar idMagma[] = L"magma";
   constexpr wxChar nameMagma[] = L"Magma";

   // Anchor definition
   std::vector<wxColour> MagmaAnchors()
   {
      return {
         wxColour(0,   0,   4),
         wxColour(28, 16, 68),
         wxColour(79, 18,123),
         wxColour(129,37,129),
         wxColour(181,54,122),
         wxColour(229,80,100),
         wxColour(251,135,97),
         wxColour(254,194,135),
         wxColour(252,253,191)
      };
   }

   std::vector<wxColour> MagmaColormap()
   {
      return ColormapUtils::BuildFromAnchors(MagmaAnchors());
   }

   using Magma = LUTColormapGenerator<idMagma, nameMagma, MagmaColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idMagma, nameMagma, MagmaColormap> sMagmaRegistrar;
}
