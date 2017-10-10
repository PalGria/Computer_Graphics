#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <Windows.h>

#include "aiterror.h"

using namespace std;
using namespace glm;

GLuint VBO;
const int NUMVERTS = 3;


static void renderSceneCallBack()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);

    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}

static void initializeGlutCallbacks()
{
    glutDisplayFunc(renderSceneCallBack);
}

static void createVertexBuffer()
{
	// Create some vertices to put in our VBO.
	vec3 vertices[NUMVERTS];
    vertices[0] = vec3(-0.5f, -0.5f, 0.0f);
	vertices[1] = vec3(0.5f, -0.5f, 0.0f);
	vertices[2] = vec3(0.0f, 0.5f, 0.0f);
    
 	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

static void addShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
{
    GLuint shaderObj = glCreateShader(shaderType);

    if (shaderObj == 0) 
	{
		std::stringstream ss;
		ss<<"Error creating shader type "<<shaderType<<endl;
		AIT_ASSERT(0, ss.str());
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(shaderObj, 1, p, Lengths);
    glCompileShader(shaderObj);
    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
    if (!success) 
	{
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shaderObj, 1024, NULL, InfoLog);
		std::stringstream ss;
		ss <<"Error compiling shader type "<<shaderType<<": "<<InfoLog<<endl;
		AIT_ASSERT(0, ss.str());
    }

    glAttachShader(shaderProgram, shaderObj);
}

const char* readFileToBuffer(char* filename)
{
	ifstream file (filename, ios::in);
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		unsigned int size = (unsigned int)file.tellg();
		char *contents = new char [size+1];
		file.seekg (0, ios::beg);
		file.read (contents, size);
		file.close();
		contents[size] = 0;
		return contents;
	}
    return NULL;
}

static void buildShaders()
{
    GLuint shaderProgram = glCreateProgram();

    if (shaderProgram == 0) 
	{
		AIT_ASSERT(0, "Error creating shader program\n");
    }

	const char* pVS = readFileToBuffer("vertexShader.glsl");
	const char* pFS = readFileToBuffer("fragmentShader.glsl");

    addShader(shaderProgram, pVS, GL_VERTEX_SHADER);
    addShader(shaderProgram, pFS, GL_FRAGMENT_SHADER);

	delete [] pVS;
	delete [] pFS;

    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == 0) 
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
		std::stringstream ss;
		ss<<"Error linking shader program: "<<errorLog<<endl;
		AIT_ASSERT(0, ss.str());
	}

    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success) 
	{
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
        cerr<<"Error linking shader program: "<<errorLog<<endl;
        std::stringstream ss;
		ss<<"Error linking shader program: "<<errorLog<<endl;
		AIT_ASSERT(0, ss.str());
    }

    glUseProgram(shaderProgram);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("First Shaders");

    initializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
		cerr<<"Error: "<<glewGetErrorString(res)<<"\n";
		return 1;
    }

	buildShaders();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

	// Create a vertex buffer
	createVertexBuffer();

    glutMainLoop();
    
    return 0;
}