#include "ofMain.h"
#include "CogApp.h"
#include "Factory.h"
#include "Node.h"
#include "RotateAnim.h"
#include "TranslateAnim.h"
#include "HitEvent.h"
#include "Collider.h"

using namespace std;
using namespace Cog;

#include "CogEngine.h"



class XmlTestingApp : public CogApp {


	void InitComponents() {

	}

	void InitEngine() {
		ofxXmlSettings* xml = new ofxXmlSettings();
		xml->loadFile("config.xml");
		auto xmlPtr = spt<ofxXmlSettings>(xml);

		COGEngine.Init(xmlPtr);

		xmlPtr->popAll();
		xmlPtr->pushTag("app_config");
		xmlPtr->pushTag("scenes");
		xmlPtr->pushTag("scene", 0);

		auto mgr = GETCOMPONENT(NodeContext);
		mgr->LoadSceneFromXml(xmlPtr);
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

