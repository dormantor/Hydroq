#include "ofxCogMain.h"
#include "MenuBehavior.h"
#include "MenuIconBehavior.h"
#include "Board.h"
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

/**
* Back button simulator that checks BACKSPACE key
*/
class BackButtonKey : public Behavior {
	OBJECT_PROTOTYPE(BackButtonKey)

	void Init() {
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
		for (auto key : CogGetPressedKeys()) {

			if (!key->IsHandled()) {
				
				if (key->key == OF_KEY_BACKSPACE) {
					// handle key press
					key->handlerNodeId = owner->GetId();
					// simulate back button
					auto sceneContext = GETCOMPONENT(Stage);
					sceneContext->SwitchBackToScene(TweenDirection::NONE);
				}
			}
		}
	}
};

class ReportKey : public Behavior {
	OBJECT_PROTOTYPE(ReportKey)

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

		auto playerModel = new HydroqPlayerModel();
		auto gameModel = new HydroqGameModel();
		auto view = new HydroqGameView();
		auto netSender = new HydNetworkSender();
		auto netReceiver = new HydNetworkReceiver();
		auto deltaUpdate = new DeltaUpdate();

		REGISTER_COMPONENT(playerModel);
		REGISTER_COMPONENT(gameModel);
		REGISTER_COMPONENT(view);
		REGISTER_COMPONENT(netSender);
		REGISTER_COMPONENT(netReceiver);
		REGISTER_COMPONENT(deltaUpdate);

		auto comm = new NetworkCommunicator();
		REGISTER_COMPONENT(comm);

		REGISTER_COMPONENT(new LuaScripting());

	}

	void InitComponents() {
	}

	void InitStage(Stage* stage) {
		CogEngine::GetInstance().LoadStageFromXml(this->xmlConfig);
		stage->GetRootObject()->AddBehavior(new BackButtonKey());
		stage->GetRootObject()->AddBehavior(new ReportKey());
	}
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
#include "StringHashTest.h"
#include "JavaScriptTest.h"
#include "LuaTest.h"
#include "MonteCarloTest.h"
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

