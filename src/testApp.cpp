#include "testApp.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);
    threshold = 80;

    thread_1.cam.setVerbose(true);
    thread_1.cam.setDesiredFrameRate(60);
    thread_1.cam.setDeviceID(1);
    thread_1.cam.initGrabber(320,240);
    thread_1.cam.setUseTexture(false);

    /*thread_2.cam.loadMovie("fingers.mov");
    thread_2.cam.play();
    thread_2.cam.setUseTexture(false);*/

    thread_2.cam.setVerbose(true);
    thread_2.cam.setDesiredFrameRate(60);
    thread_2.cam.setDeviceID(0);
    thread_2.cam.initGrabber(320,240);
    thread_2.cam.setUseTexture(false);

    thread_1.initAndSleep();
    thread_2.initAndSleep();

    ID = 0;

    //RM
    rm.allocateForNScreens(2, 1024, 768);
    rm.loadFromXml("fboSettings.xml");

    guiIn   = ofRectangle(320, 295, 500, 178);
    guiOut  = ofRectangle(guiIn.x + guiIn.width + 30, guiIn.y, 500, 178);

    twoScreenImage.loadImage("adam.jpg");
    toggleDebugOutput = true;

    //GUI

    //gui.config->gridSize.set(340,0,0);
    gui.addTitle("1");
    //gui.config->gridSize.set(340,0,0);
    gui.addQuadWarper("Cam 1", thread_1.colorImg, thread_1.warpPoints);
    gui.addTitle("-").newColumn = true;
    gui.addTitle("2").newColumn = true;
    gui.addQuadWarper("Cam 2", thread_2.colorImg, thread_2.warpPoints);
    gui.loadFromXML();
    gui.show();
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
            if(b.distance(_b) < 10)
            {
                bIsNewBlob = false;
                blobs[j].frame = ofGetFrameNum();
                blobs[j].pX = blobs[j].x;
                blobs[j].pY = blobs[j].y;
                blobs[j].x = _blobs[i].centroid.x;
                blobs[j].y = _blobs[i].centroid.y;
                blobs[j].nPts = _blobs[i].nPts;
                blobs[j].pts = _blobs[i].pts;
                blobs[j].boundingRect.x           = _blobs[i].boundingRect.x;
                blobs[j].boundingRect.y           = _blobs[i].boundingRect.y;
                blobs[j].boundingRect.width       = _blobs[i].boundingRect.width;
                blobs[j].boundingRect.height      = _blobs[i].boundingRect.height;
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
            tB.pX = _blobs[i].centroid.x;
            tB.pY = _blobs[i].centroid.y;
            tB.nPts = _blobs[i].nPts;
            tB.pts = _blobs[i].pts;
            tB.boundingRect.x           = _blobs[i].boundingRect.x;
            tB.boundingRect.y           = _blobs[i].boundingRect.y;
            tB.boundingRect.width       = _blobs[i].boundingRect.width;
            tB.boundingRect.height      = _blobs[i].boundingRect.height;
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

    if(bLearnBackground) {
        thread_1.bLearnBackground = true;
        thread_2.bLearnBackground = true;
        bLearnBackground = false;
    }

    thread_1.setThreshold(threshold);
    thread_2.setThreshold(threshold);

    thread_1.updateOnce();
    blobs_1 = thread_1.getBlobs();
    trackBlobs(blobs_1);

    thread_2.updateOnce();
    blobs_2 = thread_2.getBlobs();
    trackBlobs(blobs_2);

    rm.myOffscreenTexture.clear();
}

//--------------------------------------------------------------
void testApp::draw()
{
    thread_1.draw();
    thread_2.draw();

    // then draw the contours:

    ofFill();
    ofSetHexColor(0x333333);
    //ofRect(360,540,320,240);
    //ofRect(680,540,320,240);
    ofSetHexColor(0xffffff);

    //ofTranslate(320,295);
    //rm.myOffscreenTexture.clear((float)0.0,(float)0.0,(float)0.0,(float)0.0);

    rm.startOffscreenDraw();
    ofPushMatrix();
    glScalef(1024/640,768/240,0);
    if( toggleDebugOutput )
    {
        for (int i = 0; i < blobs.size(); i++)
        {
            // pos += (targetPos - pos) * SPEED;

            ofSetHexColor(0xdd00cc);

            ofNoFill();
            ofRect( blobs[i].boundingRect.x, blobs[i].boundingRect.y,
                    blobs[i].boundingRect.width, blobs[i].boundingRect.height );

            ofSetHexColor(0x00ffff);

            ofNoFill();
            ofBeginShape();
            for( int j=0; j<blobs[i].nPts; j++ )
            {
                ofVertex( blobs[i].pts[j].x, blobs[i].pts[j].y );
            }
            ofEndShape();


            ofFill();
            ofEnableAlphaBlending();
            ofSetColor(255,255,255,blobs[i].alpha);
            ofLine(blobs[i].x,blobs[i].y,blobs[i].pX,blobs[i].pY);
            float size = sqrt( pow(blobs[i].boundingRect.width,2) + pow(blobs[i].boundingRect.height,2) ) / 2 * 0.8;
            ofCircle(blobs[i].x,blobs[i].y,size);

            ofDisableAlphaBlending();

        }
    }
    else
    {
        ofSetHexColor(0x323232);
        ofRect(0, 0, rm.width, rm.height);

        ofSetHexColor(0xFF0000);
        ofRect(0, 0, 100, 768);

        ofSetHexColor(0x0000FF);
        ofRect(502, 130, 20, 150);

        ofSetHexColor(0x00FF00);
        ofRect(924, 0, 100, 400);

        ofSetHexColor(0xFFFF00);
        ofRect(502, 0, 20, 130);



        ofSetHexColor(0xFF00FF);
        ofRect(0,50,1024,20);
        //ofSetHexColor(0xFF00FF);
        //ofRect(0,190,800,20);
    }
    ofPopMatrix();
    rm.endOffscreenDraw();
    //ofTranslate(-360,-520);

    // finally, a report:

    ofSetHexColor(0xffffff);

    ofDrawBitmapString("ID",800,20);
    ofDrawBitmapString("x",900,20);
    ofDrawBitmapString("y",1000,20);
    ofDrawBitmapString("pX",1100,20);
    ofDrawBitmapString("pY",1200,20);
    ofDrawBitmapString("Alive",1300,20);
    for (int j = 0; j < blobs.size(); j++)
    {
        ofDrawBitmapString(ofToString(blobs[j].ID),800,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].x),900,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].y),1000,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].pX),1100,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].pY),1200,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].framesAlive),1300,(j*20) +40 );
    }

    char reportStr[1024];
    sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\ntracked Blob count: %i\nfps: %f", threshold, blobs.size(), ofGetFrameRate());
    ofDrawBitmapString(reportStr, 10, 295);

    //RM

    /*rm.startOffscreenDraw();

        if( toggleImage ){
            ofSetHexColor(0xffffff);
            twoScreenImage.draw(0, 0, 1400,500);
        }
        else{

            ofSetHexColor(0x323232);
            ofRect(0, 0, rm.width, rm.height);

            ofSetHexColor(0xFF0000);
            ofRect(100, 100, 100, 100);

            ofSetHexColor(0xFF00FF);
            ofRect(200, 100, 100, 100);

            ofSetHexColor(0xFFFF00);
            ofRect(800, 100, 100, 100);

            ofSetHexColor(0x0000FF);
            ofRect(1050, 100, 100, 100);
        }

    rm.endOffscreenDraw();*/

    ofSetHexColor(0xffffff);

    rm.drawInputDiagnostically(guiIn.x, guiIn.y, guiIn.width, guiIn.height);
    rm.drawOutputDiagnostically(guiOut.x, guiOut.y, guiOut.width, guiOut.height);

    glPushMatrix();
    glTranslatef(320, 575, 0);
    glScalef(0.3,0.3,0);
    ofSetHexColor(0xffffff);
    rm.drawScreen(0);
    rm.drawScreen(1);
    glPopMatrix();

    ofDrawBitmapString("internal texture points", 320, 290);
    ofDrawBitmapString("texture warping points", 850, 290);

    ofDrawBitmapString("screen 1", 10, 290);
    ofDrawBitmapString("screen 2", 710, 290);

    ofDrawBitmapString("s - to save to xml   r - to reload from xml    c - reset coordinates    g -  draw open gl shapes\n", 10, 275);

    if(debug) {
     ofSetColor(0);
    ofRect(0,0,2048,768);
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(2,1,0);
    ofSetHexColor(0xffffff);
    rm.drawScreen(0);
    rm.drawScreen(1);
    glPopMatrix();
    }

    gui.draw();
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{

    switch (key)
    {
    case ' ':
        bLearnBackground = true;
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

    if( key == 'g')
    {
        toggleDebugOutput = !toggleDebugOutput;
    }

    if( key == 's')
    {
        rm.saveToXml();
    }

    if( key == 'r' )
    {
        rm.reloadFromXml();
    }

    if(key == 'c')
    {
        rm.resetCoordinates();
    }

    if(key == 'd')
    {
       debug = !debug;
    }

    if(key == 'q')
    {
        gui.toggleDraw();
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    rm.mouseDragInputPoint(guiIn, ofPoint(x, y));
    rm.mouseDragOutputPoint(guiOut, ofPoint( x, y));
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    if( !rm.mouseSelectInputPoint(guiIn, ofPoint(x, y)) )
    {
        rm.mouseSelectOutputPoint(guiOut, ofPoint( x,  y));
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

void testApp::exit()
{
    thread_1.stop();
}

