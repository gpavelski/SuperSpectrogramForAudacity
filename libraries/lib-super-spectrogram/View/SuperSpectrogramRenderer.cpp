/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramRenderer.cpp

  Guilherme Pavelski

*******************************************************************/

#include "View/SuperSpectrogramRenderer.h"

//------------------------------------------------------------
// Core rendering
//------------------------------------------------------------
wxBitmap SuperSpectrogramRenderer::Render(
   const SuperSpectrogramRenderContext& ctx,
   const wxSize& target
) const
{
   const int width = target.GetWidth();
   const int height = target.GetHeight();

   wxImage img(width, height, false);
   unsigned char* data = img.GetData();

   const double left   = ctx.viewport.Left();
   const double right  = ctx.viewport.Right();
   const double top    = ctx.viewport.Top();
   const double bottom = ctx.viewport.Bottom();

   const double dx = (right - left) / width;
   const double dy = (bottom - top) / height;

   for (int py = 0; py < height; ++py)
   {
      for (int px = 0; px < width; ++px)
      {
         double fx = left + px * dx;
         double fy = top + py * dy;

         size_t ix = static_cast<size_t>(fx);
         size_t iy = static_cast<size_t>(fy);

         float norm = ctx.normalized[iy * ctx.cols + ix];

         const wxColour& c = ctx.colormap.Map(norm);

         size_t offset = 3 * (py * width + px);
         data[offset + 0] = c.Red();
         data[offset + 1] = c.Green();
         data[offset + 2] = c.Blue();
      }
   }

   return wxBitmap(img);
}