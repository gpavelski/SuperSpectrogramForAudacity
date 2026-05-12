/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderer.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramRenderer.h"
#include "SuperSpectrogramColormap.h"
#include <wx/bitmap.h>

wxBitmap SuperSpectrogramRenderer::Render(
   const std::vector<float>& normalized,
   size_t rows,
   size_t cols,
   const IColormap& colormap) const
{
   if (normalized.empty() || rows == 0 || cols == 0)
      return wxBitmap();

   wxImage img(static_cast<int>(cols), static_cast<int>(rows), false);

   unsigned char* data = img.GetData();

   for (size_t y = 0; y < rows; ++y)
   {
      for (size_t x = 0; x < cols; ++x)
      {
         float norm = normalized[y * cols + x];

         const wxColour& c = colormap.Map(norm);

         size_t offset = 3 * (y * cols + x);
         data[offset + 0] = c.Red();
         data[offset + 1] = c.Green();
         data[offset + 2] = c.Blue();
      }
   }

   return wxBitmap(img);
}