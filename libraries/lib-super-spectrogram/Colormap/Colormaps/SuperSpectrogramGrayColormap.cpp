/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramGrayColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   // ID and Name for the template
   constexpr wxChar idGray[] = L"gray";
   constexpr wxChar nameGray[] = L"Gray";

   std::vector<wxColour> GrayColormap()
   {
      std::vector<wxColour> cmap(256);
      for (int i = 0; i < 256; ++i)
         cmap[i] = wxColour(i, i, i);
      return cmap;
   }

   using Gray = LUTColormapGenerator<idGray, nameGray, GrayColormap>;

   // Automatically register the colormap
   ColormapRegistrar<idGray, nameGray, GrayColormap> sGrayRegistrar;
}
