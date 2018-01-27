#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
	settings.windowMode = OF_FULLSCREEN;
    settings.multiMonitorFullScreen = true;
    
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
