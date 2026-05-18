/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapRegistry.h

  Guilherme Pavelski

*******************************************************************/

#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include "Colormap/SuperSpectrogramColormap.h"

class IColormap;

class SuperSpectrogramColormapRegistry
{
public:
   using Factory =
      std::function<std::unique_ptr<IColormap>()>;

   struct Entry
   {
      wxString id;
      wxString displayName;
      Factory factory;
   };

   static SuperSpectrogramColormapRegistry& Instance();

   void Register(
      const wxString& id,
      const wxString& displayName,
      Factory factory
   );

   std::unique_ptr<IColormap> Create(
      const wxString& id
   ) const;

   const std::vector<Entry>& GetEntries() const;

private:
   std::vector<Entry> mEntries;
};

