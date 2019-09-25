// main.c++

#include "GLFWController.h"
#include "ModelView.h"
#include <math.h>
#include <fstream>

void createScene(GLFWController& c, ShaderIF* sIF, float a[], float b[], int totalNPoints, int mPoints)
{
	float dt = 1.0 / (mPoints - 1);
	
	vec2 vertices[totalNPoints];
	vec2 buf[totalNPoints];

	vec2* mPointVertices = new vec2[mPoints];

	float t;

	for (int i = 0; i < totalNPoints; i++)
	{
		vertices[i][0] = a[i];
		vertices[i][1] = b[i];
	}

	for (int i = 0; i < mPoints; i++)
	{
		for (int a = 0; a < totalNPoints; a++)
		{
			buf[a][0] = vertices[a][0];
			buf[a][1] = vertices[a][1];
		}

		t = i * dt;
		for (int j = 0; j <= totalNPoints - 2; j++) 
		 {
		 	for (int k = 0; k <= totalNPoints - j - 2; k++)
		 	{
		 		buf[k][0] = (1 - t) * buf[k][0] + t * buf[k + 1][0];
		 		buf[k][1] = (1 - t) * buf[k][1] + t * buf[k + 1][1];
		 	}
		 }
		//i-th point for the VBO is now in buf[0]
		mPointVertices[i][0] = buf[0][0];
		mPointVertices[i][1] = buf[0][1];

		std::cout << "x: " << mPointVertices[i][0] << "\n";
		std::cout << "y: " << mPointVertices[i][1] << "\n";
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
	while (!userFileChoice.eof())
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
