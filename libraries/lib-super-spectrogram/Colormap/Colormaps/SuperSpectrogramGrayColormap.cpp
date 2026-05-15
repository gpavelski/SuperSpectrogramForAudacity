/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramGrayColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class GrayColormap final : public LUTColormap
   {
   public:
      GrayColormap()
         :
         LUTColormap(
            "gray",
            "Gray",
            ColormapUtils::MakeGray())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "gray",
      "Gray",
      []()
      {
         return std::make_unique<GrayColormap>();
      });
}