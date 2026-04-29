/**********************************************************************

  Audacity: A Digital Audio Editor

  SuperSpectrogramSession.h

  Guilherme Pavelski

**********************************************************************/

#ifndef __SUPER_SPECTROGRAM_SESSION__
#define __SUPER_SPECTROGRAM_SESSION__

#include "SuperSpectrogramController.h"

class SuperSpectrogramSession
{
public:
   SuperSpectrogramSession(AudacityProject& project)
      : mProject(project)
   {}

   void Show()
   {
      if (mView) {
         mView->Raise();
         mView->SetFocus();
         return;
      }

      Create();
   }

private:
   void Create()
   {
      mView = new SuperSpectrogramView(
         &GetProjectFrame(mProject),
         wxID_ANY,
         SuperSpectrogramTitle,
         wxPoint{150,150}
      );

      mAudioExtractor = std::make_unique<SuperSpectrogramAudioExtractor>(mProject);
      mConfig = std::make_unique<SuperSpectrogramConfig>();
      mModel = std::make_unique<SuperSpectrogramModel>();

      mConfig->Load();

      mController = std::make_unique<SuperSpectrogramController>(
         *mAudioExtractor,
         *mConfig,
         *mModel,
         *mView
      );

      mController->BindView();

      if (!mController->Initialize()) {
         Destroy();
         return;
      }

      // Hook lifecycle cleanup
      mView->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
         Destroy();
         evt.Skip();
      });

      mView->Show(true);
   }

   void Destroy()
   {
      if (mView) {
         mView->Destroy();
         mView = nullptr;
      }

      mController.reset();
      mAudioExtractor.reset();
      mConfig.reset();
      mModel.reset();
   }

private:
   AudacityProject& mProject;

   SuperSpectrogramView* mView = nullptr;

   std::unique_ptr<SuperSpectrogramController> mController;
   std::unique_ptr<SuperSpectrogramAudioExtractor> mAudioExtractor;
   std::unique_ptr<SuperSpectrogramModel> mModel;
   std::unique_ptr<SuperSpectrogramConfig> mConfig;
};

#endif
