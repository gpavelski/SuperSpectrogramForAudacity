/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramInfernoColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   // ID and Name for the template
   constexpr wxChar idInferno[] = L"inferno";
   constexpr wxChar nameInferno[] = L"Inferno";

   // Anchor definition
   std::vector<wxColour> InfernoAnchors()
   {
      return {
         wxColour(0,   0,   4),
         wxColour(31, 12, 72),
         wxColour(85, 15,109),
         wxColour(136,34,106),
         wxColour(186,54, 85),
         wxColour(227,89, 51),
         wxColour(249,140,10),
         wxColour(252,195,65),
         wxColour(252,255,164)
      };
   }

   std::vector<wxColour> InfernoColormap()
   {
      return ColormapUtils::BuildFromAnchors(InfernoAnchors());
   }

   using Inferno = LUTColormapGenerator<idInferno, nameInferno, InfernoColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idInferno, nameInferno, InfernoColormap> sJetRegistrar;
}
