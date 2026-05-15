/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramJetColormap.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormap.h"
#include "SuperSpectrogramColormapUtils.h"
#include "SuperSpectrogramColormapRegistration.h"

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