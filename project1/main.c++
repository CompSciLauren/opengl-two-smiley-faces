// main.c++

#include "GLFWController.h"
#include "ModelView.h"
#include <math.h>
#include <fstream>

void createScene(GLFWController& c, ShaderIF* sIF, float xValues[], float yValues[], int totalNPoints, int totalMPoints)
{
	float dt = 1.0 / (totalMPoints - 1);
	vec2 nVertexPositions[totalNPoints];
	vec2 buf[totalNPoints];
	vec2* mVertexPositions = new vec2[totalMPoints];
	float t;

	for (int i = 0; i < totalNPoints; i++)
	{
		nVertexPositions[i][0] = xValues[i];
		nVertexPositions[i][1] = yValues[i];
	}

	for (int i = 0; i < totalMPoints; i++)
	{
		for (int a = 0; a < totalNPoints; a++)
		{
			buf[a][0] = nVertexPositions[a][0];
			buf[a][1] = nVertexPositions[a][1];
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
		// i-th point for the VBO is now in buf[0]
		mVertexPositions[i][0] = buf[0][0];
		mVertexPositions[i][1] = buf[0][1];
	}

	c.addModel(new ModelView(sIF, nVertexPositions, totalNPoints, totalMPoints, mVertexPositions));
}

int main(int argc, char* argv[])
{
	GLFWController c(argv[0]);
	c.reportVersions(std::cout);

	ShaderIF* sIF = new ShaderIF("shaders/project1.vsh", "shaders/project1.fsh");

	int totalNPoints = 0;
	int totalMPoints = 0;

	std::ifstream userFileChoice;
	std::string fileName = argv[1];
	userFileChoice.open(fileName);
	if(!userFileChoice.is_open())
	{
		std::cout << "Failed to open file. File was not found.\n";
		exit(1);
	}
	while (!userFileChoice.eof())
	{
		userFileChoice >> totalNPoints;
		userFileChoice >> totalMPoints;
		float xValues[totalNPoints];
		float yValues[totalNPoints];
		
		for (int i = 0; i < totalNPoints; i++)
		{
			userFileChoice >> xValues[i];
			userFileChoice >> yValues[i];
		}

		createScene(c, sIF, xValues, yValues, totalNPoints, totalMPoints);
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
	userFileChoice.close();
	return 0;
}
