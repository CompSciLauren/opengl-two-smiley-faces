// GLFWController.c++: a basic GLFWController (in Model-View-GLFWController sense)

#include <stdlib.h>

#include "GLFWController.h"
#include "ModelView.h"

int GLFWController::numGLFWControllers = 0;

GLFWController::GLFWController(const std::string& windowTitle, int rcFlags) :
	theWindow(nullptr),
	returnFromRun(false), runWaitsForAnEvent(true),
	lastPixelPosX(0), lastPixelPosY(0)
{
	if (numGLFWControllers++ == 0)
		glfwInit();

	// First create the window and its Rendering Context (RC)
	createWindowAndRC(windowTitle, rcFlags);
}

GLFWController::~GLFWController()
{
	if (theWindow != nullptr)
		glfwDestroyWindow(theWindow);

	if (--numGLFWControllers == 0)
		glfwTerminate();
}

void GLFWController::charCB(GLFWwindow* window, unsigned int theChar)
{
	if (theChar < 128)
	{
		GLFWController* c = dynamic_cast<GLFWController*>(curController);
		c->handleAsciiChar(
			static_cast<unsigned char>(theChar), c->lastPixelPosX, c->lastPixelPosY);
	}
}

void GLFWController::createWindowAndRC(const std::string& windowTitle, int rcFlags)
{
	// The following calls enforce use of only non-deprecated functionality.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	int minor = 8; // Start AT LEAST one greater than where you really want to start
	while ((theWindow == nullptr) && (minor > 0))
	{
		minor--;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		theWindow = glfwCreateWindow(newWindowWidth, newWindowHeight, titleString(windowTitle).c_str(), nullptr, nullptr);
	}
	if (theWindow == nullptr)
	{
		std::cerr << "\n**** COULD NOT CREATE A 4.x RENDERING CONTEXT ****\n\n";
		return;
	}

	glfwMakeContextCurrent(theWindow);
	initializeCallbacksForRC();
}

void GLFWController::handleDisplay()
{
	prepareWindow();
	// clear the frame buffer
	glClear(glClearFlags);

	renderAllModels();

	glfwSwapBuffers(theWindow);

	checkForErrors(std::cout, "GLFWController::handleDisplay");
}

void GLFWController::initializeCallbacksForRC()
{
	glfwSetWindowSizeCallback(theWindow, reshapeCB);
	glfwSetCharCallback(theWindow, charCB);
	glfwSetKeyCallback(theWindow, keyboardCB);
	glfwSetCursorPosCallback(theWindow, mouseMotionCB);
}

void GLFWController::keyboardCB(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (curController != nullptr)
	{
		GLFWController* theC = dynamic_cast<GLFWController*>(curController);
		if ((key == GLFW_KEY_ESCAPE) && (action != GLFW_PRESS))
			theC->handleAsciiChar(27, theC->lastPixelPosX, theC->lastPixelPosY);
	}
}

void GLFWController::mouseMotionCB(GLFWwindow* window, double x, double y)
{
	if (curController != nullptr)
	{
		GLFWController* c = dynamic_cast<GLFWController*>(curController);
		c->lastPixelPosX = static_cast<int>(x + 0.5);
		c->lastPixelPosY = static_cast<int>(y + 0.5);
	}
}

void GLFWController::prepareWindow() const
{
#ifdef __APPLE__
	if (theWindow == nullptr)
		return;
	glfwMakeContextCurrent(theWindow);
	int width, height;
	glfwGetFramebufferSize(theWindow, &width, &height);
	glViewport(0, 0, width, height);
#endif
}

void GLFWController::reportWindowInterfaceVersion(std::ostream& os) const
{
	os << "        GLFW: " << glfwGetVersionString() << '\n';
}

void GLFWController::reshapeCB(GLFWwindow* window, int width, int height)
{
	dynamic_cast<GLFWController*>(curController)->handleReshape(width, height);
}

void GLFWController::run()
{
	if (theWindow == nullptr)
		return;
	while (!glfwWindowShouldClose(theWindow) && !returnFromRun)
	{
		if (runWaitsForAnEvent)
			glfwWaitEvents();
		else
			glfwPollEvents();
		handleDisplay();
	}
	glfwDestroyWindow(theWindow);
	theWindow = nullptr;
}

void GLFWController::setWindowTitle(const std::string& title)
{
	if (theWindow != nullptr)
		glfwSetWindowTitle(theWindow, title.c_str());
}
