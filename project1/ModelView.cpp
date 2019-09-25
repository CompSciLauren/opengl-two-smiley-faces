// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"

double ModelView::mcRegionOfInterest[6] = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };
bool ModelView::aspectRatioPreservationEnabled = true;
int ModelView::numInstances = 0;

ModelView::ModelView(ShaderIF* sIF, vec2* triangleVertices, int nPoints, int mPoints, vec2* mVertexPositions) : shaderIF(sIF), serialNumber(++numInstances)
{
	nTotalPoints = nPoints;
	mTotalPoints = mPoints;
	initModelGeometry(triangleVertices, mVertexPositions);
}

ModelView::~ModelView()
{
	deleteObject();
}

void ModelView::deleteObject()
{
	if (vao[0] > 0) // not already deleted
	{
		glDeleteBuffers(1, vbo);
		glDeleteVertexArrays(1, vao);
		vao[0] = vbo[0] = 0;
	}
}

void ModelView::compute2DScaleTrans(float* scaleTransF)
{
	double xmin = mcRegionOfInterest[0];
	double xmax = mcRegionOfInterest[1];
	double ymin = mcRegionOfInterest[2];
	double ymax = mcRegionOfInterest[3];

	if (aspectRatioPreservationEnabled)
	{
		// preserve aspect ratio. Make "region of interest" wider or taller to
		// match the Controller's viewport aspect ratio.
		double vAR = Controller::getCurrentController()->getViewportAspectRatio();
		matchAspectRatio(xmin, xmax, ymin, ymax, vAR);
	}

    // We are only concerned with the xy extents for now, hence we will
    // ignore mcRegionOfInterest[4] and mcRegionOfInterest[5].
    // Map the overall limits to the -1..+1 range expected by the OpenGL engine
	double scaleTrans[4];
	linearMap(xmin, xmax, -1.0, 1.0, scaleTrans[0], scaleTrans[1]);
	linearMap(ymin, ymax, -1.0, 1.0, scaleTrans[2], scaleTrans[3]);
	for (int i=0 ; i<4 ; i++)
		scaleTransF[i] = static_cast<float>(scaleTrans[i]);
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void ModelView::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xmin;
	xyzLimits[1] = xmax;
	xyzLimits[2] = ymin;
	xyzLimits[3] = ymax;
	xyzLimits[4] = -1.0; // [4] and [5] are
	xyzLimits[5] = 1.0;  // (zmin, zmax) but really it's 0..0
}

bool toggle = true;

bool ModelView::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	if (anASCIIChar == 'n')
	{
		toggle = true;
	}
	else if (anASCIIChar == 'm')
	{
		toggle = false;
	}
	return true; // not intended for me; tell Controller to keep trying
}

void ModelView::initModelGeometry(vec2* nVertexPositions, vec2* mVertexPositions)
{
	// Alternate colors for n
	if ((serialNumber % 2) == 1)
	{
		nColor[0] = 0.0; nColor[1] = 0.5; nColor[2] = 0.0;
	}
	else
	{
		nColor[0] = 0.5; nColor[1] = 0.0; nColor[2] = 0.0;
	}

	// Alternate colors for m
	if ((serialNumber % 2) == 1)
	{
		mColor[0] = 0.4; mColor[1] = 0.9; mColor[2] = 0.7;
	}
	else
	{
		mColor[0] = 0.2; mColor[1] = 0.0; mColor[2] = 0.4;
	}

	// create VAOs and VBOs
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	// initialize these for n
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// allocate space and send data to GPU for n
	int numBytesInBuffer = nTotalPoints * sizeof(vec2); 
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, nVertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	// initialize these for m
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	// allocate space and send data to GPU for m
	int numBytesInBufferM = mTotalPoints * sizeof(vec2);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBufferM, mVertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	// determine and remember min/max coordinates
	xmin = xmax = nVertexPositions[0][0];
	ymin = ymax = nVertexPositions[0][1];
	
	for (int i = 0; i < nTotalPoints; i++)
	{
		if (nVertexPositions[i][0] < xmin)
		{
			xmin = nVertexPositions[i][0];
		}
		else if (nVertexPositions[i][0] > xmax)
		{
			xmax = nVertexPositions[i][0];
		}

		if (nVertexPositions[i][1] < ymin)
		{
			ymin = nVertexPositions[i][1];
		}
		else if (nVertexPositions[i][1] > ymax)
		{
			ymax = nVertexPositions[i][1];
		}
	}

	for (int i = 0; i < mTotalPoints; i++)
	{
		if (mVertexPositions[i][0] < xmin)
		{
			xmin = mVertexPositions[i][0];
		}
		else if (mVertexPositions[i][0] > xmax)
		{
			xmax = mVertexPositions[i][0];
		}

		if (mVertexPositions[i][1] < ymin)
		{
			ymin = mVertexPositions[i][1];
		}
		else if (mVertexPositions[i][1] > ymax)
		{
			ymax = mVertexPositions[i][1];
		}
	}
}

// linearMap determines the scale and translate parameters needed in
// order to map a value, f (fromMin <= f <= fromMax) to its corresponding
// value, t (toMin <= t <= toMax). Specifically: t = scale*f + trans.
void ModelView::linearMap(double fromMin, double fromMax, double toMin, double toMax,
					  double& scale, double& trans) // CLASS METHOD
{
	scale = (toMax - toMin) / (fromMax - fromMin);
	trans = toMin - scale*fromMin;
}

void ModelView::matchAspectRatio(double& xmin, double& xmax,
        double& ymin, double& ymax, double vAR)
{
	double wHeight = ymax - ymin;
	double wWidth = xmax - xmin;
	double wAR = wHeight / wWidth;
	if (wAR > vAR)
	{
		// make window wider
		wWidth = wHeight / vAR;
		double xmid = 0.5 * (xmin + xmax);
		xmin = xmid - 0.5*wWidth;
		xmax = xmid + 0.5*wWidth;
	}
	else
	{
		// make window taller
		wHeight = wWidth * vAR;
		double ymid = 0.5 * (ymin + ymax);
		ymin = ymid - 0.5*wHeight;
		ymax = ymid + 0.5*wHeight;
	}
}

void ModelView::render() const
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	// set scaleTrans (and all other needed) uniform(s)
	float scaleTrans[4];
	compute2DScaleTrans(scaleTrans);
	glUniform4fv(shaderIF->ppuLoc("scaleTrans"), 1, scaleTrans);

    // make require primitive call(s)
	if (toggle == true)
	{
		glBindVertexArray(vao[0]);
		glDrawArrays(GL_LINE_STRIP, 0, nTotalPoints);
		glUniform3fv(shaderIF->ppuLoc("color"), 1, nColor); // establish the color for n
	}
	else
	{
		glBindVertexArray(vao[1]);
		glDrawArrays(GL_LINE_STRIP, 0, mTotalPoints);
		glUniform3fv(shaderIF->ppuLoc("color"), 1, mColor); // establish the color for m
	}

	// restore the previous program
	glUseProgram(pgm);
}

void ModelView::setMCRegionOfInterest(double xyz[6])
{
	for (int i=0 ; i<6 ; i++)
		mcRegionOfInterest[i] = xyz[i];
}
