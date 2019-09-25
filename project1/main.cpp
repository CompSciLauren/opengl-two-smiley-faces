// main.c++

#include "GLFWController.h"
#include "ModelView.h"
#include <math.h>
#include <fstream>

void createScene(GLFWController& c, ShaderIF* sIF, float a[], float b[], int totalNPoints, int mPoints)
{
	// TODO: Complete this function
	// DONE
	float dt = 1.0 / (mPoints - 1);
	float xt, yt;

	vec2 vertices[totalNPoints];
	vec2 buf[totalNPoints];

	vec2* mPointVertices = new vec2[mPoints];

	for (int i = 0; i < totalNPoints; i++)
	{
		float t = i * dt;
		for (int j = 0; j < totalNPoints - j - 2; j++) 
		{
			buf[j][0] = (1 - t) * buf[j][0] + t * buf[j + 1][0];
			buf[j][1] = (1 - t) * buf[j][1] + t * buf[j + 1][1];
			xt = a[0] + (a[1] * t) + (a[2] * pow(t, 2.0)) + (a[3] * pow(t, 3.0));
			yt = b[0] + (b[1] * t) + (b[2] * pow(t, 2.0)) + (b[3] * pow(t, 3.0));
		}
		//i-th point for the VBO is now in buf[0]
		//vertices[i][0] = xt;
		//vertices[i][1] = yt;
		vertices[i][0] = a[i];
		vertices [i][1] = b[i];
		mPointVertices[i][0] = buf[i][0];
		mPointVertices[i][1] = buf[i][1];
	}

	c.addModel(new ModelView(sIF, vertices, totalNPoints, mPoints, mPointVertices));
}

int main(int argc, char* argv[])
{
	GLFWController c(argv[0]);
	c.reportVersions(std::cout);

	ShaderIF* sIF = new ShaderIF("shaders/project1.vsh", "shaders/project1.fsh");

	int totalNPoints = 0;
	int mPoints = 0;

	std::ifstream userFileChoice;
	std::string fileName = argv[1];
	userFileChoice.open(fileName);
	for (int i = 0; i < 8; i++) // was while (userFileChoice)
	{
		userFileChoice >> totalNPoints;
		userFileChoice >> mPoints;
		float a[totalNPoints];
		float b[totalNPoints];
		
		for (int i = 0; i < totalNPoints; i++)
		{
			userFileChoice >> a[i];
			userFileChoice >> b[i];
		}

		createScene(c, sIF, a, b, totalNPoints, mPoints);
	}

	// initialize 2D viewing information:
	// Get the overall scene bounding box in Model Coordinates:
	double xyz[6]; // xyz limits, even though this is 2D
	c.getOverallMCBoundingBox(xyz);
	std::cout << "Bounding box: " << xyz[0] << " <= x <= " << xyz[1] << '\n';
	std::cout << "              " << xyz[2] << " <= y <= " << xyz[3] << '\n';
	std::cout << "              " << xyz[4] << " <= z <= " << xyz[5] << "\n\n";
	// Tell class ModelView we initially want to see the whole scene:
	ModelView::setMCRegionOfInterest(xyz);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	c.run();

	delete sIF;

	return 0;
}
