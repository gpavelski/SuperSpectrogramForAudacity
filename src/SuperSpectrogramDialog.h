/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramDialog.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __AUDACITY_SUPER_SPECTROGRAM_DIALOG__
#define __AUDACITY_SUPER_SPECTROGRAM_DIALOG__

#include <memory>
#include <wx/window.h>

#include "CommandContext.h"
#include "CommandManager.h"
#include "ProjectWindows.h"
#include "CommonCommandFlags.h"
#include "View/SuperSpectrogramSession.h"

class AudacityProject;

DECLARE_EXPORTED_EVENT_TYPE(AUDACITY_DLL_API, EVT_SUPER_SPECTROGRAM_RECALC, -1);

class SuperSpectrogramDialog final
{
public:
    // Constructor: takes the project and optional parent window
    SuperSpectrogramDialog(AudacityProject& project, wxWindow* parent = nullptr);

    // Show the dialog
    void Show(bool show = true);

    // Singleton-style static accessor (like gSession)
    static std::unique_ptr<SuperSpectrogramSession>& Instance();

private:
    // Internal initialization
    void Initialize(AudacityProject& project, wxWindow* parent);

    // The session object
    static std::unique_ptr<SuperSpectrogramSession> gSession;
};

#endif // __AUDACITY_SUPER_SPECTROGRAM_DIALOG__