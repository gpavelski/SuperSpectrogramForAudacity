/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramDataAdapter.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramDataAdapter.h"
#include <limits>
#include <cmath>
#include <algorithm>

void SuperSpectrogramDataAdapter::SetFrame(
   const SuperSpectrogramFrame& frame)
{
   m_matrix = frame.matrix;
   m_maxFreq = frame.maxFreq;
   m_numSamples = frame.numSamples;

   Normalize();
}

void SuperSpectrogramDataAdapter::Normalize()
{
   if (m_matrix.empty() || m_matrix[0].empty())
      return;

   m_rows = m_matrix.size();
   m_cols = m_matrix[0].size();

   m_normalized.resize(m_rows * m_cols);

   double minv = std::numeric_limits<double>::infinity();
   double maxv = -std::numeric_limits<double>::infinity();

   // Pass 1
   for (const auto& row : m_matrix)
      for (double v : row)
         if (std::isfinite(v)) {
            minv = std::min(minv, v);
            maxv = std::max(maxv, v);
         }

   if (!std::isfinite(minv) || minv == maxv) {
      minv = 0.0;
      maxv = 1.0;
   }

   const double invRange = 1.0 / (maxv - minv + 1e-9);

   // Pass 2
   for (size_t y = 0; y < m_rows; ++y)
   {
      for (size_t x = 0; x < m_cols; ++x)
      {
         double v = m_matrix[y][x];

         float norm = 0.0f;
         if (std::isfinite(v))
            norm = static_cast<float>((v - minv) * invRange);

         m_normalized[y * m_cols + x] = norm;
      }
   }
}