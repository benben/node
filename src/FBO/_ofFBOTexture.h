/*
 *  ofFBOTexture.h
 *  openFrameworks
 *
 *  Created by Zach Gage on 3/28/08.
 *  Copyright 2008 STFJ.NET. All rights reserved.
 *
 */

#ifndef _FBO_TEX
#define _FBO_TEX


#include "ofMain.h"
#include <iostream>


class ofFBOTexture : public ofTexture
{

	public:

		void allocate(int w, int h, bool autoC);

		void swapIn();
		void swapOut();

		void setupScreenForMe();
		void setupScreenForThem();


		void clear();

		//----------------------------------------------------------
        void draw(float x, float y){
            draw(x,y,texData.width, texData.height);
        }
        void draw(float x, float y, float w, float h);

	protected:

		GLuint fbo;					// Our handle to the FBO
		GLuint depthBuffer;			// Our handle to the depth render buffer
		bool autoClear;
		void clean();
};

#endif

