#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);
    threshold = 80;

    thread_1.cam.setVerbose(true);
    thread_1.cam.setDesiredFrameRate(125);
    thread_1.cam.initGrabber(320,240);
    thread_1.cam.setUseTexture(false);

    thread_1.initAndSleep();
    //thread_1.start();
    //thread_2.start();

    ID = 0;
}

void testApp::trackBlobs(vector<ofxCvBlob> _blobs)
{

    ofxVec2f _b;
    ofxVec2f b;
    for (int i = 0; i < _blobs.size(); i++)
    {
        bool bIsNewBlob = true;
        for (int j = 0; j < blobs.size(); j++)
        {
            _b.set( _blobs[i].centroid );
            b.set(blobs[j].x, blobs[j].y);
            if(b.distance(_b) < 30)
            {
                bIsNewBlob = false;
                blobs[i].frame = ofGetFrameNum();
                blobs[i].x = _b.x;
                blobs[i].y = _b.y;
                /*blobs[i].pX = blobs[i].x;
                blobs[i].pY = blobs[i].y;
                blobs[i].nPts = _blobs[i].nPts;
                blobs[i].pts = _blobs[i].pts;
                blobs[i].boundingRect.x           = _blobs[i].boundingRect.x;
                blobs[i].boundingRect.y           = _blobs[i].boundingRect.y;
                blobs[i].boundingRect.width       = _blobs[i].boundingRect.width;
                blobs[i].boundingRect.height      = _blobs[i].boundingRect.height;*/
            }

        }

        if(bIsNewBlob)
        {
            trackedBlob tB;
            tB.ID = ID;
            ID++;
            tB.state = UPCOMING;
            tB.frame = ofGetFrameNum();
            tB.x = _blobs[i].centroid.x;
            tB.y = _blobs[i].centroid.y;
            /*tB.pX = _blobs[i].centroid.x;
            tB.pY = _blobs[i].centroid.y;
            tB.nPts = _blobs[i].nPts;
            tB.pts = _blobs[i].pts;
            tB.boundingRect.x           = _blobs[i].boundingRect.x;
            tB.boundingRect.y           = _blobs[i].boundingRect.y;
            tB.boundingRect.width       = _blobs[i].boundingRect.width;
            tB.boundingRect.height      = _blobs[i].boundingRect.height;*/
            tB.framesAlive = 0;
            tB.alpha = 0;
            blobs.push_back(tB);
        }
    }

    //finally kill all blobs which weren't updated the last 50 frames
    for (int j = 0; j < blobs.size(); j++)
    {
        if(blobs[j].frame < ofGetFrameNum() - 50)
        {
            blobs.erase(blobs.begin() + j);
        }
    }

    for (int j = 0; j < blobs.size(); j++)
    {

        //if blob comes up, fade it in
        if(blobs[j].state == UPCOMING)
        {
            blobs[j].alpha = (blobs[j].framesAlive - 10) * 5;
        }

        //set state to alive, if they live longer then 10 frames
        //set alpha to 255
        if(blobs[j].framesAlive > 30)
        {
            blobs[j].state = ALIVE;
            blobs[j].alpha = 255;
        }

        //set state dying which weren't updated since the last 5 frames
        //fade alpha out
        if(blobs[j].frame < ofGetFrameNum() - 2)
        {
            blobs[j].state = DYING;
            blobs[j].alpha = -6 * (ofGetFrameNum() - blobs[j].frame) + blobs[j].alpha;
        }

        //count frames alive
        if(blobs[j].state != DYING)
        {
            blobs[j].framesAlive++;
        }

        //bound alpha to 0..255
        if (blobs[j].alpha > 255)
        {
            blobs[j].alpha = 255;
        }
        if (blobs[j].alpha < 0)
        {
            blobs[j].alpha = 0;
        }
    }
}

//--------------------------------------------------------------
void testApp::update()
{
    ofBackground(100,100,100);
    thread_1.updateOnce();
    blobs_1 = thread_1.getBlobs();
    trackBlobs(blobs_1);
}

//--------------------------------------------------------------
void testApp::draw()
{

    thread_1.draw();

    // then draw the contours:

    ofFill();
    ofSetHexColor(0x333333);
    ofRect(360,540,320,240);
    ofRect(1040,540,320,240);
    ofSetHexColor(0xffffff);

    ofTranslate(360,520);
    for (int i = 0; i < blobs.size(); i++)
    {
        // pos += (targetPos - pos) * SPEED;

        ofSetHexColor(0xdd00cc);

        ofNoFill();
        for( int i=0; i<(int)blobs.size(); i++ )
        {
            ofRect( blobs[i].boundingRect.x, blobs[i].boundingRect.y,
                    blobs[i].boundingRect.width, blobs[i].boundingRect.height );
        }

        ofSetHexColor(0x00ffff);

        for( int i=0; i<(int)blobs.size(); i++ )
        {
            ofNoFill();
            ofBeginShape();
            for( int j=0; j<blobs[i].nPts; j++ )
            {
                ofVertex( blobs[i].pts[j].x, blobs[i].pts[j].y );
            }
            ofEndShape();

        }

        ofFill();
        ofEnableAlphaBlending();
        ofSetColor(255,255,255,blobs[i].alpha);
        ofLine(blobs[i].x,blobs[i].y,blobs[i].pX,blobs[i].pY);
        ofCircle(blobs[i].x,blobs[i].y,5);

        ofDisableAlphaBlending();
    }
    ofTranslate(-360,-520);

    // finally, a report:

    ofSetHexColor(0xffffff);
    char reportStr[1024];
    sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\ntracked Blob count: %i\nfps: %f", threshold, blobs.size(), ofGetFrameRate());
    ofDrawBitmapString(reportStr, 20, 600);
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{

    	switch (key){
    		case ' ':
    			thread_1.bLearnBackground = true;
    			break;
    		case '+':
    			threshold ++;
    			if (threshold > 255) threshold = 255;
    			break;
    		case '-':
    			threshold --;
    			if (threshold < 0) threshold = 0;
    			break;
    	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

void testApp::exit() {
    blobs_1 = vector<ofPoint>();
    thread_1.stop();
}

