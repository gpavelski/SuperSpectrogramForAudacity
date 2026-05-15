/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramInfernoColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class InfernoColormap final : public LUTColormap
   {
   public:
      InfernoColormap()
         :
         LUTColormap(
            "inferno",
            "Inferno",
            ColormapUtils::MakeInferno())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "inferno",
      "Inferno",
      []()
      {
         return std::make_unique<InfernoColormap>();
      });
}