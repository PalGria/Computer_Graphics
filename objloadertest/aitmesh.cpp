#include "aitMesh.h"
#include <sstream>
#include <map>
#include <iostream>
#include <assert.h>
#include <string>
#include <fstream>

using namespace std;

bool aitMesh::loadFromObj(std::string path)
{
	/*
	aitVertex v1(glm::vec3(0.000000, 1.000000, 0.000000), glm::vec3(0.000000, 0.000000, 1.000000));
	aitVertex v2(glm::vec3(-1.000000, -1.000000, 0.000000), glm::vec3(0.000000, 0.000000, 1.000000));
	aitVertex v3(glm::vec3(1.000000, -1.000000, 0.000000), glm::vec3(0.000000, 0.000000, 1.000000));
	
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	
	*/

	vector <glm::vec3> vertTemp, normalTemp;
	vector <unsigned int> vertIndex, normalIndex;
	char *cstr = new char[path.length() + 1];
	strcpy(cstr, path.c_str());

	FILE * file = fopen(cstr, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
	}
	else {
		printf("File read!\n");

		
		while (true) {

			char lineHeader[64];

			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; 
			else {
				if (strcmp(lineHeader, "v") == 0) {

					glm::vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					vertTemp.push_back(vertex);
					//cout << lineHeader << vertex.x <<" "<< vertex.y << " "<< vertex.z << "\n";
				}

				else if (strcmp(lineHeader, "vn") == 0) {
					glm::vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					normalTemp.push_back(normal);
					//cout << lineHeader << normal.x << " " << normal.y << " " << normal.z << "\n";
				}
				else if (strcmp(lineHeader, "f") == 0) {
					unsigned int vIndex[3], vtIndex[3];
					fscanf(file, "%d//%d %d//%d %d//%d\n", &vIndex[0], &vtIndex[0], &vIndex[1], &vtIndex[1], &vIndex[2], &vtIndex[2]);
					//cout << lineHeader << vIndex[0] << " " << vIndex[1] << " " << vIndex[2] << "\n";
					vertIndex.push_back(vIndex[0]);
					vertIndex.push_back(vIndex[1]);
					vertIndex.push_back(vIndex[2]);

					normalIndex.push_back(vtIndex[0]);
					normalIndex.push_back(vtIndex[1]);
					normalIndex.push_back(vtIndex[2]);

				}

			}
		}

		for ( int i = 0; i < vertIndex.size(); i++) {
			unsigned int indexVActual = vertIndex[i];
			unsigned int indexNActual = normalIndex[i];
			//format aitVertex v2(glm::vec3(-1.000000, -1.000000, 0.000000), glm::vec3(0.000000, 0.000000, 1.000000));

			glm::vec3 vert = vertTemp[indexVActual - 1];
			glm::vec3 norm = normalTemp[indexNActual - 1];
			aitVertex vert_final(vert, norm);
			vertices.push_back(vert_final);
		}
		return true;

	}

	return false; 


}