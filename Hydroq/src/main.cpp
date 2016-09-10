#include "ofMain.h"
#include "CogApp.h"
#include "Node.h"
#include "RotateAnim.h"
#include "TranslateAnim.h"
#include "HitEvent.h"
#include "Collider.h"

using namespace std;
using namespace Cog;

#include "CogEngine.h"


class MenuBehavior : public Behavior {
	OBJECT_PROTOTYPE(MenuBehavior)

	void Init() {
		RegisterListening(ACT_OBJECT_HIT_ENDED);
	}

	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
			if (msg.GetSourceObject()->GetTag().compare("sgame_but") == 0) {
				// click on single game button -> switch scene
				auto sceneContext = GETCOMPONENT(SceneContext);
				auto scene = sceneContext->FindSceneByName("game");
				sceneContext->SwitchToScene(scene, TweenDirection::UP);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};


class XmlTestingApp : public CogApp {


	void InitComponents() {
		REGISTER_BEHAVIOR(MenuBehavior);
	}

	void InitEngine() {
		ofxXmlSettings* xml = new ofxXmlSettings();
		xml->loadFile("config.xml");
		auto xmlPtr = spt<ofxXmlSettings>(xml);

		COGEngine.Init(xmlPtr);

		xmlPtr->popAll();
		xmlPtr->pushTag("app_config");
		xmlPtr->pushTag("scenes");

		auto context = GETCOMPONENT(SceneContext);
		context->LoadScenesFromXml(xmlPtr);

		xmlPtr->popTag();
	}
};




#ifdef WIN32
#ifdef TESTING

#else



int main() {
	ofSetupOpenGL(800, 450, OF_WINDOW);
	ofRunApp(new XmlTestingApp());

	return 0;
}
#endif

#else
#include <jni.h>

int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new XmlTestingApp());
	return 0;
}


//========================================================================
extern "C" {
	void Java_cc_openframeworks_OFAndroid_init(JNIEnv*  env, jobject  thiz) {
		main();
	}
}
#endif

