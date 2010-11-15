#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

    cam_1.loadMovie("fingers.mov");
    cam_1.play();

    cam_2.loadMovie("fingers.mov");
    cam_2.play();

    colorImg_1.allocate(320,240);
	grayImg_1.allocate(320,240);
	grayBg_1.allocate(320,240);
	grayDiff_1.allocate(320,240);

    colorImg_2.allocate(320,240);
	grayImg_2.allocate(320,240);
	grayBg_2.allocate(320,240);
	grayDiff_2.allocate(320,240);

	bLearnBakground = true;
	threshold = 80;

	thread_1.initAndSleep();
	thread_2.start();
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame_1 = false;
    bool bNewFrame_2 = false;

    cam_1.idleMovie();
    bNewFrame_1 = cam_1.isFrameNew();

    cam_2.idleMovie();
    bNewFrame_2 = cam_2.isFrameNew();

	if (bNewFrame_1){

        colorImg_1.setFromPixels(cam_1.getPixels(), 320,240);

        grayImg_1 = colorImg_1;
		if (bLearnBakground == true){
			grayBg_1 = grayImg_1;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			//##bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff_1.absDiff(grayBg_1, grayImg_1);
		grayDiff_1.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		//contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
        thread_1.setImage(grayDiff_1);

        blobs_1 = thread_1.getBlobs();

	}

	if (bNewFrame_2){

        colorImg_2.setFromPixels(cam_2.getPixels(), 320,240);

        grayImg_2 = colorImg_2;
		if (bLearnBakground == true){
			grayBg_2 = grayImg_2;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff_2.absDiff(grayBg_2, grayImg_2);
		grayDiff_2.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		//contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
        thread_2.setImage(grayDiff_2);

        blobs_2 = thread_2.getBlobs();

	}

}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg_1.draw(20,20);
	grayImg_1.draw(360,20);
	grayBg_1.draw(20,280);
	grayDiff_1.draw(360,280);

    // draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg_2.draw(700,20);
	grayImg_2.draw(1040,20);
	grayBg_2.draw(700,280);
	grayDiff_2.draw(1040,280);

	// then draw the contours:

	ofFill();
	ofSetHexColor(0x333333);
	ofRect(360,540,320,240);
	ofRect(1040,540,320,240);
	ofSetHexColor(0xffffff);

	// we could draw the whole contour finder
	// contourFinder.draw(360,540);

	// or, instead we can draw each blob individually,
	// this is how to get access to them:
    for (int i = 0; i < blobs_1.size(); i++){
        blobs_1[i].draw(360,540);
    }

    for (int i = 0; i < blobs_2.size(); i++){
        blobs_2[i].draw(1040,540);
    }

	// finally, a report:

	ofSetHexColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\nfps: %f", threshold, ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 600);
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
        case 't':
                thread_1.beginUpdate();
			break;
        case 'z':
                thread_1.pauseUpdate();
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

