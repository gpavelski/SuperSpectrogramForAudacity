/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramViridisColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapUtils.h"
#include "SuperSpectrogramColormapRegistration.h"

namespace
{
   class ViridisColormap final : public LUTColormap
   {
   public:
      ViridisColormap()
         :
         LUTColormap(
            "viridis",
            "Viridis",
            ColormapUtils::MakeViridis())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "viridis",
      "Viridis",
      []()
      {
         return std::make_unique<ViridisColormap>();
      });
}