#pragma once

#include "ofMain.h"
#include "ofxRuiThread.h"

class SimpleThread:public ofxRuiThread{
public:
	int count;
	ofxCvGrayscaleImage grayDiff;
	ofxCvContourFinder 	contourFinder;
	bool bhasNewImage;

	SimpleThread(){
        grayDiff.setUseTexture(false);
        grayDiff.allocate(320,240);
        bhasNewImage = false;
	}

	void updateThread(){
	    if(bhasNewImage) {
	        contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);
	        bhasNewImage = false;
	    }
	}

	void setImage(ofxCvGrayscaleImage _grayDiff) {
        grayDiff = _grayDiff;
        bhasNewImage = true;
	}

	vector<ofxCvBlob> getBlobs() {
	    return contourFinder.blobs;
	}
};
