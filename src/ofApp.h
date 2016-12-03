//
//  ofApp.h for domeProjector
//
//  Created by Kazushi Mukaiyama on 16/12/03.
//  Copyright 2016 Future University Hakodate. All rights reserved.
//
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "DistortionImage.h"

#define PROJECTOR_WIDTH 1920
#define PROJECTOR_HEIGHT 1080

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void loadButtonPressed();
    void saveButtonPressed();
    void resetButtonPressed();
    void playListClearPressed();
    void playButtonPressed();
    void deviceValueChanged(bool & value);
    
    bool screenInit;
    bool captureMode;
    DistortionImage projector;
    ofVideoPlayer video;
    ofVideoGrabber capture;
    ofxCvColorImage image;
    ofImage mask;
    ofPoint* dp;

    char titleStr[1024];
    char creditStr[1024];
    char menuStr[1024];
    char menuStrEdit[1024];

	ofxPanel controlPanel;
	ofxFloatSlider duration;
	ofxFloatSlider contrast;
    ofxButton playListClear;
    ofxButton playButton;
    ofxToggle loop;

    ofxPanel moniterPanel;
    ofxToggle editArea;
    ofxButton loadButton;
    ofxButton saveButton;
    ofxButton resetButton;

	ofxPanel deviceList;
    ofxToggle toggles[10];

    ofDirectory directory;
    string path;
    int dirIndex;

    vector<ofFile> playList;
    int current;

    float shutterColor;
    float alpha;
    float fadeStep;

    ofTrueTypeFont font;
    ofTrueTypeFont fontBold;
};
