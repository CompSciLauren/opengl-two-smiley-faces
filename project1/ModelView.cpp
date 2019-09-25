// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"

double ModelView::mcRegionOfInterest[6] = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };
bool ModelView::aspectRatioPreservationEnabled = true;
int ModelView::numInstances = 0;

// NOTE: You will likely want to modify the ModelView constructor to
//       take additional parameters.
ModelView::ModelView(ShaderIF* sIF, vec2* triangleVertices, int nPoints, int mPoints) : shaderIF(sIF), serialNumber(++numInstances)
{
	// TODO: define and call method(s) to initialize your model and send data to GPU
	//       Be sure to track this instance's min/max coordinate ranges as you do so.
	//       (See ModelView::getMCBoundingBox below.)
	// DONE
	nTotalPoints = nPoints;
	mTotalPoints = mPoints;
	initModelGeometry(triangleVertices);
}

ModelView::~ModelView()
{
	// TODO: delete the vertex array objects and buffers here
	// DONE
	deleteObject();
}

void ModelView::deleteObject()
{
	if (vao[0] > 0) // not already deleted
	{
		glDeleteBuffers(1, vbo);
		glDeleteVertexArrays(1, vao);
		vao[0] = vbo[0] = 0; // keep? don't keep?
	}
}

void ModelView::compute2DScaleTrans(float* scaleTransF) // CLASS METHOD
{
	// TODO: This code can be used as is, BUT be absolutely certain you
	//       understand everything about how it works.
	// DONE

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
    // Map the overall limits to the -1..+1 range expected by the OpenGL engine:
	double scaleTrans[4];
	linearMap(xmin, xmax, -1.0, 1.0, scaleTrans[0], scaleTrans[1]);
	linearMap(ymin, ymax, -1.0, 1.0, scaleTrans[2], scaleTrans[3]);
	for (int i=0 ; i<4 ; i++)
		scaleTransF[i] = static_cast<float>(scaleTrans[i]);
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void ModelView::getMCBoundingBox(double* xyzLimits) const
{
	// TODO:
	// DONE
	// Put this ModelView instance's min and max x, y, and z extents
	// into xyzLimits[0..5]. (-1 .. +1 is OK for z direction for 2D models)
	xyzLimits[0] = xmin;
	xyzLimits[1] = xmax;
	xyzLimits[2] = ymin;
	xyzLimits[3] = ymax;
	xyzLimits[4] = -1.0; // [4] and [5] are
	xyzLimits[5] = 1.0;  // (zmin, zmax) but really it's 0..0
}

bool ModelView::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	if ((anASCIIChar >= '0') && (anASCIIChar <= '9'))
	{
		int which = static_cast<int>(anASCIIChar) - static_cast<int>('0');
		if (which == serialNumber) // was this message intended for me?
		{
			// Yes, it is intended for me. Process and then tell Controller
			// stop sending this event to other ModelView instances:
			// toggle display of N lines
			deleteObject();
			glBindVertexArray(vao[0]);
			glDrawArrays(GL_LINE_STRIP, 0, nTotalPoints);
			return false; // tell Controller to stop send this event
		}
		else if (which != serialNumber) // was this message intended for me?
		{
			// Yes, it is intended for me. Process and then tell Controller
			// stop sending this event to other ModelView instances:
			// toggle display of M lines
			deleteObject();
			glBindVertexArray(vao[1]);
			glDrawArrays(GL_LINE_STRIP, 0, mTotalPoints);
		}
	}
	return true; // not intended for me; tell Controller to keep trying.
}

void ModelView::initModelGeometry(vec2* vertices)
{
	// Alternate triangle colors between dark green and dark red
	if ((serialNumber % 2) == 1)
	{
		nColor[0] = 0.0; nColor[1] = 0.5; nColor[2] = 0.0;
	}
	else
	{
		nColor[0] = 0.5; nColor[1] = 0.0; nColor[2] = 0.0;
	}

	// create VAO and VBO names
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);

	// initialize them
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// allocate space, send data to GPU
	int numBytesInBuffer = nTotalPoints * sizeof(vec2);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	// determine and remember min/max coordinates
	xmin = xmax = vertices[0][0];
	ymin = ymax = vertices[0][1];
	for (int i = 0; i < nTotalPoints; i++)
	{
		if (vertices[i][0] < xmin)
		{
			xmin = vertices[i][0];
		}
		else if (vertices[i][0] > xmax)
		{
			xmax = vertices[i][0];
		}

		if (vertices[i][1] < ymin)
		{
			ymin = vertices[i][1];
		}
		else if (vertices[i][1] > ymax)
		{
			ymax = vertices[i][1];
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
	// TODO: This code can be used as is, BUT be absolutely certain you
	//       understand everything about how it works.
	// DONE

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

	// TODO: set scaleTrans (and all other needed) uniform(s)
	// DONE
	float scaleTrans[4];
	compute2DScaleTrans(scaleTrans);
	glUniform4fv(shaderIF->ppuLoc("scaleTrans"), 1, scaleTrans);

	glUniform3fv(shaderIF->ppuLoc("color"), 1, nColor); // establish the triangle color


	glUniform3fv(shaderIF->ppuLoc("color"), 1, mColor); // establish the triangle color
	// TODO: make require primitive call(s)
	// DONE
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINE_STRIP, 0, nTotalPoints);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_LINE_STRIP, 0, mTotalPoints);

	// restore the previous program
	glUseProgram(pgm);
}

void ModelView::setMCRegionOfInterest(double xyz[6])
{
	for (int i=0 ; i<6 ; i++)
		mcRegionOfInterest[i] = xyz[i];
}
