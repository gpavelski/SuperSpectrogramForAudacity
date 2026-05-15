/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramJetColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "Colormap/SuperSpectrogramColormap.h"
#include "Colormap/SuperSpectrogramColormapUtils.h"
#include "Colormap/SuperSpectrogramColormapRegistration.h"

namespace
{
   class JetColormap final : public LUTColormap
   {
   public:
      JetColormap()
         :
         LUTColormap(
            "jet",
            "Jet",
            ColormapUtils::MakeJet())
      {
      }
   };

   ColormapRegistrar sRegistrar(
      "jet",
      "Jet",
      []()
      {
         return std::make_unique<JetColormap>();
      });
}