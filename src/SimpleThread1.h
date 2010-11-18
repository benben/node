#pragma once

#include "ofMain.h"
#include "ofxRuiThread.h"

class SimpleThread1:public ofxRuiThread
{
public:
    ofVideoGrabber cam;

    ofxCvColorImage		colorImg;

    ofxCvGrayscaleImage 	grayImg;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;

    int 				threshold;
    bool				bLearnBackground;
    ofxCvContourFinder 	contourFinder;

    ofPoint             warpPoints[4];

    SimpleThread1()
    {
        //cam.initGrabber(640,480);
        //cam.setUseTexture(false);
        //colorImg.setUseTexture(false);
        colorImg.allocate(320,240);
        //grayImg.setUseTexture(false);
        grayImg.allocate(320,240);
        //grayBg.setUseTexture(false);
        grayBg.allocate(320,240);
        //grayDiff.setUseTexture(false);
        grayDiff.allocate(320,240);
        bLearnBackground = true;
        threshold = 80;

        warpPoints[0].x = 0;
        warpPoints[0].y = 0;
        warpPoints[1].x = 320;
        warpPoints[1].y = 0;
        warpPoints[2].x = 320;
        warpPoints[2].y = 240;
        warpPoints[3].x = 0;
        warpPoints[3].y = 240;
    }

    void updateThread()
    {
            //cam.idleMovie();
            cam.grabFrame();

            if(cam.isFrameNew())
            {
                colorImg.setFromPixels(cam.getPixels(), 320,240);
                grayImg = colorImg;
                grayImg.warpPerspective(warpPoints[0],warpPoints[1],warpPoints[2],warpPoints[3]);
                if (bLearnBackground == true)
                {
                    grayBg = grayImg;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
                    bLearnBackground = false;
                }

                // take the abs value of the difference between background and incoming and then threshold:
                grayDiff.absDiff(grayBg, grayImg);
                grayDiff.threshold(threshold);

                // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
                // also, find holes is set to true so we will get interior contours as well....
                //contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
                contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, false);
                    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].centroid.x += 320;
        contourFinder.blobs[i].boundingRect.x += 320;
        for (int j = 0; j < contourFinder.blobs[i].nPts; j++){
            contourFinder.blobs[i].pts[j].x += 320;
        }
    }
            }
    }

    void draw()
    {
        ofSetHexColor(0xffffff);
        colorImg.draw(360,10,160,120);
        grayImg.draw(530,10,160,120);
        grayBg.draw(360,140,160,120);
        grayDiff.draw(530,140,160,120);
    }

    vector<ofxCvBlob> getBlobs()
    {
        return contourFinder.blobs;
    }

    void setThreshold(int _t){
        threshold = _t;
    }
};
