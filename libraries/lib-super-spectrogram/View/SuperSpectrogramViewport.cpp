/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramViewport.cpp

  Guilherme Pavelski

*******************************************************************/

#include "View/SuperSpectrogramViewport.h"

void SuperSpectrogramViewport::SetBounds(size_t rows, size_t cols)
{
   m_rows = rows;
   m_cols = cols;
   Reset();
}

void SuperSpectrogramViewport::Reset()
{
   m_left = 0.0;
   m_right = static_cast<double>(m_cols);
   m_top = 0.0;
   m_bottom = static_cast<double>(m_rows);
}

void SuperSpectrogramViewport::Zoom(double factor, double fx, double fy)
{
   double width  = m_right - m_left;
   double height = m_bottom - m_top;

   double newWidth  = width * factor;
   double newHeight = height * factor;

   m_left += (width - newWidth) * fx;
   m_right = m_left + newWidth;

   m_top += (height - newHeight) * fy;
   m_bottom = m_top + newHeight;

   Clamp();
}

void SuperSpectrogramViewport::Pan(
   double dxPixels,
   double dyPixels,
   const wxSize& size)
{
   double width  = m_right - m_left;
   double height = m_bottom - m_top;

   double dx = dxPixels / size.GetWidth()  * width;
   double dy = dyPixels / size.GetHeight() * height;

   m_left   -= dx;
   m_right  -= dx;
   m_top    -= dy;
   m_bottom -= dy;

   Clamp();
}

void SuperSpectrogramViewport::Clamp()
{
   if (m_rows == 0 || m_cols == 0)
      return;

   // Horizontal
   if (m_left < 0) m_left = 0;
   if (m_right > m_cols) m_right = m_cols;
   if (m_right - m_left < 2)
      m_right = m_left + 2;

   // Vertical
   if (m_top < 0) m_top = 0;
   if (m_bottom > m_rows) m_bottom = m_rows;
   if (m_bottom - m_top < 2)
      m_bottom = m_top + 2;
}