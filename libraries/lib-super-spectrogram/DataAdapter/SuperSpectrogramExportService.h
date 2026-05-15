/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramExportService.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_EXPORT_SERVICE__
#define __AUDACITY_SUPER_SPECTROGRAM_EXPORT_SERVICE__

#include <vector>
#include <string>
#include <fstream>
#include <wx/bitmap.h>

class SuperSpectrogramExportService
{
public:
   void ExportMatrixAsText(
      const std::vector<std::vector<double>>& matrix,
      const std::string& filePath
   );

   void ExportViewAsPNG(
      const wxBitmap& bitmap,
      const std::string& filePath
   );
};

#endif
