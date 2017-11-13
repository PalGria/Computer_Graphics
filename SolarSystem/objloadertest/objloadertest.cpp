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
const int maxPlanets = 3; 
GLuint VBO;
int NUMVERTS = 0;

// Transform uniforms location
GLuint gModelToWorldTransformLoc;
/*
GLuint gReigenTransformsLoc;
GLuint gShigeoTransformsLoc;
GLuint gReigenMoonTransformsLoc;
GLuint gShigeoMoonTransformsLoc;
*/
GLuint gRedLocation;
GLuint gBlueLocation;
GLuint gGreenLocation;

GLuint gWorldToViewToProjectionTransformLoc;

// Lighting uniforms location
GLuint gAmbientLightIntensityLoc;
GLuint gDirectionalLightIntensityLoc;
GLuint gDirectionalLightDirectionLoc;

// Materials uniform location
GLuint gKaLoc;
GLuint gKdLoc;

class Sun {
	private: 
		mat4 transforms;
		float red, blue, green, size;
		Planet planets[maxPlanets];
		int planetsNumber;
		float speed;

	public:	
		Sun() {
			transforms = mat4(1.0f);
			planetsNumber = 0;
			red = 1.0f;
			blue = 1.0f;
			green = 1.0f;

			size = 1.0f;
		}
		void addPlanet(Planet planet) {
			if (planetsNumber < maxPlanets) {
				planets[planetsNumber] = planet;
				planetsNumber++;

			}
		}
		void setColor(float r, float g, float b) {
			red = r;
			green = g;
			blue = b;
		}
		mat4 rotateObj(float angle, float x, float y, float z) {
			transforms = rotate(transforms, angle, vec3(0.0f, 0.5f, 0.0f));
			return transforms;
		}
		mat4 scaleObj(float size) {
			transforms = scale(transforms, vec3(size, size, size));
			return transforms;

		}
		mat4 translateOBJ(float x, float y, float z) {
			transforms = translate(transforms, vec3(x, y, z));
			return transforms;
		}
		mat4 getTransforms() {
			return transforms; 
		}
		void setSpeed(float s) {
			speed = s;
		}
		float getSpeed() {
			return speed;
		}
};
class Planet {
private:
	mat4 transforms;
	float red, blue, green, size, distance;
	Moon moons[maxPlanets];
	int moonsNumber;
	float speed;

public:
	Planet() {
		transforms = mat4(1.0f);
		red = 1.0f;
		blue = 1.0f;
		green = 1.0f;

	}

	void addMoon(Moon moon) {
		if (moonsNumber < maxPlanets) {
			moons[moonsNumber] = moon;
			moonsNumber++;

		}
	}
	void setColor(float r, float g, float b) {
		red = r;
		green = g;
		blue = b;
	}
	mat4 rotateObj(float angle, float x, float y, float z) {
		transforms = rotate(transforms, angle, vec3(0.0f, 0.5f, 0.0f));
		return transforms;
	}
	mat4 scaleObj(float size) {
		transforms = scale(transforms, vec3(size, size, size));
		return transforms;

	}
	mat4 translateOBJ(float x, float y, float z) {
		transforms = translate(transforms, vec3(x, y, z));
		return transforms;
	}
	mat4 getTransforms() {
		return transforms;
	}
	void setSpeed(float s) {
		speed = s;
	}
	float getSpeed() {
		return speed;
	}
};
class Moon {
private:
	mat4 transforms;
	float red, blue, green, size, distance;
	float speed;
public:
	Moon() {
		transforms = mat4(1.0f);
		red = 1.0f;
		blue = 1.0f;
		green = 1.0f;

	}

	void setColor(float r, float g, float b) {
		red = r;
		green = g;
		blue = b;
	}
	mat4 rotateObj(float angle, float x, float y, float z) {
		transforms = rotate(transforms, angle, vec3(0.0f, 0.5f, 0.0f));
		return transforms;
	}
	mat4 scaleObj(float size) {
		transforms = scale(transforms, vec3(size, size, size));
		return transforms;

	}
	mat4 translateOBJ(float x,float y,float z) {
		transforms = translate(transforms, vec3(x, y, z));
		return transforms;
	}
	mat4 getTransforms() {
		return transforms;
	}
	void setSpeed(float s) {
		speed = s;
	}
	float getSpeed() {
		return speed;
	}

};
static void update() {


}
static void renderSceneCallBack()
{
	// Clear the back buffer and the z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create our world space to view space transformation matrix
	mat4 worldToViewTransform = lookAt(
		vec3(0.0f, 2.0f, 8.0f), // The position of your camera, in world space
		vec3(0.0f, 0.0f, 0.0f), // where you want to look at, in world space
		vec3(0.0f, 1.0f, 0.0f)  // Camera up direction (set to 0,-1,0 to look upside-down)
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

	mat4 ReigenTransforms = mat4(1.0f);
	mat4 ReigenMoonTransforms = mat4(1.0f);
	mat4 ShigeoTransforms = mat4(1.0f);
	mat4 ShigeoMoonTransforms = mat4(1.0f);

	float red = 0.9f;
	float green = 0.9f;;
	float blue = 0.0f;;
	glUniform1f(gRedLocation , red);
	glUniform1f(gBlueLocation, blue);
	glUniform1f(gGreenLocation, green);
	/*
	static float angleReigen = 0.0f;
	angleReigen += 0.15f;

	static float angleShigeo = 0.0f;
	angleShigeo += 0.2f;
	//reigen transforms
	ReigenTransforms = rotate(ReigenTransforms, angleReigen, vec3(0.0f, 0.5f, 0.0f));
	ReigenTransforms = translate(ReigenTransforms, vec3(2.0f, 0.0f, 0.0f));
	ReigenTransforms = rotate(ReigenTransforms, 2.0f, vec3(0.0f, 1.0f, 0.0f));

	ReigenTransforms = scale(ReigenTransforms, vec3(0.4, 0.4, 0.4));

	ReigenMoonTransforms = ReigenTransforms;

	ReigenMoonTransforms = rotate(ReigenMoonTransforms, angleReigen, vec3(0.0f, 0.5f, 0.0f));
	ReigenMoonTransforms = translate(ReigenMoonTransforms, vec3(2.0f, 0.0f, 0.0f));
	ReigenMoonTransforms = scale(ReigenMoonTransforms, vec3(0.4, 0.4, 0.4));
	//shigeo transforms
	ShigeoTransforms = rotate(ShigeoTransforms, angleShigeo, vec3(0.0f, 1.0f, 0.0f));
	ShigeoTransforms = translate(ShigeoTransforms, vec3(-4.0f, 0.0f, 0.0f));
	ShigeoTransforms = scale(ShigeoTransforms, vec3(0.4, 0.4, 0.4));

	ShigeoMoonTransforms = ShigeoTransforms;

	ShigeoMoonTransforms = rotate(ShigeoMoonTransforms, angleShigeo, vec3(0.0f, 1.0f, 0.0f));
	ShigeoMoonTransforms = translate(ShigeoMoonTransforms, vec3(2.0f, 0.0f, 0.0f));
	ShigeoMoonTransforms = scale(ShigeoMoonTransforms, vec3(0.4, 0.4, 0.4));


	modelToWorldTransform = rotate(modelToWorldTransform, angleReigen, vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &modelToWorldTransform[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);
	//reigen
	glUniform1f(gRedLocation, 0.0f);
	glUniform1f(gBlueLocation, 1.0f);
	glUniform1f(gGreenLocation, 0.2f);
	glUniformMatrix4fv(gReigenTransformsLoc, 1, GL_FALSE, &ReigenTransforms[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);

	glUniformMatrix4fv(gReigenMoonTransformsLoc, 1, GL_FALSE, &ReigenMoonTransforms[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);


	//shigeo
	glUniform1f(gRedLocation, 1.0f);
	glUniform1f(gBlueLocation, 0.3f);
	glUniform1f(gGreenLocation, 0.2f);
	glUniformMatrix4fv(gShigeoTransformsLoc, 1, GL_FALSE, &ShigeoTransforms[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);

	glUniformMatrix4fv(gShigeoMoonTransformsLoc, 1, GL_FALSE, &ShigeoMoonTransforms[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	*/
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

	if (!mesh1.loadFromObj("assets/sphere.obj"))
	{
		cerr << "Error loading mesh from obj file." << endl;
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
		cerr << "Error creating shader type " << shaderType << endl;
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(shaderObj, 1, p, Lengths);
	glCompileShader(shaderObj);
	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shaderObj, 1024, NULL, InfoLog);
		cerr << "Error compiling shader type " << shaderType << ": " << InfoLog << endl;
		exit(1);
	}

	glAttachShader(shaderProgram, shaderObj);
}

const string readFileToString(const char* filename)
{
	ifstream file(filename, ios::in);
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
		cerr << "Error creating shader program\n";
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
		cerr << "Error linking shader program: " << errorLog << endl;
		exit(1);
	}

	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
		cerr << "Error linking shader program: " << errorLog << endl;
		exit(1);
	}

	glUseProgram(shaderProgram);

	//The frickin sun
	gModelToWorldTransformLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gModelToWorldTransformLoc != 0xFFFFFFFF);
	/*
	//Reigen and his moon
	gReigenTransformsLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gModelToWorldTransformLoc != 0xFFFFFFFF);

	gReigenMoonTransformsLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gReigenMoonTransformsLoc != 0xFFFFFFFF);

	//Shigeo and his moon
	gShigeoTransformsLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gShigeoTransformsLoc != 0xFFFFFFFF);

	gShigeoMoonTransformsLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gShigeoMoonTransformsLoc != 0xFFFFFFFF);
	
	gShigeoMoonTransformsLoc = glGetUniformLocation(shaderProgram, "gModelToWorldTransform");
	assert(gShigeoMoonTransformsLoc != 0xFFFFFFFF);
	*/
	gRedLocation = glGetUniformLocation(shaderProgram, "red");
	assert(gRedLocation != 0xFFFFFFFF);
	gGreenLocation = glGetUniformLocation(shaderProgram, "green");
	assert(gGreenLocation != 0xFFFFFFFF);	
	gBlueLocation = glGetUniformLocation(shaderProgram, "blue");
	assert(gBlueLocation != 0xFFFFFFFF);




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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("obj Loader Test");

	initializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cerr << "Error: " << glewGetErrorString(res) << "\n";
		return 1;
	}

	buildShaders();
	//define objects

	Sun Saitama;
	Planet Reigen, Shigeo;
	Moon ReigenMoon, ShigeoMoon;

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