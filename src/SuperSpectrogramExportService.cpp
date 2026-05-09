/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramExportService.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramExportService.h"
#include <fstream>

void SuperSpectrogramExportService::ExportMatrixAsText(
   const std::vector<std::vector<double>>& matrix,
   const std::string& filePath)
{
   std::ofstream out(filePath);
   if (!out.is_open())
      return;

   for (const auto& column : matrix)
   {
      for (size_t i = 0; i < column.size(); ++i)
      {
         out << column[i];
         if (i + 1 < column.size())
            out << '\t';
      }
      out << '\n';
   }
}

void SuperSpectrogramExportService::ExportViewAsPNG(
   const wxBitmap& bitmap,
   const std::string& filePath)
{
   if (!bitmap.IsOk())
      return;

   bitmap.SaveFile(filePath, wxBITMAP_TYPE_PNG);
}
