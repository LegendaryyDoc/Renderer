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
		{{-.5f, -.5f, 0, 1} , {1.0f, 0.0f, 0.0f, 1}, {0, 0}},
		{{.5f, -.5f, 0, 1} , {0.0f, 1.0f, 0.0f, 1}, {1, 0}},
		{{0, .5f, 0, 1} , {0.0f, 0.0f, 1.0f, 1}, {.5f, 1}}
	};

	unsigned int triIndices[] = { 0, 1, 2 };

	/*meshFormat obj = loadMeshFromFile("test.obj");

	if (!obj.err.empty)
	{
		std::cout << obj.err << std::endl;
	}*/

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);

	const char * basicVert =
		"#version 430\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec2 uv;\n"
		"layout (location = 1) in vec4 color;\n"
		"layout (location = 0) uniform mat4 proj;\n"
		"layout (location = 1) uniform mat4 view;\n"
		"layout (location = 2) uniform mat4 model;\n"
		"out vec4 vColor;\n"
		"out vec2 vUV;\n"
		"void main() {gl_Position = proj * view * model * position; vColor = color; vUV = uv;}";

	const char * basicFrag =
		"#version 430 \n"
		"layout (location = 3) uniform sampler2D albedo;\n"
		"in vec4 vColor;\n"
		"in vec2 vUV;\n"
		"out vec4 vertColor;\n"
		"void main() {vertColor = texture(albedo, vUV);}";

	shader basicShade = makeShader(basicVert, basicFrag);

	glm::mat4 triModel = glm::identity<glm::mat4>(); 
	glm::mat4 camProj = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	texture cube = loadTexture("tex.tga");

	setUniform(basicShade, 0, camProj);
	setUniform(basicShade, 1, camView);
	setUniform(basicShade, 3, cube, 0);

	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		triModel = glm::rotate(triModel, glm::radians(5.0f), glm::vec3(0, 1, 0));

		setUniform(basicShade, 2, triModel);

		draw(basicShade, triangle);
	}

	game.term();

	return 0;
}