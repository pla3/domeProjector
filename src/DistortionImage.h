//
//  DistortionImage.h
//  drawingCanvas
//
//  Created by Kazushi Mukaiyama on 13/10/12.
//  Copyright 2011 Future University Hakodate. All rights reserved.
//
#pragma once

#include "ofMain.h"

#define RIM 36
#define LAYER 9

class DistortionImage {
public:
	enum Mode
	{
		EDIT_OP,
		PROJ,
        SETUP
	};

    struct Vertex{
        ofPoint op; // original point
        ofPoint cp; // current point
        float b; // current point
    };
    struct Triangle{
        Vertex *p[3];
    };
    
    float width;
    float height;
    ofPoint uvcenter;
    ofRectangle uvarea;
    
	Mode mode;
    bool edit;
    
    ofFbo texture;
    vector<Vertex> vertexes;
    
    DistortionImage();
    virtual ~DistortionImage();
    
    void allocate(float w, float h);
    void draw();
    void draw(int x, int y);
    void clear(int r, int g, int b, int a);

    void loadAnchors(string path);
    void saveAnchors(string path);
    void loadVertexs();
    void makeTriangles();
    void updateUVs();
    
    void reset();
    
private:
    string dataPath;
    int nW, nH;
    vector<Triangle> triangles;
    void drawFrame();
    ofPoint framePoint[65][10];
    
};
