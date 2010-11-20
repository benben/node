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

    ofPoint             warpPoints[4];

    int                 offset;

    SimpleThread()
    {
        colorImg.allocate(320,240);
        grayImg.allocate(320,240);
        grayBg.allocate(320,240);
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

        offset = 0;
    }

    void updateThread()
    {
        cam.grabFrame();

        if(cam.isFrameNew())
        {
            colorImg.setFromPixels(cam.getPixels(), 320,240);
            grayImg = colorImg;
            grayImg.warpPerspective(warpPoints[0],warpPoints[1],warpPoints[2],warpPoints[3]);

            grayImg.erode();
            grayImg.dilate();

            if (bLearnBackground == true)
            {
                grayBg = grayImg;
                bLearnBackground = false;
            }

            grayDiff.absDiff(grayBg, grayImg);
            grayDiff.threshold(threshold);

            contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, false);
            if(offset != 0)
            {
                for (int i = 0; i < contourFinder.nBlobs; i++)
                {
                    contourFinder.blobs[i].centroid.x += offset;
                    contourFinder.blobs[i].boundingRect.x += offset;
                    for (int j = 0; j < contourFinder.blobs[i].nPts; j++)
                    {
                        contourFinder.blobs[i].pts[j].x += offset;
                    }
                }
            }

        }
    }

    vector<ofxCvBlob> getBlobs()
    {
        return contourFinder.blobs;
    }

    void setThreshold(int _t)
    {
        threshold = _t;
    }
};
