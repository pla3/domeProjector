//
//  DistortionImage.cpp
//
//  Created by 迎山 和司 on 11/09/16.
//  Copyright 2011 Future University-Hakodate. All rights reserved.
//

#include "DistortionImage.h"

DistortionImage::DistortionImage(){
    mode = DistortionImage::PROJ;
}
DistortionImage::~DistortionImage(){
    //texture.~ofFbo();
    //textureBG.~ofFbo();
}

void DistortionImage::allocate(float w, float h){
    width = w;
    height = h;

    // create texture
    texture.allocate(width, height, GL_RGBA, 0);
	texture.begin();
    ofClear(0,0,0,255);
	texture.end();
    
    reset();
 }

void DistortionImage::draw(){
	draw(0, 0);
}

void DistortionImage::draw(int x, int y){
    ofPushMatrix();
    ofTranslate(x, y);
    
    if(edit){
        texture.begin();
        drawFrame();
        ofFill();
        for(int i=0; i<vertexes.size(); i++){
            ofSetHexColor(0x0000FF);
            ofDrawCircle(vertexes[i].op.x, vertexes[i].op.y, 1.0);
        }
        ofFill();
        ofSetHexColor(0xFF0000);
        ofDrawCircle(uvcenter.x, uvcenter.y, 2.0);
        
        ofNoFill();
        ofSetLineWidth(2.0);
        ofSetHexColor(0xFF0000);
        ofDrawRectangle(uvarea);
        texture.end();
    }
    
    if(mode==DistortionImage::PROJ){
        texture.getTexture().bind();
        glBegin(GL_TRIANGLES);
        for(int i=0; i<triangles.size(); i++){
            for(int j=0; j<3; j++){
                ofPoint op = triangles[i].p[j]->op;
                ofPoint cp = triangles[i].p[j]->cp;
                float b = triangles[i].p[j]->b;
                glColor4f(1.0, 1.0, 1.0, b);
                glTexCoord2f(op.x, op.y);
                glVertex2f(cp.x, cp.y);
            }
        }
        glEnd();
        texture.getTexture().unbind();
        
    } else if(mode==DistortionImage::EDIT_OP){
        texture.getTexture().bind();
        glBegin(GL_TRIANGLES);
        for(int i=0; i<triangles.size(); i++){
            for(int j=0; j<3; j++){
                ofPoint op = triangles[i].p[j]->cp;
                ofPoint cp = triangles[i].p[j]->cp;
                glColor4f(1.0, 1.0, 1.0, 1.0);
                glTexCoord2f(op.x, op.y);
                glVertex2f(cp.x, cp.y);
            }
        }
        glEnd();
        texture.getTexture().unbind();
    }
    
	ofPopMatrix();
}

void DistortionImage::clear(int r, int g, int b, int a){
    texture.begin();
	ofClear(r, g, b, a);
	texture.end();
}

void DistortionImage::makeTriangles(){
    triangles.clear();

    for(int j=0; j<nH-1; j++){
        for(int i=0; i<nW-1; i++){
            int i0 = i+nW*j;
            int i1 = i+nW*(j+1);
            int i2 = (i+1)+nW*j;
            int i3 = (i+1)+nW*(j+1);
            Vertex* v0 = &vertexes[i0];
            Vertex* v1 = &vertexes[i1];
            Vertex* v2 = &vertexes[i2];
            Vertex* v3 = &vertexes[i3];
            
            Triangle t1;
            t1.p[0] = v0;
            t1.p[1] = v1;
            t1.p[2] = v2;
            Triangle t2;
            t2.p[0] = v3;
            t2.p[1] = v2;
            t2.p[2] = v1;
            
            triangles.push_back(t1);
            triangles.push_back(t2);
        }
    }
}

void DistortionImage::loadAnchors(string path){
    ifstream fin(ofToDataPath(path).c_str(), ios::binary);
    
    // error check
    if(!fin){
        ofLog(OF_LOG_ERROR,"DistortionImage::loadAnchors(): cannot open the file...");
        return;
    } else {
        fin >> uvcenter.x;
        fin >> uvcenter.y;
        fin >> uvarea.x;
        fin >> uvarea.y;
        fin >> uvarea.width;
        fin >> uvarea.height;
        cout<<"anchors loaded\n";
    }
    
    fin.close();
}

void DistortionImage::saveAnchors(string path){
    ofstream fout(ofToDataPath(path).c_str(), ios::binary);
    
    // error check
    if(!fout){
        ofLog(OF_LOG_ERROR,"DistortionImage::saveAnchors(): cannot open the file...");
        return;
    } else {
        fout << uvcenter.x;
        fout << " ";
        fout << uvcenter.y;
        fout << " ";
        fout << uvarea.x;
        fout << " ";
        fout << uvarea.y;
        fout << " ";
        fout << uvarea.width;
        fout << " ";
        fout << uvarea.height;
        cout<<"anchors saved\n";
    }
    
    fout.close();
}

void DistortionImage::loadVertexs(){
    vertexes.clear();
    triangles.clear();
    
    dataPath = "standard_16x9.data";
    
	ifstream fin(ofToDataPath(dataPath).c_str(), ios::in);
	string str;
    
	// error check
	if(!fin){
		ofLog(OF_LOG_ERROR,"DistortionImage::loadVertexs(): cannot open the file..."+dataPath);
		return;
	} else {
        fin >> str;
        //cout << 1;
        //cout << ": "+str << endl;
        fin >> nW;
        fin >> nH;
        for(int j=0; j<nH; j++){
            for(int i=0; i<nW; i++){
                Vertex vtx;
                vertexes.push_back(vtx);
            }
		}
	}
    
	fin.close();
    
    updateUVs();
}

void DistortionImage::updateUVs(){
	ifstream fin(ofToDataPath(dataPath).c_str(), ios::in);
	string str;
    
	// error check
	if(!fin){
		ofLog(OF_LOG_ERROR,"DistortionImage::loadVertexs(): cannot open the file..."+dataPath);
		return;
	} else {
        fin >> str;
        //cout << 1;
        //cout << ": "+str << endl;
        fin >> nW;
        fin >> nH;
        for(int j=0; j<nH; j++){
            for(int i=0; i<nW; i++){
                float x, y, u, v, b;
                fin >> x;
                fin >> y;
                fin >> u;
                fin >> v; v = 1.0-v;
                fin >> b;
                Vertex* vtx = &vertexes[i+nW*j];
                vtx->cp.x = (height/2.0)*x+width/2.0;
                vtx->cp.y = -(height/2.0)*y+height/2.0;
                float p1, p2, pt;
                p1 = uvarea.x;
                p2 = uvarea.x+uvarea.width;
                pt = uvcenter.x;
                if(u<=0.5){
                    vtx->op.x = p1+(pt-p1)*(u/0.5);
                } else {
                    vtx->op.x = p2+(pt-p2)*((u-1.0)/-0.5);
                }
                p1 = uvarea.y;
                p2 = uvarea.y+uvarea.height;
                pt = uvcenter.y;
                if(v<=0.5){
                    vtx->op.y = p1+(pt-p1)*(v/0.5);
                } else {
                    vtx->op.y = p2+(pt-p2)*((v-1.0)/-0.5);
                }
                vtx->b = b;
                //printf("node(%2d, %2d): %1.5f, %1.5f, %1.5f, %1.5f, %1.5f\n", i, j, vtx.cp.x, vtx.cp.y, vtx.op.x, vtx.op.y, b);
            }
		}
	}
    
	fin.close();
}

void DistortionImage::reset(){
    uvcenter.x = width/2.0;
    uvcenter.y = height/2.0;
    uvarea.x = (width-height)/2.0;
    uvarea.y = 0;
    uvarea.width = height;
    uvarea.height = height;
    
    loadVertexs();
    makeTriangles();
}

void DistortionImage::drawFrame(){
    int m = 64;
    int n = 10;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            framePoint[i][j].x = (j+1.0)*((height/2)/n)*cos(i*(-2.0*PI/m));
            framePoint[i][j].y = (j+1.0)*((height/2)/n)*sin(i*(-2.0*PI/m));
        }
    }
    for (int j = 0; j < n; j++) {
        framePoint[m][j] = framePoint[0][j];
    }
    
    ofPushMatrix();
    ofTranslate(width/2, height/2);
    ofSetHexColor(0xFFFFFF);
    ofNoFill();
    ofSetLineWidth(1.0);
    for (int i = 0; i < m; i++) {
        ofDrawLine(ofPoint(0,0), framePoint[i][n-1]);
        for (int j = 0; j < n; j++) {
            ofDrawLine(framePoint[i][j], framePoint[i+1][j]);
        }
    }
    ofPopMatrix();
}
