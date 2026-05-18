/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapUtils.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

#include <wx/colour.h>
#include <vector>
#include <algorithm>

namespace ColormapUtils
{
   // Linear interpolation between two colors
   inline wxColour Lerp(const wxColour& a, const wxColour& b, double t)
   {
      return wxColour(
         static_cast<unsigned char>(a.Red() + t * (b.Red() - a.Red())),
         static_cast<unsigned char>(a.Green() + t * (b.Green() - a.Green())),
         static_cast<unsigned char>(a.Blue() + t * (b.Blue() - a.Blue()))
      );
   }

   // Build a color map from anchors with linear interpolation
   inline std::vector<wxColour> BuildFromAnchors(
      const std::vector<wxColour>& anchors,
      size_t resolution = 256
   )
   {
      std::vector<wxColour> cmap;
      cmap.reserve(resolution);

      const size_t segments = anchors.size() - 1;
      for (size_t i = 0; i < resolution; ++i)
      {
         double x = static_cast<double>(i) / (resolution - 1);
         double scaled = x * segments;
         size_t idx = static_cast<size_t>(scaled);
         double t = scaled - idx;

         if (idx >= segments)
            cmap.push_back(anchors.back());
         else
            cmap.push_back(Lerp(anchors[idx], anchors[idx + 1], t));
      }

      return cmap;
   }
}
