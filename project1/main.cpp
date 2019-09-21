// main.c++

#include "GLFWController.h"
#include "ModelView.h"

void createScene(GLFWController& c, ShaderIF* sIF)
{
	// TODO: Complete this function
	// DONE
	vec2 vertexPositions[][3] =
	{
		// triangle 1
		{ { -6.0, 137.0 }, {2.0, 137.0 }, {-2.0, 145.0 } },
		// triangle 2
		{ { -6.0, 135.0 }, {2.0, 135.0 }, { -2.0, 127.0 } }
	};

	c.addModel(new ModelView(sIF, vertexPosition[0]));
	c.addModel(new ModelView(sIF, vertexPosition[1]));
}

int main(int argc, char* argv[])
{
	GLFWController c(argv[0]);
	c.reportVersions(std::cout);

	ShaderIF* sIF = new ShaderIF("shaders/project1.vsh", "shaders/project1.fsh");

	createScene(c, sIF);

	// initialize 2D viewing information:
	// Get the overall scene bounding box in Model Coordinates:
	double xyz[6]; // xyz limits, even though this is 2D
	c.getOverallMCBoundingBox(xyz);
	std::cout << "Bounding box: " << xyz[0] << " <= x <= " << xyz[1] << '\n';
	std::cout << "              " << xyz[2] << " <= y <= " << xyz[3] << '\n';
	std::cout << "              " << xyz[4] << " <= z <= " << xyz[5] << "\n\n";
	// Tell class ModelView we initially want to see the whole scene:
	ModelView::setMCRegionOfInterest(xyz);

	c.run();

	delete sIF;

	return 0;
}
