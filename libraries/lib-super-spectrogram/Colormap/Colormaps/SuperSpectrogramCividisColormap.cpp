/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramCividisColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class CividisColormap final : public LUTColormap
   {
   public:
      CividisColormap()
         :
         LUTColormap(
            "cividis",
            "Cividis",
            ColormapUtils::MakeCividis())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "cividis",
      "Cividis",
      []()
      {
         return std::make_unique<CividisColormap>();
      });
}