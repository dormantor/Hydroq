#include "ofxCogMain.h"
#include "MenuIconBehavior.h"
#include "GameBoard.h"
#include "RightPanel.h"
#include "RightPanelSections.h"
#include "TileEventBehavior.h"
#include "SelectedFuncBehavior.h"
#include "PlayerModel.h"
#include "GameModel.h"
#include "GameView.h"
#include "PlayerController.h"
#include "LeftPanel.h"
#include "SingleGameMenu.h"
#include "MultiplayerMenu.h"
#include "HostInit.h"
#include "NetworkCommunicator.h"
#include "HydNetworkSender.h"
#include "HydNetworkReceiver.h"
#include "TimeMeasure.h"
#include "AttractorPlacement.h"

#include "LuaScripting.h"

#include "HydAIAction.h"
#include "HydAISimulator.h"
#include "HydAIState.h"
#include "SpriteSheetGenerator.h"
#include "TopPanel.h"
#include "HydroqLuaMapper.h"

/**
* Back button simulator that checks BACKSPACE key
* Only for Windows
*/
class BackButtonKey : public Behavior {
	
public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		for (auto key : CogGetPressedKeys()) {

			if (!key->IsHandled()) {
				
				if (key->key == OF_KEY_BACKSPACE) {
					// handle key press
					key->handlerNodeId = owner->GetId();
					// simulate back button
					
					auto stage = GETCOMPONENT(Stage);
					if (stage->GetActualScene() != nullptr && stage->GetActualScene()->GetName().compare("game") == 0) {
						auto scene = stage->FindSceneByName("confirm_dialog");
						stage->SwitchToScene(scene, TweenDirection::NONE);
					}
					else {
						stage->SwitchBackToScene(TweenDirection::NONE);
					}
				}
			}
		}
	}
};


class HydroqApp : public ofxCogApp {
public:

	HydroqApp() : ofxCogApp("config/config.xml") {

	}

	void RegisterComponents() {
		REGISTER_BEHAVIOR(SingleGameMenu);
		REGISTER_BEHAVIOR(MultiplayerMenu);
		REGISTER_BEHAVIOR(MenuIconBehavior);
		REGISTER_BEHAVIOR(GameBoard);
		REGISTER_BEHAVIOR(RightPanel);
		REGISTER_BEHAVIOR(RightPanelSections);
		REGISTER_BEHAVIOR(TileEventBehavior);
		REGISTER_BEHAVIOR(SelectedFuncBehavior);
		REGISTER_BEHAVIOR(PlayerController);
		REGISTER_BEHAVIOR(LeftPanel);
		REGISTER_BEHAVIOR(HostInit);
		REGISTER_BEHAVIOR(AttractorPlacement);
		REGISTER_BEHAVIOR(TopPanel);
		REGISTER_BEHAVIOR(GameModel);
		REGISTER_BEHAVIOR(GameView);
		REGISTER_BEHAVIOR(HydNetworkSender);
		REGISTER_BEHAVIOR(HydNetworkReceiver);

		auto playerModel = new PlayerModel();
		auto interpolator = new Interpolator();

		REGISTER_COMPONENT(playerModel);
		REGISTER_COMPONENT(interpolator);
		REGISTER_COMPONENT(new NetworkCommunicator());
		REGISTER_COMPONENT(new LuaScripting());
		REGISTER_COMPONENT(new HydroqLuaMapper());
	}

	void InitComponents() {
	}

	void InitStage(Stage* stage) {
		ofxCogEngine::GetInstance().LoadStageFromXml(this->xmlConfig);
		stage->GetRootObject()->AddBehavior(new BackButtonKey());
	}

#ifdef ANDROID

	virtual bool OnBackPress() {
		auto stage = GETCOMPONENT(Stage);
		if (stage->GetActualScene() != nullptr && stage->GetActualScene()->GetName().compare("game") == 0) {
			auto scene = stage->FindSceneByName("confirm_dialog");
			stage->SwitchToScene(scene, TweenDirection::NONE);
			return true;
		}
		else return false;
	}

#endif
};

#ifdef WIN32

int main() {
	ofSetupOpenGL(800, 450, OF_WINDOW);
	ofRunApp(new HydroqApp());

	return 0;
}

#else
#include <jni.h>

int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new HydroqApp());
	return 0;
}


//========================================================================
extern "C" {
	void Java_cc_openframeworks_OFAndroid_init(JNIEnv*  env, jobject  thiz) {
		main();
	}
}
#endif

