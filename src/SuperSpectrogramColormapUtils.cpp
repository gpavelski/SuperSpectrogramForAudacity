/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapUtils.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormapUtils.h"

//----------------------------------------------------------------------
// Generates a 256-entry Jet-style colormap for value-to-color mapping
//----------------------------------------------------------------------
std::vector<wxColour> ColormapUtils::MakeJet()
{
   std::vector<wxColour> cmap(256);
   for (int i = 0; i < 256; ++i) {
      double x = i / 255.0;
      double r = std::clamp(1.5 - std::abs(4 * x - 3.0), 0.0, 1.0);
      double g = std::clamp(1.5 - std::abs(4 * x - 2.0), 0.0, 1.0);
      double b = std::clamp(1.5 - std::abs(4 * x - 1.0), 0.0, 1.0);
      cmap[i] = wxColour(
         static_cast<unsigned char>(r * 255),
         static_cast<unsigned char>(g * 255),
         static_cast<unsigned char>(b * 255)
      );
   }
   return cmap;
}

std::vector<wxColour> ColormapUtils::MakeGray()
{
   std::vector<wxColour> cmap(256);
   for (int i = 0; i < 256; ++i)
      cmap[i] = wxColour(i, i, i);
   return cmap;
}

std::vector<wxColour> ColormapUtils::MakeViridis()
{
   const std::vector<wxColour> anchors = {
      wxColour(68,  1,  84),   // dark purple
      wxColour(71,  44, 122),
      wxColour(59,  81, 139),
      wxColour(44, 113, 142),
      wxColour(33, 144, 141),
      wxColour(39, 173, 129),
      wxColour(92, 200,  99),
      wxColour(170, 220,  50),
      wxColour(253, 231,  37)  // yellow
   };

   return BuildFromAnchors(anchors);
}

std::vector<wxColour> ColormapUtils::MakeHot()
{
   std::vector<wxColour> cmap(256);

   for (int i = 0; i < 256; ++i) {
      double x = i / 255.0;

      double r = std::min(1.0, 3 * x);
      double g = std::clamp(3 * x - 1.0, 0.0, 1.0);
      double b = std::clamp(3 * x - 2.0, 0.0, 1.0);

      cmap[i] = wxColour(r * 255, g * 255, b * 255);
   }

   return cmap;
}

std::vector<wxColour> ColormapUtils::MakeInferno()
{
   const std::vector<wxColour> anchors = {
      wxColour(0,   0,   4),
      wxColour(31, 12, 72),
      wxColour(85, 15,109),
      wxColour(136,34,106),
      wxColour(186,54, 85),
      wxColour(227,89, 51),
      wxColour(249,140,10),
      wxColour(252,195,65),
      wxColour(252,255,164)
   };

   return BuildFromAnchors(anchors);
}

std::vector<wxColour> ColormapUtils::MakeMagma()
{
   const std::vector<wxColour> anchors = {
      wxColour(0,   0,   4),
      wxColour(28, 16, 68),
      wxColour(79, 18,123),
      wxColour(129,37,129),
      wxColour(181,54,122),
      wxColour(229,80,100),
      wxColour(251,135,97),
      wxColour(254,194,135),
      wxColour(252,253,191)
   };

   return BuildFromAnchors(anchors);
}

std::vector<wxColour> ColormapUtils::MakeCividis()
{
   const std::vector<wxColour> anchors = {
      wxColour(0,  32, 76),
      wxColour(0,  55, 90),
      wxColour(39, 77, 95),
      wxColour(73, 99, 98),
      wxColour(109,122,100),
      wxColour(146,145,99),
      wxColour(184,168,94),
      wxColour(221,192,85),
      wxColour(255,221,70)
   };

   return BuildFromAnchors(anchors);
}

std::vector<wxColour> ColormapUtils::MakeParula()
{
   const std::vector<wxColour> anchors = {
      wxColour(53, 42,135),
      wxColour(15, 92,221),
      wxColour(18,125,216),
      wxColour(7, 156,207),
      wxColour(21,177,180),
      wxColour(89,189,140),
      wxColour(165,190,107),
      wxColour(225,185,82),
      wxColour(252,206,46),
      wxColour(249,251,14)
   };

   return BuildFromAnchors(anchors);
}

wxColour ColormapUtils::Lerp(const wxColour& a,
   const wxColour& b,
   double t)
{
   return wxColour(
      static_cast<unsigned char>(a.Red() + t * (b.Red() - a.Red())),
      static_cast<unsigned char>(a.Green() + t * (b.Green() - a.Green())),
      static_cast<unsigned char>(a.Blue() + t * (b.Blue() - a.Blue()))
   );
}

std::vector<wxColour> ColormapUtils::BuildFromAnchors(
   const std::vector<wxColour>& anchors,
   size_t resolution
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
      {
         cmap.push_back(anchors.back());
      }
      else
      {
         cmap.push_back(Lerp(anchors[idx], anchors[idx + 1], t));
      }
   }

   return cmap;
}
