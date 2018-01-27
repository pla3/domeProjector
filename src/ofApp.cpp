//
//  ofApp.cpp for domeProjector
//
//  Created by Kazushi Mukaiyama on 16/12/03.
//  Copyright 2016 Future University Hakodate. All rights reserved.
//
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground(0, 0, 0);
    ofShowCursor();

    font.load("Roboto-Medium.ttf", 9);
    fontBold.load("Roboto-Bold.ttf", 12);

    loadButton.addListener(this, &ofApp::loadButtonPressed);
    saveButton.addListener(this, &ofApp::saveButtonPressed);
    resetButton.addListener(this, &ofApp::resetButtonPressed);
    playListClear.addListener(this, &ofApp::playListClearPressed);
    playButton.addListener(this, &ofApp::playButtonPressed);
    for(int i=0; i<10; i++){
        toggles[i].addListener(this, &ofApp::deviceValueChanged);
    }

    moniterPanel.setup("moniter");
    moniterPanel.loadFont("Roboto-Medium.ttf", 12);
    moniterPanel.setPosition(10, 600);
    moniterPanel.add(editArea.setup("edit area", false));
    moniterPanel.add(loadButton.setup("load"));
    moniterPanel.add(saveButton.setup("save"));
    moniterPanel.add(resetButton.setup("reset"));

	controlPanel.setup("video control");
    controlPanel.loadFont("Roboto-Medium.ttf", 12);
    controlPanel.setPosition(290, 600);
    controlPanel.add(playButton.setup("play/stop"));
	controlPanel.add(duration.setup("duration", 0.0, 0.0, 1.0));
    controlPanel.add(contrast.setup("contrast", 0.0, 0.0, 1.0));
    controlPanel.add(playListClear.setup("clear playlist"));
    controlPanel.add(loop.setup("loop", false));

    captureMode = false;
    deviceList.setup("device list");
    deviceList.loadFont("Roboto-Medium.ttf", 12);
    deviceList.setPosition(570, 600);
    deviceList.clear();
    vector<ofVideoDevice> devices = capture.listDevices();
    if(devices.size()==0){
        captureMode = false;
    }
    for(int i = 0; i < devices.size(); i++){
        cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
        if(i<10){
            deviceList.add(toggles[i].setup(devices[i].deviceName, false));
        }
    }

    dp = NULL; // the anchor drag point

    projector.allocate(PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
    projector.clear(0, 0, 0, 255);
    projector.mode = DistortionImage::PROJ;

    video.load("planetarium_autumn.mov");
    video.setLoopState(OF_LOOP_NONE);
    video.stop();
    image.allocate(video.getWidth(), video.getHeight());
    mask.allocate(projector.width, projector.height, OF_IMAGE_COLOR_ALPHA);
    mask.load("mask.png");

    path = ofFilePath::getUserHomeDir();
    directory.allowExt("mov");
	directory.allowExt("mp4");
    directory.allowExt("");
	directory.listDir(path);
	directory.sort();
    dirIndex = 0;

    playList.clear();
    current = 0;

    string title;
	title += "Dome Projector V2\n";
	sprintf(titleStr, "%s", title.c_str());
    string credit;
    credit += "programmed by Kazushi Mukaiyama, deformation data by Paul Bourke, 2014-2018\n";
    sprintf(creditStr, "%s", credit.c_str());
	string menu;
	menu += "+press SPACE to play/stop a video\n";
	menu += "+press ENTER to set a video into the playlist\n";
	menu += "+press ALLOW UP to select a video\n";
	menu += "+press ALLOW DOWN to select a video\n";
	menu += "+press 'O' to show white screen\n";
	menu += "+press 'P' to show black screen\n";
	sprintf(menuStr, "%s", menu.c_str());
    menu = "+press ALLOW UP to tranlsate projection area up\n";
    menu += "+press ALLOW DOWN to tranlsate projection area down\n";
    menu += "+press ALLOW LEFT to tranlsate projection area left\n";
    menu += "+press ALLOW RIGHT to tranlsate projection area right\n";
    menu += "+press CTL+ALLOW UP to scale projection area up\n";
    menu += "+press CTL+ALLOW DOWN to scale projection area down\n";
    menu += "+press SHIFT+ALLOW UP to move projection center up\n";
    menu += "+press SHIFT+ALLOW DOWN to move projection center down\n";
    menu += "+press SHIFT+ALLOW LEFT to move projection center left\n";
    menu += "+press SHIFT+ALLOW RIGHT to move projection center right\n";
    menu += "+press 'J' to reset adjust points\n";
    menu += "+press 'K' to save adjust points\n";
    menu += "+press 'L' to load adjust points\n";
    sprintf(menuStrEdit, "%s", menu.c_str());

    alpha = 0;
    fadeStep = -12.0;
    
    screenInit = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(screenInit){
        int w=0, h=0;
        w = ofGetScreenWidth();
        h = ofGetScreenHeight();
        ofSetWindowShape(w+PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
        screenInit = false;
        return;
    }
    
    projector.edit = editArea;
    
    // check playlist of video
    if(video.isPlaying() && video.getCurrentFrame()>=video.getTotalNumFrames()-1){
        current++;
        if(current>playList.size()-1){
            if(loop){
                current = 0;
            }
        }

        if(current<playList.size()){
            video.closeMovie();
            video.load(playList[current].path());
            image.allocate(video.getWidth(), video.getHeight());
            duration = video.getPosition();
            video.setPosition(duration);
            video.play();
        } else {
            video.setPaused(true);
            video.stop();
            duration = video.getPosition();
        }
    }

    // update an image of caputure or video
    if(captureMode && capture.isInitialized()){
        capture.update();
        image.setFromPixels(capture.getPixels());
    } else {
        video.update();
        image.setFromPixels(video.getPixels());
    }

    image.convertToRange(-128.0*contrast, 255.0);
    
    projector.texture.begin();
    //ofSetHexColor(0x000000);
    //ofFill();
    //ofRect(0, 0, projector.width, projector.height);
	ofSetHexColor(0xFFFFFF);
    float vscale = projector.height/image.height;
    image.draw((projector.width-image.width*vscale)/2.0,0,image.width*vscale,image.height*vscale);
    mask.draw(0, 0);
    //ofSetHexColor(0x000000);
    //ofNoFill();
    //ofSetLineWidth(2.0);
    //ofSetCircleResolution(100);
    //ofCircle(projector.width/2.0, projector.height/2.0, projector.height/2.0);
    projector.texture.end();

    alpha += fadeStep;
    if(alpha>255.0){alpha=255.0;}else if(alpha<0.0){alpha=0.0;}
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    
    // original moniter
    ofPushMatrix();
    ofTranslate(10, 40);
    ofScale(0.5, 0.5);
    projector.mode = DistortionImage::EDIT_OP;
    ofSetHexColor(0xFFFFFF);
    projector.draw(0,0);
    ofNoFill();
    ofSetLineWidth(1.0);
    ofDrawRectangle(0, 0, projector.width, projector.height);
    
    ofPopMatrix();

    int offsetX = 1000;
    int offsetY = 50;
    int stepY = 24;
    // play list
    ofSetColor(ofColor::white);
    font.drawString("playlist:", offsetX, 0*stepY+offsetY);
    if(loop){ font.drawString("loop", offsetX+80, 0*stepY+offsetY); }
	for(int i = 0; i < (int)playList.size(); i++){
        ofFile file = playList[i];
		string filename = ofToString(i+1)+": "+file.getFileName();
        if(i==current){
            ofSetColor(ofColor::blue);
        } else {
            ofSetColor(ofColor::white);
        }
        font.drawString(filename, offsetX, (i+1)*stepY+offsetY);
	}

    // mask
    ofSetHexColor(0x000000);
    ofFill();
    ofDrawRectangle(offsetX+180, 0, ofGetScreenWidth()-(offsetX+180), PROJECTOR_HEIGHT);

    // directory list
    ofSetColor(ofColor::white);
    font.drawString("directory:", offsetX+200, 0*stepY+offsetY);
	for(int i = 0; i < (int)directory.size()+1; i++){
        if(i==dirIndex){
            ofSetColor(ofColor::red);
        } else {
            ofSetColor(ofColor::white);
        }
        if(i<directory.size()){
            ofFile file = directory.getFile(i);
            string filename;
            if(file.isDirectory()){
                filename = "(dir) ";
            }
            filename += file.getFileName();
            font.drawString(filename, offsetX+200, (i+2)*stepY+offsetY);
        } else {
            font.drawString("..", offsetX+200, 1*stepY+offsetY);
        }
	}

    // mask
    ofSetHexColor(0x000000);
    ofFill();
    ofDrawRectangle(offsetX+580, 0, ofGetScreenWidth()+PROJECTOR_WIDTH, PROJECTOR_HEIGHT);

    // gui
    controlPanel.draw();
    moniterPanel.draw();
    deviceList.draw();
    if(video.isPlaying()){
        duration = video.getPosition();
    } else {
        video.setPosition(duration);
    }
    
    // help & credits
    ofSetColor(ofColor::white);
    fontBold.drawString(titleStr, 10, 20);
    font.drawString(creditStr, 200, 20);
    ofSetColor(ofColor::gray);
    if(editArea){
        font.drawString(menuStrEdit, 10, 735);
    } else {
        font.drawString(menuStr, 10, 735);
    }

    // projector screen
    projector.mode = DistortionImage::PROJ;
    ofSetColor(ofColor::black);
    ofFill();
    ofDrawRectangle(ofGetScreenWidth(), 0, projector.width, projector.height);
	ofSetHexColor(0xFFFFFF);
    projector.draw(ofGetScreenWidth(), 0);

    // projector shutter
    ofSetColor(shutterColor,shutterColor,shutterColor,alpha);
    ofFill();
    ofDrawRectangle(ofGetScreenWidth(), 0, projector.width, projector.height);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //printf("key: %d\n", key);

    if(editArea){
        switch(key){
            case 'O':
            case 'o':
                shutterColor = 255;
                fadeStep *= -1.0;
                break;
            case 'P':
            case 'p':
                shutterColor = 0;
                fadeStep *= -1.0;
                break;
            case OF_KEY_UP:
                if(editArea){
                    if(ofGetKeyPressed(OF_KEY_CONTROL)){
                        // scale up
                        float cx = projector.uvarea.x+projector.uvarea.width/2.0;
                        float cy = projector.uvarea.y+projector.uvarea.height/2.0;
                        projector.uvarea.width += 5.0;
                        projector.uvarea.height += 5.0;
                        projector.uvarea.x = cx-projector.uvarea.width/2.0;
                        projector.uvarea.y = cy-projector.uvarea.height/2.0;
                        projector.updateUVs();
                    } else if(ofGetKeyPressed(OF_KEY_SHIFT)){
                        // move center up
                        projector.uvcenter.y -= 1.0;
                        projector.updateUVs();
                    } else {
                        // translate up
                        projector.uvcenter.y -= 1.0;
                        projector.uvarea.y -= 1.0;
                        projector.updateUVs();
                    }
                }
                break;
            case OF_KEY_DOWN:
                if(editArea){
                    if(ofGetKeyPressed(OF_KEY_CONTROL)){
                        // scale down
                        float cx = projector.uvarea.x+projector.uvarea.width/2.0;
                        float cy = projector.uvarea.y+projector.uvarea.height/2.0;
                        projector.uvarea.width -= 5.0;
                        projector.uvarea.height -= 5.0;
                        projector.uvarea.x = cx-projector.uvarea.width/2.0;
                        projector.uvarea.y = cy-projector.uvarea.height/2.0;
                        projector.updateUVs();
                    } else if(ofGetKeyPressed(OF_KEY_SHIFT)){
                        // move center down
                        projector.uvcenter.y += 1.0;
                        projector.updateUVs();
                    } else {
                        // translate down
                        projector.uvcenter.y += 1.0;
                        projector.uvarea.y += 1.0;
                        projector.updateUVs();
                    }
                }
                break;
            case OF_KEY_LEFT: // translate left
                if(editArea){
                    if(ofGetKeyPressed(OF_KEY_CONTROL)){
                        // scale down
                        float cx = projector.uvarea.x+projector.uvarea.width/2.0;
                        float cy = projector.uvarea.y+projector.uvarea.height/2.0;
                        projector.uvarea.width -= 5.0;
                        projector.uvarea.height -= 5.0;
                        projector.uvarea.x = cx-projector.uvarea.width/2.0;
                        projector.uvarea.y = cy-projector.uvarea.height/2.0;
                        projector.updateUVs();
                    } else if(ofGetKeyPressed(OF_KEY_SHIFT)){
                        // move center left
                        projector.uvcenter.x -= 1.0;
                        projector.updateUVs();
                    } else {
                        // translate left
                        projector.uvcenter.x -= 1.0;
                        projector.uvarea.x -= 1.0;
                        projector.updateUVs();
                    }
                }
                break;
            case OF_KEY_RIGHT: // translate right
                if(editArea){
                    if(ofGetKeyPressed(OF_KEY_CONTROL)){
                        // scale up
                        float cx = projector.uvarea.x+projector.uvarea.width/2.0;
                        float cy = projector.uvarea.y+projector.uvarea.height/2.0;
                        projector.uvarea.width += 5.0;
                        projector.uvarea.height += 5.0;
                        projector.uvarea.x = cx-projector.uvarea.width/2.0;
                        projector.uvarea.y = cy-projector.uvarea.height/2.0;
                        projector.updateUVs();
                    } else if(ofGetKeyPressed(OF_KEY_SHIFT)){
                        // move center right
                        projector.uvcenter.x += 1.0;
                        projector.updateUVs();
                    } else {
                        // translate right
                        projector.uvcenter.x += 1.0;
                        projector.uvarea.x += 1.0;
                        projector.updateUVs();
                    }
                }
                break;
            case 'K': // save adjust projection data
            case 'k':
                projector.saveAnchors("adjust.data");
                break;
            case 'L': // load adjust projection data
            case 'l':
                projector.loadAnchors("adjust.data");
                projector.updateUVs();
                break;
            case 'J': // reset adjust projection data
            case 'j':
                projector.reset();
                break;
            default:
                break;
        }
    } else {
        switch(key){
            case ' ':
                if(video.isPlaying()){
                    video.stop();
                } else {
                    video.play();
                }
                break;
            case OF_KEY_RETURN:
                if(!video.isPlaying()){
                    if(dirIndex<directory.size()){
                        ofFile file = directory.getFile(dirIndex);
                        if (file.exists()){
                            if(file.isDirectory()){
                                path = file.path();
                                directory.listDir(path);
                                directory.sort();
                                dirIndex = 0;
                            } else {
                                ofLogVerbose("The file exists - now checking the type via file extension");
                                string fileExtension = ofToUpper(file.getExtension());
                                if (fileExtension == "MOV" || fileExtension == "MP4") {
                                    captureMode = false;
                                    projector.clear(255,255,255,255);
                                    playList.push_back(file);
                                    current = 0;
                                    video.closeMovie();
                                    video.load(playList[current].path());
                                    image.allocate(video.getWidth(), video.getHeight());
                                    duration = video.getPosition();
                                } else {
                                    ofLogVerbose("This selected file is not movie file!");
                                }
                            }
                        }
                    } else {
                        // ".." parent
                        path = ofFilePath::getEnclosingDirectory(path);
                        directory.listDir(path);
                        directory.sort();
                        dirIndex = 0;
                    }
                }
                break;
            case OF_KEY_DOWN:
                if(!video.isPlaying()){
                    dirIndex++;
                    if(dirIndex>directory.size()) {dirIndex = 0;}
                }
                break;
            case OF_KEY_UP:
                if(!video.isPlaying()){
                    dirIndex--;
                    if(dirIndex<0) {dirIndex = directory.size();}
                }
                break;
            case 'O':
            case 'o':
                shutterColor = 255;
                fadeStep *= -1.0;
                break;
            case 'P':
            case 'p':
                shutterColor = 0;
                fadeStep *= -1.0;
                break;
            default:
                break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    /*
    if(editArea){
        int mx = x; int my = y;
        mx = (mx-10)*2; my = (my-40)*2;
        int sx = projector.uvcenter.x;
        int sy = projector.uvcenter.y;
        if(dp!=NULL){
            dp->set(mx, my);
        } else {
            int cx = projector.uvarea.getCenter().x;
            int cy = projector.uvarea.getCenter().y;
            projector.uvarea.scaleFromCenter(
                (fabs((float)(mx-cx))*2.0)/projector.uvarea.width,
                (fabs((float)(my-cy))*2.0)/projector.uvarea.height
            );
        }
    }
     */
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    /*
    if(editArea){
        int mx = x; int my = y;
        mx = (mx-10)*2; my = (my-40)*2;
        printf("mx: %d, my: %d\n", mx, my);
		int area = 10;
        int sx = projector.uvcenter.x;
        int sy = projector.uvcenter.y;
        printf("sx: %d, sy: %d\n", sx, sy);
        if(sx-area < mx && mx < sx+area && sy-area < my && my < sy+area){
            printf("hit uvcenter\n");
            dp = &projector.uvcenter;
        } else {
            int cx = projector.uvarea.getCenter().x;
            int cy = projector.uvarea.getCenter().y;
            projector.uvarea.scaleFromCenter(
                (fabs((float)(mx-cx))*2.0)/projector.uvarea.width,
                (fabs((float)(my-cy))*2.0)/projector.uvarea.height
            );
        }
    }
     */
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    /*
    dp = NULL;
    if(editArea){
        projector.updateUVs();
    }
     */
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::loadButtonPressed(){
    projector.loadAnchors("adjust.data");
    projector.updateUVs();
}

void ofApp::saveButtonPressed(){
    projector.saveAnchors("adjust.data");
}

void ofApp::resetButtonPressed(){
    projector.reset();
}

void ofApp::playListClearPressed(){
    playList.clear();
}

void ofApp::playButtonPressed(){
    if(video.isPlaying()){
        video.stop();
    } else {
        video.play();
    }
}

void ofApp::deviceValueChanged(bool & value){
    // initialize captute
    if(value && !capture.isInitialized()){
        for(int i=0; i<10; i++){
            if(toggles[i]){
                capture.setDeviceID(i);
                capture.setDesiredFrameRate(60);
                capture.initGrabber(1920, 1080);
                image.allocate(capture.getWidth(), capture.getHeight());
                captureMode = true;
            }
        }
    } else if(!value){
        capture.close();
        captureMode = false;
    }
}

