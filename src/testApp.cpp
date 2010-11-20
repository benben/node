#include "testApp.h"
#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);
    threshold = 80;
    debug = true;

    thread_1.cam.setVerbose(true);
    thread_1.cam.setDesiredFrameRate(60);
    thread_1.cam.setDeviceID(0);
    //thread_1.cam.initGrabber(320,240);
    thread_1.cam.setUseTexture(false);

    thread_2.offset = 320;
    thread_2.cam.setVerbose(true);
    thread_2.cam.setDesiredFrameRate(60);
    thread_2.cam.setDeviceID(1);
    thread_2.cam.initGrabber(320,240);
    thread_2.cam.setUseTexture(false);

    thread_1.initAndSleep();
    thread_2.initAndSleep();

    ID = 0;

    //RM
    // num, renderwidth, renderheight
    rm.allocateForNScreens(2, 1920, 768);
    rm.loadFromXml("fboSettings.xml");

    guiIn   = ofRectangle(320, 295, 500, 178);
    guiOut  = ofRectangle(guiIn.x + guiIn.width + 300, guiIn.y, 500, 178);

    twoScreenImage.loadImage("adam_1920_768.jpg");
    debugOutput = 0;

    //GUI
    gui.addTitle("1");
    gui.addQuadWarper("Cam 1", thread_1.colorImg, thread_1.warpPoints);
    gui.addSlider("threshold", threshold,20,160);
    gui.addTitle("-").newColumn = true;
    gui.addTitle("2").newColumn = true;
    gui.addQuadWarper("Cam 2", thread_2.colorImg, thread_2.warpPoints);
    gui.loadFromXML();
}

void testApp::trackBlobs(vector<ofxCvBlob> _blobs)
{

    ofxVec2f _b;
    ofxVec2f b;
    float dist;
    float speed = 0.8;
    float dX = 0;
    float dY = 0;
    float minD = 4;
    float maxD = 25;

    for (int i = 0; i < _blobs.size(); i++)
    {
        bool bIsNewBlob = true;
        for (int j = 0; j < blobs.size(); j++)
        {
            _b.set( _blobs[i].boundingRect.x, _blobs[i].boundingRect.y );
            b.set(blobs[j].x, blobs[j].y);
            dist = b.distance(_b);
            if(dist < 25)
            {
                bIsNewBlob = false;
                blobs[j].pX = blobs[j].x;
                blobs[j].pY = blobs[j].y;
                //if (dist >= 5)
                //{
                    // pos += (targetPos - pos) * SPEED;
                    float dX = (_blobs[i].boundingRect.x - blobs[j].x) * speed;
                    float dY = (_blobs[i].boundingRect.y - blobs[j].y) * speed;

                    if((dX >= minD || dX <= -minD) && (dX <= maxD || dX >= -maxD)) {
                        blobs[j].x += dX;
                    }
                    if((dY >= minD || dY <= -minD) && (dY <= maxD || dY >= -maxD)) {
                        blobs[j].y += dY;
                    }
                    //blobs[j].boundingRect.x           += (_blobs[i].boundingRect.x - blobs[j].boundingRect.x) * speed;
                    //blobs[j].boundingRect.y           += (_blobs[i].boundingRect.y - blobs[j].boundingRect.y) * speed;
                //}
                blobs[j].frame = ofGetFrameNum();
                blobs[j].nPts = _blobs[i].nPts;
                blobs[j].pts = _blobs[i].pts;

                /*if(_blobs[i].boundingRect.width <= blobs[j].boundingRect.width -2 || _blobs[i].boundingRect.width >= blobs[j].boundingRect.width +2)
                {
                    blobs[j].boundingRect.width += (_blobs[i].boundingRect.width - blobs[j].boundingRect.width) * speed;
                }
                if(_blobs[i].boundingRect.height <= blobs[j].boundingRect.height -2 || _blobs[i].boundingRect.height >= blobs[j].boundingRect.height +2)
                {
                    blobs[j].boundingRect.height += (_blobs[i].boundingRect.height - blobs[j].boundingRect.height) * speed;
                } */
            }

        }

        if(bIsNewBlob)
        {
            trackedBlob tB;
            tB.ID = ID;
            ID++;
            tB.state = UPCOMING;
            tB.frame = ofGetFrameNum();
            tB.x = _blobs[i].boundingRect.x;
            tB.y = _blobs[i].boundingRect.y;
            tB.pX = _blobs[i].boundingRect.x;
            tB.pY = _blobs[i].boundingRect.y;
            tB.nPts = _blobs[i].nPts;
            tB.pts = _blobs[i].pts;
            /*tB.boundingRect.x           = _blobs[i].boundingRect.x;
            tB.boundingRect.y           = _blobs[i].boundingRect.y;
            tB.boundingRect.width       = _blobs[i].boundingRect.width;
            tB.boundingRect.height      = _blobs[i].boundingRect.height;*/
            tB.framesAlive = 0;
            tB.alpha = 0;
            blobs.push_back(tB);
        }
    }

    //finally kill all blobs which weren't updated the last 25 frames
    for (int j = 0; j < blobs.size(); j++)
    {
        if(blobs[j].frame < ofGetFrameNum() - 40)
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

        //set state to alive, if they live longer then 30 frames
        //set alpha to 255
        if(blobs[j].framesAlive > 40)
        {
            blobs[j].state = ALIVE;
            blobs[j].alpha = 255;
        }

        //set state dying which weren't updated since the last 2 frames
        //fade alpha out
        if(blobs[j].frame < ofGetFrameNum() - 2)
        {
            blobs[j].state = DYING;
            blobs[j].alpha = -5 * (ofGetFrameNum() - blobs[j].frame) + blobs[j].alpha;
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

    if(bLearnBackground)
    {
        thread_1.bLearnBackground = true;
        thread_2.bLearnBackground = true;
        bLearnBackground = false;
    }

    thread_1.setThreshold(threshold);
    thread_2.setThreshold(threshold);

    thread_1.updateOnce();
    trackBlobs(thread_1.getBlobs());

    thread_2.updateOnce();
    trackBlobs(thread_2.getBlobs());

    rm.myOffscreenTexture.clear();
}

//--------------------------------------------------------------
void testApp::draw()
{
    /****************************************
    /* START TO DRAW IN THE FBO
    /****************************************/
    rm.startOffscreenDraw();
    switch (debugOutput)
    {
    //DRAW THE REAL OUTPUT
    case 0:
        glPushMatrix();
        glScalef(1920/640,768/240,0);
        for (int i = 0; i < blobs.size(); i++)
        {
            ofEnableAlphaBlending();

            ofFill();
            ofSetColor(255,255,255,blobs[i].alpha);
            ofLine(blobs[i].x,blobs[i].y,blobs[i].pX,blobs[i].pY);

            ofCircle(blobs[i].x+7.5,blobs[i].y+7.5,15);

            ofDisableAlphaBlending();
        }
        glPopMatrix();
        break;
    //DRAW OUTPUT WITH BLOBS
    case 1:
        glPushMatrix();
        glScalef(1920/640,768/240,0);
        for (int i = 0; i < blobs.size(); i++)
        {
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

            ofEnableAlphaBlending();

            ofFill();
            ofSetColor(255,255,255,blobs[i].alpha);
            ofLine(blobs[i].x,blobs[i].y,blobs[i].pX,blobs[i].pY);
            float size = sqrt( pow(blobs[i].boundingRect.width,2) + pow(blobs[i].boundingRect.height,2) ) / 2 * 0.8;
            ofCircle(blobs[i].x,blobs[i].y,size);

            ofDisableAlphaBlending();
        }
        glPopMatrix();
        break;
    //DRAW GL BOXES
    case 2:
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
        break;
    //DRAW AN IMAGE
    case 3:
        twoScreenImage.draw(0,0);
        break;
    }
    rm.endOffscreenDraw();
    /****************************************
    /* END TO DRAW IN THE FBO
    /****************************************/

    if(!debug)
    {
        /****************************************
        /* DRAW THE REAL OUTPUT
        /****************************************/
        glPushMatrix();
        ofRect(0,0,1920,768);
        //glTranslatef(0, 0, 0);
        //glScalef(2,1,0);
        ofSetHexColor(0xffffff);
        rm.drawScreen(0);
        rm.drawScreen(1);
        glPopMatrix();
    }
    else
    {
        /****************************************
        /* DRAW THE DEBUG OUTPUT
        /****************************************/
        ofSetHexColor(0xffffff);
        char reportStr[1024];
        sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\ntracked Blob count: %i\nfps: %f", threshold, blobs.size(), ofGetFrameRate());
        ofDrawBitmapString(reportStr, 10, 295);



        rm.drawInputDiagnostically(guiIn.x, guiIn.y, guiIn.width, guiIn.height);
        rm.drawOutputDiagnostically(guiOut.x, guiOut.y, guiOut.width, guiOut.height);

        glPushMatrix();
        glTranslatef(320, 525, 0);
        glScalef(0.3,0.3,0);
        rm.drawScreen(0);
        rm.drawScreen(1);
        glPopMatrix();

        ofDrawBitmapString("internal texture points", 320, 290);
        ofDrawBitmapString("texture warping points", 850, 290);

        ofDrawBitmapString("screen 1", 10, 290);
        ofDrawBitmapString("screen 2", 710, 290);

        ofDrawBitmapString("s - to save to xml   r - to reload from xml    c - reset coordinates    g -  draw open gl shapes\n", 10, 275);

        drawInputs();
        drawDebugTable();
        gui.draw();
    }


}

void testApp::drawInputs ()
{
    thread_1.colorImg.draw(10,10,160,120);
    thread_1.grayImg.draw(180,10,160,120);
    thread_1.grayBg.draw(10,140,160,120);
    thread_1.grayDiff.draw(180,140,160,120);

    thread_2.colorImg.draw(360,10,160,120);
    thread_2.grayImg.draw(530,10,160,120);
    thread_2.grayBg.draw(360,140,160,120);
    thread_2.grayDiff.draw(530,140,160,120);
}

void testApp::drawDebugTable ()
{
    ofSetHexColor(0xffffff);
    int start = 1024;
    ofDrawBitmapString("ID",start,20);
    ofDrawBitmapString("x",start+30,20);
    ofDrawBitmapString("y",start+110,20);
    ofDrawBitmapString("pX",start+180,20);
    ofDrawBitmapString("pY",start+250,20);
    ofDrawBitmapString("width",start+320,20);
    ofDrawBitmapString("height",start+390,20);
    ofDrawBitmapString("frame",start+460,20);
    ofDrawBitmapString("alive",start+530,20);
    ofDrawBitmapString("state",start+600,20);
    for (int j = 0; j < blobs.size(); j++)
    {
        ofDrawBitmapString(ofToString(blobs[j].ID),start,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].x,2),start+30,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].y,2),start+110,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].pX,2),start+180,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].pY,2),start+250,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].boundingRect.width,2),start+320,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].boundingRect.height,2),start+390,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].frame,2),start+460,(j*20) +40 );
        ofDrawBitmapString(ofToString(blobs[j].framesAlive,2),start+530,(j*20) +40 );
        string state;
        switch(blobs[j].state)
        {
        case UPCOMING:
            state = "UPCOMING";
            break;
        case ALIVE:
            state = "ALIVE";
            break;
        case DYING:
            state = "DYING";
            break;
        }
        ofDrawBitmapString(state,start+600,(j*20) +40 );
    }
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
        debugOutput += 1;
        if(debugOutput > 3) {
            debugOutput = 0;
        }
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
    thread_2.stop();
}

