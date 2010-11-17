#pragma once

#include "ofMain.h"
#include "ofxRuiThread.h"

class SimpleThread:public ofxRuiThread
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

    SimpleThread()
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
    }

    void updateThread()
    {
            cam.grabFrame();

            if(cam.isFrameNew())
            {
                colorImg.setFromPixels(cam.getPixels(), 320,240);
                grayImg = colorImg;
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
            }
    }

    void draw()
    {
        ofSetHexColor(0xffffff);
        colorImg.draw(20,20);
        grayImg.draw(360,20);
        grayBg.draw(20,280);
        grayDiff.draw(360,280);
    }

    vector<ofxCvBlob> getBlobs()
    {
        return contourFinder.blobs;
    }
};
