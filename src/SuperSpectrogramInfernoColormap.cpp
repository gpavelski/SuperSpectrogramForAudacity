/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramInfernoColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapUtils.h"
#include "SuperSpectrogramColormapRegistration.h"

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