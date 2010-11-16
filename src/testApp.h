#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "SimpleThread.h"
#include "ofxVectorMath.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file


struct trackedBlob
{
    int frame;
    int framesAlive;
    float height;
    int ID;
    float pX;
    float pY;
    float width;
    float x;
    float y;
};

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void trackBlobs(vector<ofxCvBlob> _blobs);

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        ofVideoPlayer 		cam_1, cam_2;

        ofxCvColorImage		colorImg_1, colorImg_2;

        ofxCvGrayscaleImage 	grayImg_1, grayImg_2;
		ofxCvGrayscaleImage 	grayBg_1, grayBg_2;
		ofxCvGrayscaleImage 	grayDiff_1, grayDiff_2;

		int 				threshold;
		bool				bLearnBakground;

		SimpleThread thread_1, thread_2;

		vector<ofxCvBlob> blobs_1, blobs_2;
		vector<trackedBlob> blobs;
		int                 ID;
};

#endif
