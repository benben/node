#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	//ofSetupOpenGL(&window, 1440,768, OF_WINDOW);			// <-------- setup the GL context
    ofSetupOpenGL(&window, 2048,768, OF_GAME_MODE);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
