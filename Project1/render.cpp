#include "render.h"

#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

geometry makeGeometry(vertex * verts, size_t vertCount, unsigned * indices, size_t indxCount)
{
	// create an instance of geometry
	geometry newGeo = {};
	newGeo.size = indxCount;

	// generate buffers
	glGenVertexArrays(1, &newGeo.vao); // Vertex Array Object
	glGenBuffers(1, &newGeo.vbo); // Vertex Buffer Object
	glGenBuffers(1, &newGeo.ibo); // index Buffer Object

	// bind buffers
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);

	// populate buffers
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned), indices, GL_STATIC_DRAW);

	// describe vertex data
	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

	glEnableVertexAttribArray(1); // normals
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)16);

	glEnableVertexAttribArray(2); // colors
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)32);

	glEnableVertexAttribArray(3); // uvs
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)48);

	// unbind buffers (in a specific order)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// return the geometry
	return newGeo;
}

void freeGeometry(geometry &geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	geo = {};
}

shader loadShader(const char * vertPath, const char * fragPath)
{
	std::string basicVert;
	std::string basicFrag;
	std::string str;

	std::ifstream vertStream;
	std::ifstream fragStream;

	vertStream.open(vertPath);

	if (vertStream.fail() == true)
	{
		std::cout << "Failed" << std::endl;;
	}

	while(std::getline(vertStream, str))
	{
		basicVert += str;
		basicVert.push_back('\n');
	}

	vertStream.close();

	fragStream.open(fragPath);

	if (fragStream.fail() == true)
	{
		std::cout << "Failed" << std::endl;;
	}

	while (std::getline(fragStream, str))
	{
		basicFrag += str;
		basicFrag.push_back('\n');
	}

	fragStream.close();

	const char * vert = basicVert.c_str();
	const char * frag = basicFrag.c_str();

	shader newShad = makeShader(vert, frag);

	return newShad;
}

shader makeShader(const char * vertSource, const char * fragSource)
{
	// make the shader object
	shader newShad = {};
	newShad.program = glCreateProgram();

	// create the shaders
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	// compile the shaders
	glShaderSource(vert, 1, &vertSource, 0);
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);
	// TODO: See docs.gl on how to do error checking for shader

	glCompileShader(frag);
	// TODO: See docs.gl on how to do error checking for shader

	// attach the shaders
	glAttachShader(newShad.program, vert);
	glAttachShader(newShad.program, frag);

	// link the shaders
	glLinkProgram(newShad.program);
	// TODO: see docs.gl on how to do error checking on program

	// delete the shaders
	glDeleteShader(vert);
	glDeleteShader(frag);

	// return the shader object
	return newShad;
}

void freeShader(shader &shad)
{
	glDeleteProgram(shad.program);
	shad = {};
}

void draw(const shader &shad, const geometry &geo)
{
	// bind the shader program
	glUseProgram(shad.program);

	// bind the VAO (geo and indices)
	glBindVertexArray(geo.vao);

	// draw
	glDrawElements(GL_TRIANGLES, geo.size, GL_UNSIGNED_INT, 0);
}

void setUniform(const shader & shad, GLuint location, const glm::vec3 &value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value)
{
	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader & shad, GLuint location, const texture & value, int textureSlot)
{
	// specify the texture slot were working with
	glActiveTexture(GL_TEXTURE0 + textureSlot);

	// bind the texture to that slot
	glBindTexture(GL_TEXTURE_2D, value.handle);

	// assign the uniform to the shader 
	glProgramUniform1i(shad.program, location, textureSlot);
}

meshFormat loadMeshFromFile(const char * path)
{
	meshFormat geo;
	
	//tinyobj::LoadObj(&geo.attrib, &geo.shapes, &geo.materials, &geo.err, path, (const char *)0, true);

	return geo;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char * pixels)
{
	GLenum oglFormat = GL_RGBA;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;
		break;
	case 2:
		oglFormat = GL_RG;
		break;
	case 3:
		oglFormat = GL_RGB;
		break;
	case 4:
		oglFormat = GL_RGBA;
		break;
	default:
		// TODO: fatal error, halt the program
		break;
	}

	// generating and binding the texture
	texture tex = { 0, width, height, channels };
	glGenTextures(1, &tex.handle);
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	// buffer/send the actual data
	glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);

	// describe how the texture will be used
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void freeTexture(texture & tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};
}

texture loadTexture(const char * path)
{
	int imageWidth, imageHeight, imageFormat;
	unsigned char *rawPixelData = nullptr;

	// tell stb image to load the image
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);
	//TODO: ensure that rawPixelData is NOT NULL. if it is, the image failed to load

	// pass the data to make texture to make the texture
	texture tex = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);

	// free the image
	stbi_image_free(rawPixelData);

	return tex;
}