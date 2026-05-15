/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramParulaColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class ParulaColormap final : public LUTColormap
   {
   public:
      ParulaColormap()
         :
         LUTColormap(
            "parula",
            "Parula",
            ColormapUtils::MakeParula())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "parula",
      "Parula",
      []()
      {
         return std::make_unique<ParulaColormap>();
      });
}