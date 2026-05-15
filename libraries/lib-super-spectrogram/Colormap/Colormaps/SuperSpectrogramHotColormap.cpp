/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramHotColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class HotColormap final : public LUTColormap
   {
   public:
      HotColormap()
         :
         LUTColormap(
            "hot",
            "Hot",
            ColormapUtils::MakeHot())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "hot",
      "Hot",
      []()
      {
         return std::make_unique<HotColormap>();
      });
}