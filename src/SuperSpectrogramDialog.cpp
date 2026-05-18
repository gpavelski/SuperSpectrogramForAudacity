/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramDialog.cpp

  Guilherme Pavelski

**********************************************************************/

#include "SuperSpectrogramDialog.h"

std::unique_ptr<SuperSpectrogramSession> SuperSpectrogramDialog::gSession;

SuperSpectrogramDialog::SuperSpectrogramDialog(AudacityProject& project, wxWindow* parent)
{
    Initialize(project, parent);
}

void SuperSpectrogramDialog::Initialize(AudacityProject& project, wxWindow* parent)
{
    gSession = std::make_unique<SuperSpectrogramSession>(project, parent);
}

void SuperSpectrogramDialog::Show(bool show)
{
    if (gSession)
        gSession->Show();
}

std::unique_ptr<SuperSpectrogramSession>& SuperSpectrogramDialog::Instance()
{
    return gSession;
}

// Optional: menu callback
namespace
{
    void OnPlotSuperSpectrogram(const CommandContext& context)
    {
        auto* parent = &GetProjectFrame(context.project);
        SuperSpectrogramDialog dialog(context.project, parent);
        dialog.Show();
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