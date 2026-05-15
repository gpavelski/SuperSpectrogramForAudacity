/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramColormapRegistry.cpp

  Guilherme Pavelski

*******************************************************************/

#include "SuperSpectrogramColormapRegistry.h"
#include "SuperSpectrogramColormap.h"

SuperSpectrogramColormapRegistry&
SuperSpectrogramColormapRegistry::Instance()
{
   static SuperSpectrogramColormapRegistry instance;
   return instance;
}

void SuperSpectrogramColormapRegistry::Register(
   const wxString& id,
   const wxString& displayName,
   Factory factory)
{
   mEntries.push_back({
      id,
      displayName,
      std::move(factory)
   });
}

std::unique_ptr<IColormap>
SuperSpectrogramColormapRegistry::Create(
   const wxString& id) const
{
   for (const auto& entry : mEntries)
   {
      if (entry.id == id)
         return entry.factory();
   }

   return nullptr;
}

const std::vector<
   SuperSpectrogramColormapRegistry::Entry>&
SuperSpectrogramColormapRegistry::GetEntries() const
{
   return mEntries;
}