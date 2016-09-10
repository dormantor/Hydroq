#include "ofxCogMain.h"
#include "MenuBehavior.h"
#include "MenuIconBehavior.h"
#include "HydroqBoard.h"
#include "RightPanel.h"
#include "RightPanelSections.h"
#include "BrickClickEvent.h"
#include "BrickEventBehavior.h"
#include "SelectedFuncBehavior.h"
#include "HydroqPlayerModel.h"
#include "HydroqGameModel.h"
#include "HydroqGameView.h"
#include "HydroqPlayerController.h"
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
#include "ConfirmDialog.h"
#include "TopPanel.h"
#include "GameEndDialog.h"

void RegenerateSpriteSheets() {
	SpriteSheetGenerator generator = SpriteSheetGenerator();
	generator.AddImage("F:\\rig.png");
	
	generator.AddImage("F:\\rb1.png");
	generator.AddImage("F:\\rb2.png");
	generator.AddImage("F:\\rb3.png");
	generator.AddImage("F:\\rb4.png");
	generator.AddImage("F:\\rb5.png");
	generator.AddImage("F:\\rb6.png");
	generator.AddImage("F:\\rb7.png");
	generator.AddImage("F:\\rb8.png");

	generator.AddImage("F:\\rr1.png");
	generator.AddImage("F:\\rr2.png");
	generator.AddImage("F:\\rr3.png");
	generator.AddImage("F:\\rr4.png");
	generator.AddImage("F:\\rr5.png");
	generator.AddImage("F:\\rr6.png");
	generator.AddImage("F:\\rr7.png");
	generator.AddImage("F:\\rr8.png");

	generator.AddImage("F:\\water.png");
	generator.AddImage("F:\\platform.png");
	generator.AddImage("F:\\rigplace.png");
	generator.AddImage("F:\\tobuild.png");
	generator.AddImage("F:\\forbidden.png");
	generator.AddImage("F:\\destroy.png");

	generator.AddImage("F:\\worker_blue_1.png");
	generator.AddImage("F:\\worker_blue_2.png");
	generator.AddImage("F:\\worker_blue_3.png");
	generator.AddImage("F:\\worker_blue_4.png");
	generator.AddImage("F:\\worker_red_1.png");
	generator.AddImage("F:\\worker_red_2.png");
	generator.AddImage("F:\\worker_red_3.png");
	generator.AddImage("F:\\worker_red_4.png");


	generator.AddImage("F:\\explo_01.png");
	generator.AddImage("F:\\explo_02.png");
	generator.AddImage("F:\\explo_03.png");
	generator.AddImage("F:\\explo_04.png");
	generator.AddImage("F:\\explo_05.png");
	generator.AddImage("F:\\explo_06.png");
	generator.AddImage("F:\\explo_07.png");
	generator.AddImage("F:\\explo_08.png");
	generator.AddImage("F:\\explo_09.png");
	generator.AddImage("F:\\explo_10.png");
	generator.AddImage("F:\\explo_11.png");


	generator.GenerateSheet(256, 256, 4096, 4096, "F:\\result.png");
}



/**
* Back button simulator that checks BACKSPACE key
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

class ReportKey : public Behavior {

		void Init() {
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		for (auto key : CogGetPressedKeys()) {

			if (!key->IsHandled()) {

				if (key->key == (int)('t')) {
					key->handlerNodeId = owner->GetId();
					
					// write report
					TimeMeasure::GetInstance().Report(true);
				}
			}
		}
	}
};


class HydroqApp : public CogApp {
public:

	HydroqApp() : CogApp("config.xml") {

	}

	void RegisterComponents() {
		REGISTER_BEHAVIOR(MenuBehavior);
		REGISTER_BEHAVIOR(SingleGameMenu);
		REGISTER_BEHAVIOR(MultiplayerMenu);
		REGISTER_BEHAVIOR(MenuIconBehavior);
		REGISTER_BEHAVIOR(HydroqBoard);
		REGISTER_BEHAVIOR(RightPanel);
		REGISTER_BEHAVIOR(RightPanelSections);
		REGISTER_BEHAVIOR(BrickEventBehavior);
		REGISTER_BEHAVIOR(SelectedFuncBehavior);
		REGISTER_BEHAVIOR(HydroqPlayerController);
		REGISTER_BEHAVIOR(LeftPanel);
		REGISTER_BEHAVIOR(HostInit);
		REGISTER_BEHAVIOR(AttractorPlacement);
		REGISTER_BEHAVIOR(ConfirmDialog);
		REGISTER_BEHAVIOR(TopPanel);
		REGISTER_BEHAVIOR(GameEndDialog);
		REGISTER_BEHAVIOR(HydroqGameModel);
		REGISTER_BEHAVIOR(HydroqGameView);
		REGISTER_BEHAVIOR(HydNetworkSender);
		REGISTER_BEHAVIOR(HydNetworkReceiver);

		auto playerModel = new HydroqPlayerModel();
		auto deltaUpdate = new DeltaUpdate();

		REGISTER_COMPONENT(playerModel);
		REGISTER_COMPONENT(deltaUpdate);
		REGISTER_COMPONENT(new NetworkCommunicator());
		REGISTER_COMPONENT(new LuaScripting());

	}

	void InitComponents() {
	}

	void InitStage(Stage* stage) {
		CogEngine::GetInstance().LoadStageFromXml(this->xmlConfig);
		stage->GetRootObject()->AddBehavior(new BackButtonKey());
		stage->GetRootObject()->AddBehavior(new ReportKey());

		//RegenerateSpriteSheets();
	}

#ifdef ANDROID

	virtual bool OnBackPress() {
		auto stage = GETCOMPONENT(Stage);
		if (stage->GetActualScene() != nullptr && stage->GetActualScene()->GetName().compare("game") == 0) {
			auto scene = stage->FindSceneByName("confirm_dialog");
			stage->SwitchToScene(scene, TweenDirection::NONE);
			return false;
		}
		else return true;
	}

#endif
};

// todo: specify later
#define NOSCRIPTING
#define NODATABASE

#define TESTING

#ifdef WIN32
#ifdef TESTING


#define CATCH_CONFIG_RUNNER

#include "FlagsTest.h"
#include "TransformTest.h"
#include "SQLTest.h"
#include "MeasureTest.h"
#include "NetworkTest.h"
#include "StateMachineTest.h"
#include "GoalTest.h"
#include "SettingsTest.h"
#include "EngineTest.h"
#include "StrIdTest.h"
#include "JavaScriptTest.h"
#include "LuaTest.h"
#include "MonteCarloTest.h"
#include "CoroutineTest.h"
#include "cpplinq.hpp"

int main() {
	ofSetupOpenGL(800, 450, OF_WINDOW);
	int result = Catch::Session().run();
	ofRunApp(new HydroqApp());

	return 0;
}

#else

int main() {
	ofSetupOpenGL(800, 450, OF_WINDOW);
	ofRunApp(new HydroqApp());

	return 0;
}


#endif

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

