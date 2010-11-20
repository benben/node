#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "SimpleThread.h"
#include "SimpleThread1.h"
#include "ofxVectorMath.h"
#include "renderManager.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file


enum trackedBlobState { UPCOMING, ALIVE, DYING };

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
    int nPts;
    vector <ofPoint>    pts;
    ofRectangle         boundingRect;
    trackedBlobState    state;
    int alpha;
};

class testApp : public ofBaseApp
{

public:

    void setup();
    void update();
    void draw();
    void exit();
    void trackBlobs(vector<ofxCvBlob> _blobs);

    void keyPressed  (int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);

    void drawInputs();
    void drawDebugTable();

    int 				threshold;
    bool				bLearnBackground;

    SimpleThread thread_1, thread_2;

    vector<trackedBlob> blobs;
    int                 ID;

    //RM

    renderManager rm;

    ofRectangle guiIn;
    ofRectangle guiOut;

    int debugOutput;
    bool debug;
    ofImage twoScreenImage;
};

#endif
