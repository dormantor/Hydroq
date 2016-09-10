#include "ofxCogMain.h"
#include "MenuBehavior.h"
#include "MenuIconBehavior.h"
#include "Board.h"
#include "RightPanel.h"
#include "RightPanelSections.h"
#include "BrickClickEvent.h"
#include "BrickEventBehavior.h"

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


class HydroqApp : public CogApp {
public:

	HydroqApp() : CogApp("config.xml") {

	}

	void InitComponents() {
		REGISTER_BEHAVIOR(MenuBehavior);
		REGISTER_BEHAVIOR(MenuIconBehavior);
		REGISTER_BEHAVIOR(HydroqBoard);
		REGISTER_BEHAVIOR(RightPanel);
		REGISTER_BEHAVIOR(RightPanelSections);
		REGISTER_BEHAVIOR(BrickEventBehavior);
	}

	void InitStage(Stage* stage) {
		CogEngine::GetInstance().LoadStageFromXml(this->xmlConfig);
		stage->GetRootObject()->AddBehavior(new BackButtonKey());
	}
};

//#define TESTING

#ifdef WIN32
#ifdef TESTING


#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "FlagsTest.h"
#include "TransformTest.h"
#include "StringHashTest.h"
#include "SettingsTest.h"
#include "SQLTest.h"
#include "MeasureTest.h"
#include "EngineTest.h"
#include "NetworkTest.h"
#include "StateMachineTest.h"
#include "GoalTest.h"
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

