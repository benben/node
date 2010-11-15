#pragma once

#include "ofMain.h"
#include "ofxRuiThread.h"

class SimpleThread:public ofxRuiThread{
public:
	int count;
	ofxCvGrayscaleImage grayDiff;
	ofxCvContourFinder 	contourFinder;
	bool bhasNewImage;
	bool bhasFinishedFinding;

	SimpleThread(){
        grayDiff.setUseTexture(false);
        grayDiff.allocate(320,240);
        bhasNewImage = false;
        bhasFinishedFinding = false;
	}

	void updateThread(){
	    if(bhasNewImage) {
	        contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);
	        bhasFinishedFinding = true;
	        bhasNewImage = false;
	    }
	}

	void setImage(ofxCvGrayscaleImage _grayDiff) {
        grayDiff = _grayDiff;
        //bhasFinishedFinding = false;
        bhasNewImage = true;
	}

	vector<ofxCvBlob> getBlobs() {
	    return contourFinder.blobs;
	}
};
