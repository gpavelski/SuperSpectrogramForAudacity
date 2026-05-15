/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapUtils.h

  Guilherme Pavelski

*******************************************************************/

#ifndef __SUPER_SPECTROGRAM_COLORMAP_UTILS__
#define __SUPER_SPECTROGRAM_COLORMAP_UTILS__

#include <wx/colour.h>
#include <vector>

namespace ColormapUtils
{
   wxColour Lerp(const wxColour& a, const wxColour& b, double t);

   std::vector<wxColour> BuildFromAnchors(
      const std::vector<wxColour>& anchors,
      size_t resolution = 256
   );

   std::vector<wxColour> MakeJet();
   std::vector<wxColour> MakeGray();
   std::vector<wxColour> MakeViridis();
   std::vector<wxColour> MakeHot();
   std::vector<wxColour> MakeViridis();
   std::vector<wxColour> MakeInferno();
   std::vector<wxColour> MakeMagma();
   std::vector<wxColour> MakeCividis();
   std::vector<wxColour> MakeParula();
}

#endif
