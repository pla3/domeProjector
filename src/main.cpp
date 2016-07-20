#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	int w=0, h=0;
	ifstream ifs("data/screensize.txt");
	string str;
	if(ifs.fail()) {
		cerr << "File do not exist.\n";
		exit(0);
	}
	while(getline(ifs, str)) {
		w=0; h=0;
		sscanf(str.data(), "%d,%d", &w, &h);
	}

    ofGLFWWindowSettings settings;
    settings.width = w+PROJECTOR_WIDTH;
    settings.height = PROJECTOR_HEIGHT;
    settings.windowMode = OF_FULLSCREEN;
    settings.multiMonitorFullScreen = true;
    
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
