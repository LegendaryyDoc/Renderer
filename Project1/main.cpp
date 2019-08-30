#include "context.h"
#include "render.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

int main()
{
	context game;
	game.init(640, 480, "Try"); // created window

	// Triangle, CCW
	vertex triVerts[] =
	{
		{{-.5f, -.5f, 0, 1} , {0, 0, 1, 0} , {1.0f, 0.0f, 0.0f, 1} , {0, 0}},
		{{.5f, -.5f, 0, 1} ,  {0, 0, 1, 0} , {0.0f, 1.0f, 0.0f, 1} , {1, 0}},
		{{0, .5f, 0, 1} ,	  {0, 0, 1, 0} , {0.0f, 0.0f, 1.0f, 1} , {.5f, 1}}
	};

	unsigned int triIndices[] = { 0, 1, 2 };

	/*meshFormat obj = loadMeshFromFile("test.obj");

	if (!obj.err.empty)
	{
		std::cout << obj.err << std::endl;
	}*/

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);

	shader basicShade = loadShader("shader/basicVert.txt", "shader/basicFrag.txt");


	glm::mat4 triModel = glm::identity<glm::mat4>(); 
	glm::mat4 camProj = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	texture cube = loadTexture("tex.tga");

	light sun;
	sun.direction = glm::vec4{ -1,0,0,1 };

	setUniform(basicShade, 0, camProj);
	setUniform(basicShade, 1, camView);
	setUniform(basicShade, 3, cube, 0);
	setUniform(basicShade, 4, sun.direction);

	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		triModel = glm::rotate(triModel, glm::radians(1.0f), glm::vec3(0, 1, 0));

		setUniform(basicShade, 2, triModel);

		draw(basicShade, triangle);
	}

	game.term();

	return 0;
}