// ModelView.h - a basic combined Model and View for OpenGL

#ifndef MODELVIEW_H
#define MODELVIEW_H

#include "ShaderIF.h"

#include <string>

#ifdef __APPLE_CC__
#include "GLFW/glfw3.h"
#else
#include <GL/gl.h>
#endif

// for interfacing to common GLSL data types
typedef float vec2[2];
typedef float vec3[3];

class ModelView
{
public:
	ModelView(ShaderIF* sIF, vec2* nVertexPositions, int nPoints, int mPoints, vec2* mVertexPositions);
	virtual ~ModelView();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimits) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render() const;

	// viewing controls common to 2D and 3D
	static void setAspectRatioPreservationEnabled(bool b)
		{ aspectRatioPreservationEnabled = b; }
	static void setMCRegionOfInterest(double xyz[6]);

private:
	GLuint vao[2];
	GLuint vbo[2];
	int nTotalPoints;
	int mTotalPoints;
	int serialNumber;
	float xmin, xmax, ymin, ymax;
	vec3 nColor;
	vec3 mColor;

	ShaderIF* shaderIF;

	void deleteObject();

	void initModelGeometry(vec2* nVertexPositions, vec2* mVertexPositions);

	// Routines for computing parameters necessary to map from arbitrary
	// model coordinate ranges into OpenGL's -1..+1 Logical Device Space.
	// 1. linearMap determines the scale and translate parameters needed in
	//    order to map a value, f (fromMin <= f <= fromMax) to its corresponding
	//    value, t (toMin <= t <= toMax). Specifically: t = scale*f + trans.
	static void linearMap(double fromMin, double fromMax,
		double toMin, double toMax, double& scale, double& trans);
	// 2. matchAspectRatio modifies (xmin,xmax) OR (ymin,ymax) to center that
	//    region in an expanded window that matches the given aspect ratio.
	static void matchAspectRatio(double& xmin, double& xmax,
		double& ymin, double& ymax, double vAR);
	// 3. compute2DScaleTrans uses the current model coordinate region of
	//    interest - modified as necessary to preserve aspect ratios - and
	//    then passes the modified limits to linearMap to compute the scale
	//    and translation needed to map MC to LDS.
	//    (The scales and translationes are returned in float[] because
	//    glUniform currently allows only float[].)
	static void compute2DScaleTrans(float* scaleTrans);

	static double mcRegionOfInterest[6];
	static int numInstances; // needed for serialNumber
	static bool aspectRatioPreservationEnabled;
};

#endif
