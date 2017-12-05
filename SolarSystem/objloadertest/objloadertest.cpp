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



class GameObject {
protected:
	vec4 colour;
	GLuint VBO;
	mat4 transforms;
	int NUMVERTS;
	vector<aitVertex> verts;
	int type; //0 gameobject, 1 planet, 2 fleer, 3 chaser, 4 wireframe
public:

	GameObject() {
		transforms = mat4(1.0f);
		NUMVERTS = 0;
		colour.r = 0.4f;
		colour.g = 0.4f;
		colour.b = 0.4f;
	}

	void virtual render() {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), (const GLvoid*)12);

		glUniform1f(gRedLocation, colour.r);
		glUniform1f(gGreenLocation, colour.g);
		glUniform1f(gBlueLocation, colour.b);
		glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &transforms[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	void virtual update() {



	}
	void loadVertices(string path) {


	}
	void scaleOBJ(float x, float y, float z) {
		transforms = scale(transforms, vec3(x, y, z));
	}
	void rotateOBJ(float angle, float x, float y, float z) {


	}
	void translateOBJ(float x, float y, float z) {
		transforms = translate(transforms, vec3(x, y, z));
	}

	void createVertexBuffer(string path)
	{
		aitMesh mesh1;

		//if (!mesh1.loadFromObj("assets/sphere.obj"))
		if (!mesh1.loadFromObj(path))
		{
			cerr << "Error loading mesh from obj file." << endl;
			system("pause");
			exit(0);
		}
		verts = mesh1.getVertices();

		NUMVERTS = verts.size();

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(aitVertex) * NUMVERTS, &verts[0], GL_STATIC_DRAW);
	}
	mat4 getTransforms() {
		return transforms;
	}
	void setTransfomrs(mat4 t) {
		transforms = t;
	}
	void setColor(float r, float g, float b, float a) {
		colour.r = r;
		colour.g = g;
		colour.b = b;
		colour.a = a;
	}
	int getType() {
		return type;
	}
};
class Planet : public GameObject {
protected:
	float orbitalSpeed;
	float rotationSpeed;
	float rotationAngle;
	float orbitalAngle;
public:
	Planet() {
		type = 0;
		orbitalSpeed = 0.25f;
		rotationSpeed = 0.25f;
		rotationAngle = 0; 
		orbitalAngle = 0;

	}
	void render() override {

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), (const GLvoid*)12);

		glUniform1f(gRedLocation, colour.r);
		glUniform1f(gGreenLocation, colour.g);
		glUniform1f(gBlueLocation, colour.b);
		glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &transforms[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);


	}
	void update() override {
		rotationAngle += rotationSpeed;
		orbitalAngle += orbitalSpeed;
		transforms = rotate(transforms, orbitalSpeed, vec3(0.0f, 0.5f, 0.0f));

		transforms = rotate(transforms, rotationSpeed, vec3(0.0f, 1.0f, 0.0f));

	}
};

class Moon : public GameObject {
protected:
	float orbitalSpeed;
	float rotationSpeed;
	float rotationAngle;
	float orbitalAngle;
	float distance;
	float size;
	GameObject* parent;
public:
	Moon(GameObject* par) {
		parent = par;
		type = 5;
		orbitalSpeed = 0.000000025f;
		rotationSpeed = 0.00000025f;
		rotationAngle = 0;
		orbitalAngle = 0;
		distance = 4.5;
		transforms = parent->getTransforms();
		size = 0.4;
	}
	void render() override {

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aitVertex), (const GLvoid*)12);

		glUniform1f(gRedLocation, colour.r);
		glUniform1f(gGreenLocation, colour.g);
		glUniform1f(gBlueLocation, colour.b);
		glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &transforms[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);


	}
	void update() override {

		transforms = parent->getTransforms();

		rotationAngle += rotationSpeed;
		orbitalAngle += orbitalSpeed;

		transforms = scale(transforms, vec3(size, size, size));

		transforms = rotate(transforms, rotationAngle, vec3(0.0f, 1.0f, 0.0f));
		transforms = translate(transforms, vec3(distance, 0.0f, 0.0f));
		transforms = rotate(transforms, orbitalAngle, vec3(0.0f, 1.0f, 0.0f));

	}
	void setSize(float s) {
		size = s;
	}
};



class Chaser : public GameObject {
protected:
	float speed;
	float turnSpeed;
	GameObject* objective;
	
public:
	void setObjective(GameObject* o) {
		objective = o;
		speed = 0.001f;
	}

	void render() {
		glUniform1f(gRedLocation, colour.r);
		glUniform1f(gGreenLocation, colour.g);
		glUniform1f(gBlueLocation, colour.b);
		glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &transforms[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);

	}
	void update() {
		mat4 t = objective->getTransforms();
		float xObj = t[3].x;
		float yObj = t[3].y;

		float xChaser = transforms[3].x;
		float yChaser = transforms[3].y;

		float newx, newy; 

		if (xObj > xChaser) {
			transforms = translate(transforms, vec3(speed, 0.0f, 0.0f));
		}
		else {
			transforms = translate(transforms, vec3(-speed, 0.0f, 0.0f));

		}
		if (yObj > yChaser) {
			transforms = translate(transforms, vec3(0.0f, speed, 0.0f));

		}
		else {
			transforms = translate(transforms, vec3(0.0f, -speed, 0.0f));

		}


	}
};
class Fleer : public GameObject {
protected:
	float speed;
	float turnSpeed;
	float movex, movey, movez;

public:
	Fleer() {
		movex = 0;
		movey = 0;
		movez = 0;
	}
	void render() {
		glUniform1f(gRedLocation, colour.r);
		glUniform1f(gGreenLocation, colour.g);
		glUniform1f(gBlueLocation, colour.b);
		glUniformMatrix4fv(gModelToWorldTransformLoc, 1, GL_FALSE, &transforms[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NUMVERTS);
	}
	void update() {
		transforms = translate(transforms, vec3(movex, movey, movez));
		movex = 0;
		movey = 0;
		movez = 0;
	}
	void move(int x,int y,int z) {
		movex = x;
		movey = y;
		movez = z;
	}
};
class wireframe : public GameObject {

};



GameObject* objects[10];
int objectsSize = 0;

void keyPressed(unsigned char key, int x, int y) {
	if (key == 'w') {
		objects[1]->translateOBJ(0.0f, 0.5f, 0.0f);
	}
	if (key == 's') {
		objects[1]->translateOBJ(0.0f, -0.5f, 0.0f);
	}
	if (key == 'd') {
		objects[1]->translateOBJ(0.5f, 0.0f, 0.0f);
	}
	if (key == 'a') {
		objects[1]->translateOBJ(-0.5f, 0.0f, 0.0f);
	}
	mat4 t = objects[1]->getTransforms();
	cout <<" X " << t[3].x << " Y " << t[3].y <<  "\n";
}




static void render(GameObject* GOarray[], int size) {

	int i = 0;
	for (i = 0; i < size; i++) 
			GOarray[i]->render();
	

}


static void update(GameObject* GOarray[], int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		GOarray[i]->update();
	}

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


	// Set the material properties
	glUniform1f(gKaLoc, 0.8f);
	glUniform1f(gKdLoc, 0.8f);


	// Draw the model
	//meter aqui el update y render
	update(objects, objectsSize);
	render(objects, objectsSize);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

static void initializeGlutCallbacks()
{
	glutDisplayFunc(renderSceneCallBack);
	glutIdleFunc(renderSceneCallBack);
	glutKeyboardFunc(keyPressed);
}
/*
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
*/
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
	glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);

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
	//assert(gRedLocation != 0xFFFFFFFF);
	gGreenLocation = glGetUniformLocation(shaderProgram, "green");
	//assert(gGreenLocation != 0xFFFFFFFF);
	gBlueLocation = glGetUniformLocation(shaderProgram, "blue");
	//assert(gBlueLocation != 0xFFFFFFFF);




	gWorldToViewToProjectionTransformLoc = glGetUniformLocation(shaderProgram, "gWorldToViewToProjectionTransform");
	assert(gWorldToViewToProjectionTransformLoc != 0xFFFFFFFF);
	gAmbientLightIntensityLoc = glGetUniformLocation(shaderProgram, "gAmbientLightIntensity");
//	assert(gAmbientLightIntensityLoc != 0xFFFFFFFF);
	gDirectionalLightIntensityLoc = glGetUniformLocation(shaderProgram, "gDirectionalLightIntensity");
//	assert(gDirectionalLightIntensityLoc != 0xFFFFFFFF);
	gDirectionalLightDirectionLoc = glGetUniformLocation(shaderProgram, "gDirectionalLightDirection");
//  assert(gDirectionalLightDirectionLoc != 0xFFFFFFFF);
	gKaLoc = glGetUniformLocation(shaderProgram, "gKa");
//	assert(gDirectionalLightDirectionLoc != 0xFFFFFFFF);
	gKdLoc = glGetUniformLocation(shaderProgram, "gKd");
//	assert(gDirectionalLightDirectionLoc != 0xFFFFFFFF);
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


	//we create the objects
	Planet* Saitama = new Planet();
	Saitama->setColor(0.0f, 1.0f, 0.4f, 1.0f);
	Saitama->createVertexBuffer("assets/sphere.obj");
	Saitama->scaleOBJ(0.7f, 0.7f, 0.7f);
	objects[objectsSize] = Saitama;
	objectsSize++;
	Fleer* Ribbit = new Fleer();
	Ribbit->createVertexBuffer("assets/sphere.obj");
	Ribbit->scaleOBJ(0.3, 0.3, 0.3);
	Ribbit->setColor(0.5f, 0.5f, 1.0f, 1.0f);
	Ribbit->translateOBJ(-5.0f, 3.0f, 5.0f);
	objects[objectsSize] = Ribbit;

	objectsSize++;

	Chaser* Chad = new Chaser();
	Chad->createVertexBuffer("assets/sphere.obj");
	Chad->setObjective(Ribbit);
	objects[objectsSize] = Chad;
	Chad->scaleOBJ(0.4f, 0.4f, 0.4f);
	Chad->setColor(1.0f, 0.5f, 0.5f, 1.0f);
	Chad->translateOBJ(-10.0f, -3.0f, 5.0f);

	objectsSize++;

	Moon* Shigeo = new Moon(Saitama);
	Shigeo->createVertexBuffer("assets/sphere.obj");
	objects[objectsSize] = Shigeo;
	Shigeo->setSize(0.3f);
	objectsSize++;



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
	//createVertexBuffer();

	glutMainLoop();

	// delete stuff!!!


	return 0;
}
