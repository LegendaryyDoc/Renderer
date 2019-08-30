#pragma once

#include "glew/GL/glew.h"
#include "glm/glm.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include "TinyObjLoader/tiny_obj_loader.h"

struct vertex
{
	glm::vec4 pos;
	glm::vec4 norm;
	glm::vec4 color;
	glm::vec2 uv;
};

struct geometry
{
	GLuint vao, vbo, ibo; // buffers
	GLuint size; // index count
};

struct shader
{
	GLuint program;
};

struct meshFormat
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
};

struct texture
{
	GLuint handle;
	unsigned width, height, channels;
};

struct light
{
	glm::vec3 direction;
};

geometry makeGeometry(vertex * verts, size_t vertCount, unsigned * indices, size_t indxCount);

void freeGeometry(geometry &geo);

shader loadShader(const char * vertPath, const char * fragPath);
shader makeShader(const char * vertSource, const char * fragSource);
void freeShader(shader &shad);

void draw(const shader &shad, const geometry &geo);

void setUniform(const shader &shad, GLuint location, const glm::vec3 &value);
void setUniform(const shader &shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader &shad, GLuint location, const texture &value, int textureSlot);

meshFormat loadMeshFromFile(const char * path);

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char * pixels);
void freeTexture(texture &tex);
texture loadTexture(const char * path);