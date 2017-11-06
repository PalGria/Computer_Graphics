#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Windows.h>
#include <string>
#include "aitmesh.h"

using namespace std;
using namespace glm;

const int windowWidth = 1024; 
const int windowHeight = 768;

GLuint VBO;
int NUMVERTS = 0;

// Transform uniforms location
GLuint gModelToWorldTransformLoc;
GLuint gMoonTransforms;

GLuint gWorldToViewToProjectionTransformLoc;

// Lighting uniforms location
GLuint gAmbientLightIntensityLoc;
GLuint gDirectionalLightIntensityLoc;
GLuint gDirectionalLightDirectionLoc;

// Materials uniform location
GLuint gKaLoc;
GLuint gKdLoc;

static void renderSceneCallBack()
{
	// Clear the back buffer and the z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create our world space to view space transformation matrix
	mat4 worldToViewTransform = lookAt(
		vec3(0.0f,2.0f,8.0f), // The position of your camera, in world space
		vec3(0.0f,0.0f,0.0f), // where you want to look at, in world space
		vec3(0.0f,1.0f,0.0f)  // Camera up direction (set to 0,-1,0 to look upside-down)
		); 

    // Create out projection transform
    mat4 projectionTransform = perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 100.0f);

	// Combine the world space to view space transformation matrix and the projection transformation matrix
	mat4 worldToViewToProjectionTransform = projectionTransform * worldToViewTransform;

    // Update the transforms in the shader program on the GPU
    glUniformMatrix4fv(gWorldToViewToProjectionTransformLoc, 1, GL_FALSE, &worldToViewToProjectionTransform[0][0]);

    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), (const GLvoid*)12);

    // Set the material properties
    glUniform1f(gKaLoc, 0.8f); 
    glUniform1f(gKdLoc, 0.8f);

	// Draw the model
	mat4 modelToWorldTransform = mat4(1.0f);

	mat4 MoonTransforms = mat4(1.0f);

	static float angle = 0.0f;
	angle+=0.25f;

	MoonTransforms = rotate(MoonTransforms, angle, vec3(1.0f, 0.0f, 0.5f));

	MoonTransforms = translate(MoonTransforms, vec3(0.0f, 2.0f, 0.0f));
	MoonTransforms = scale(MoonTransforms, vec3(0.4, 0.4, 0.4));



	modelToWorldTransform = rotate(modelToWorldTransform, angle, vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &modelToWorldTransform[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);
	glUniformMatrix4fv(gMoonTransforms, 1, GL_FALSE, &MoonTransforms[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);

    glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

    glutSwapBuffers();
}

static void initializeGlutCallbacks()
{
    glutDisplayFunc(renderSceneCallBack);
    glutIdleFunc(renderSceneCallBack);
}

static void createVertexBuffer()
{
	aitMesh mesh1;

	if(!mesh1.loadFromObj("assets/sphere.obj"))
	{
		cerr<<"Error loading mesh from obj file."<<endl;
		system("pause");
		exit(0);
	}
	vector<aitVertex> verts = mesh1.getVertices();
	
	NUMVERTS = verts.size(); 
    
 	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(aitVertex) * NUMVERTS, &verts[0], GL_STATIC_DRAW);
}

static void initLights()
{
	// Setup the ambient light
	vec3 ambientLightIntensity = vec3(0.2f, 0.2f, 0.2f);
	glUniform3fv(gAmbientLightIntensityLoc, 1, &ambientLightIntensity[0]);
	
	// Setup the direactional light
	vec3 directionalLightDirection = vec3(0.0f, 0.0f, -1.0f);
	normalize(directionalLightDirection);
	glUniform3fv(gDirectionalLightDirectionLoc, 1, &directionalLightDirection[0]);
	vec3 directionalLightIntensity = vec3(0.7f, 0.7f, 0.7f);
	glUniform3fv(gDirectionalLightIntensityLoc, 1, &directionalLightIntensity[0]);
}

static void addShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
{
    GLuint shaderObj = glCreateShader(shaderType);

    if (shaderObj == 0) 
	{
        cerr<<"Error creating shader type "<<shaderType<<endl;
        exit(0);
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
        cerr<<"Error compiling shader type "<<shaderType<<": "<<InfoLog<<endl;
        exit(1);
    }

    glAttachShader(shaderProgram, shaderObj);
}

const string readFileToString(const char* filename)
{
	ifstream file (filename, ios::in);
	if (file.is_open())
	{
        stringstream continut;
        continut << file.rdbuf();
        continut << '\0';
		return continut.str();
	}
    return "";
}

static void buildShaders()
{
    GLuint shaderProgram = glCreateProgram();

    if (shaderProgram == 0) 
	{
        cerr<<"Error creating shader program\n";
        exit(1);
    }

	string VS = readFileToString("vertexShader.glsl");
	string FS = readFileToString("fragmentShader.glsl");

    addShader(shaderProgram, VS.c_str(), GL_VERTEX_SHADER);
    addShader(shaderProgram, FS.c_str(), GL_FRAGMENT_SHADER);

    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == 0) 
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
		cerr<<"Error linking shader program: "<<errorLog<<endl;
        exit(1);
	}

    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success) 
	{
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
        cerr<<"Error linking shader program: "<<errorLog<<endl;
        exit(1);
    }

    glUseProgram(shaderProgram);

    gModelToWorldTransformLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
    assert(gModelToWorldTransformLoc != 0xFFFFFFFF);
	gMoonTransforms = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gMoonTransforms != 0xFFFFFFFF);
    gWorldToViewToProjectionTransformLoc = glGetUniformLocation(shaderProgram, "gWorldToViewToProjectionTransform");
    assert(gWorldToViewToProjectionTransformLoc != 0xFFFFFFFF);
	gAmbientLightIntensityLoc = glGetUniformLocation(shaderProgram, "gAmbientLightIntensity");
	assert(gAmbientLightIntensityLoc != 0xFFFFFFFF);
	gDirectionalLightIntensityLoc = glGetUniformLocation(shaderProgram, "gDirectionalLightIntensity");
	assert(gDirectionalLightIntensityLoc != 0xFFFFFFFF);
	gDirectionalLightDirectionLoc = glGetUniformLocation(shaderProgram, "gDirectionalLightDirection");
	assert(gDirectionalLightDirectionLoc != 0xFFFFFFFF);
    gKaLoc = glGetUniformLocation(shaderProgram, "gKa");
	assert(gDirectionalLightDirectionLoc != 0xFFFFFFFF);
    gKdLoc = glGetUniformLocation(shaderProgram, "gKd");
	assert(gDirectionalLightDirectionLoc != 0xFFFFFFFF);
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("obj Loader Test");

    initializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) 
    {
		cerr<<"Error: "<<glewGetErrorString(res)<<"\n";
		return 1;
    }

	buildShaders();

	// Enable the z-buffer
	glEnable(GL_DEPTH_TEST);
	// GL_LESS - Passes if the incoming depth value is less than the stored depth value
	glDepthFunc(GL_LESS);

	//Enable backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW); 

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

	// Initialise lights
	initLights();
	 
	// Create a vertex buffer
	createVertexBuffer();

    glutMainLoop();
    
    return 0;
}