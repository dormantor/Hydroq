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
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED);
	}

	void OnMessage(Msg& msg) {


		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED) {
			if (msg.GetSourceObject()->GetTag().compare("sgame_but") == 0) {
				// click on single game button -> switch scene
				auto sceneContext = GETCOMPONENT(SceneContext);
				auto scene = sceneContext->FindSceneByName("main_menu_map");
				sceneContext->SwitchToScene(scene);
			}else if (msg.GetSourceObject()->GetTag().compare("play_but") == 0) {
				// click on play button -> switch scene
				auto sceneContext = GETCOMPONENT(SceneContext);
				auto scene = sceneContext->FindSceneByName("game");
				sceneContext->SwitchToScene(scene, TweenDirection::LEFT);
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

#include "TransformAnim.h"

class MenuIconBehavior : public Behavior {
	OBJECT_PROTOTYPE(MenuIconBehavior)

		Trans initTrans; 
	    Trans animTrans;

	void Init() {
		RegisterListening(owner->GetScene(), ACT_OBJECT_HIT_ENDED, ACT_TRANSFORM_ENDED);

		Node* menu = owner->GetScene()->FindNodeByTag("rightpanel");
		initTrans = menu->GetTransform();


		TransformMath math = TransformMath();
		math.CalcTransform(animTrans, menu, menu->GetParent(), ofVec2f(100, 6), 10, CalcType::GRID, ofVec2f(1,0), ofVec2f(1), CalcType::LOC, 100, 53);
	}


	void OnMessage(Msg& msg) {
		if (msg.GetAction() == ACT_TRANSFORM_ENDED && msg.GetSourceObject()->GetTag().compare("rightpanel") == 0) {
			owner->ResetState(StringHash(STATES_LOCKED));
		}


		if (msg.GetAction() == ACT_OBJECT_HIT_ENDED && msg.GetSourceObject()->GetId() == owner->GetId()) {
			if (!owner->HasState(StringHash(STATES_LOCKED))) {
				owner->SetState(StringHash(STATES_LOCKED));

				if (owner->HasState(StringHash(STATES_ENABLED))) {
					owner->ResetState(StringHash(STATES_ENABLED));
					// roll menu back
					Node* menu = CogFindNodeByTag("rightpanel");
					TransformAnim* anim = new TransformAnim(animTrans, initTrans, 250, 0);
					menu->AddBehavior(anim);
				}
				else {
					owner->SetState(StringHash(STATES_ENABLED));

					// roll the menu
					Node* menu = CogFindNodeByTag("rightpanel");
					TransformAnim* anim = new TransformAnim(initTrans, animTrans, 250, 0);
					menu->AddBehavior(anim);
				}
			}
		}
	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {
	}
};

class HydroqBoard : public Behavior {
	OBJECT_PROTOTYPE(HydroqBoard)


	void Init() {
		spt<ofImage> img = CogGet2DImage("bricks/water.png");
		TransformMath mth = TransformMath();

		float brickRow = 20;

		for (int i = 0; i < brickRow; i++) {
			for (int j = 0; j < brickRow; j++) {
				Node* nd = new Node("mojo");
				nd->SetShape(spt<Shape>(new Image(img)));
				mth.SetTransform(nd, owner, ofVec2f(1.0f/brickRow*i, 1.0f/brickRow*j), 5, CalcType::PER, ofVec2f(0), ofVec2f(1.0f/brickRow), CalcType::PER, 5, 5);
				owner->AddChild(nd);
			}
		}
	}

	void OnMessage(Msg& msg) {

	}

public:
	virtual void Update(const uint64 delta, const uint64 absolute) {

	}
};

class XmlTestingApp : public CogApp {


	void InitComponents() {
		REGISTER_BEHAVIOR(MenuBehavior);
		REGISTER_BEHAVIOR(MenuIconBehavior);
		REGISTER_BEHAVIOR(HydroqBoard);
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

