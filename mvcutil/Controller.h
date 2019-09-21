// Controller.h - An Abstract base Class for a Controller (in Model-View-Controller sense)

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

class ModelView;

class Controller
{
public:
	Controller();
	virtual ~Controller();

	void addModel(ModelView* m);
	void getOverallMCBoundingBox(double* xyzLimits) const;
	double getViewportAspectRatio() const; // height/width
	virtual void redraw() {} // to force a display update
	void reportVersions(std::ostream& os) const;
	virtual void run() = 0;
	virtual void setWindowTitle(const std::string& title) = 0;

	static bool checkForErrors(std::ostream& os, const std::string& context);
	static Controller* getCurrentController() { return curController; }
	static void setNewWindowSize(int width, int height);

protected:
	Controller(const Controller& c) {}
	std::vector<ModelView*> models;

	// Virtual methods for event handling (any can be overridden)
	// --> Keyboard-related event handling
	virtual void handleAsciiChar(unsigned char theChar, int x, int y);

	// --> Window-related event handling
	virtual void handleReshape(int width, int height);

	// --> Miscellaneous
	virtual void endProgram() { exit(0); }
	void pixelPointToLDSPoint(int x, int y, double& ldsX, double& ldsY);
	void pixelVectorToLDSVector(int dx, int dy, double& ldsDX, double& ldsDY);
	virtual void renderAllModels();
	virtual void reportWindowInterfaceVersion(std::ostream& os) const = 0;
	virtual void swapBuffers() const = 0;

	// Data the Controller uses to track the overall MC box bounding all models.
	double overallMCBoundingBox[6];
	int glClearFlags;

	static Controller* curController;
	static int newWindowWidth, newWindowHeight;
	static std::string titleString(const std::string& str);

private:
	void updateMCBoundingBox(ModelView* m);
};

#endif
