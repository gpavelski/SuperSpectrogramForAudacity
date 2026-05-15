/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramDialog.cpp

  Guilherme Pavelski

**********************************************************************/

#include "CommandContext.h"
#include "CommandManager.h"
#include "ProjectWindows.h"
#include "CommonCommandFlags.h"

#include "View/SuperSpectrogramSession.h"

namespace
{
   std::unique_ptr<SuperSpectrogramSession> gSession;

   void OnPlotSuperSpectrogram(const CommandContext& context)
   {
      auto& project = context.project;

      CommandManager::Get(project)
         .RegisterLastAnalyzer(context);

      auto* parent =
         &GetProjectFrame(project);

      gSession =
         std::make_unique<SuperSpectrogramSession>(
            project,
            parent
         );

      gSession->Show();
   }

   using namespace MenuRegistry;

   AttachedItem sAttachment{
      Command(
         wxT("PlotSuperSpectrogram"),
         XXO("Plot Super Spectrogram..."),
         OnPlotSuperSpectrogram,
         AudioIONotBusyFlag()
            | WaveTracksSelectedFlag()
            | TimeSelectedFlag()
      ),
      wxT("Analyze/Analyzers/Windows")
   };
}