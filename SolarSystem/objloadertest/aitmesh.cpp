#include "aitMesh.h"
#include <sstream>
#include <map>
#include <iostream>
#include <assert.h>
#include <string>
#include <fstream>

using namespace std;

//function from stackoverflow https://stackoverflow.com/questions/16286095/similar-function-to-javas-string-split-in-c
vector <string> stringSplit(string content, string sep) {
	char* cstr = const_cast<char*>(content.c_str());
	char* current;
	std::vector<std::string> arr;
	current = strtok(cstr, sep.c_str());
	while (current != NULL) {
		arr.push_back(current);
		current = strtok(NULL, sep.c_str());
	}
	return arr;

}

//end of code from stackoverflow  https://stackoverflow.com/questions/16286095/similar-function-to-javas-string-split-in-c
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


	ifstream in(path);
	stringstream buffer;
	buffer << in.rdbuf();
	std::string contents(buffer.str());

	vector <string> linebyline = stringSplit(contents, "\n"); 
	
	
	cout << linebyline[1];
	
	for (int i = 0; i < linebyline.size(); i++) {
		if (linebyline[i].at(0) == 'v'  && linebyline[i].at(1) != 'n') {
			vector <string>  temp = stringSplit(linebyline[i], " ");
			glm::vec3 vertex;
			vertex.x = stof(temp[1]);
			vertex.y = stof(temp[2]);
			vertex.z = stof(temp[3]);
			vertTemp.push_back(vertex);

		}
		else if (linebyline[i].at(0) == 'v' && linebyline[i].at(1) == 'n') {
			vector <string>  temp = stringSplit(linebyline[i], " ");
			glm::vec3 normal;
			normal.x = stof(temp[1]);
			normal.y = stof(temp[2]);
			normal.z = stof(temp[3]);
			normalTemp.push_back(normal);
		}
		else if (linebyline[i].at(0) == 'f') {
			unsigned int vIndex[3], vtIndex[3];
			vector <string> temp = stringSplit(linebyline[i], " ");
			
			vIndex[0] = stoi(stringSplit(temp[1], "//")[0]);
			vtIndex[0] = stoi(stringSplit(temp[1], "//")[1]);

			vIndex[1] = stoi(stringSplit(temp[2], "//")[0]);
			vtIndex[1] = stoi(stringSplit(temp[2], "//")[1]);

			vIndex[2] = stoi(stringSplit(temp[3], "//")[0]);
			vtIndex[2] = stoi(stringSplit(temp[3], "//")[1]);

			vertIndex.push_back(vIndex[0]);
			vertIndex.push_back(vIndex[1]);
			vertIndex.push_back(vIndex[2]);

			normalIndex.push_back(vtIndex[0]);
			normalIndex.push_back(vtIndex[1]);
			normalIndex.push_back(vtIndex[2]);

			/*
						fscanf(file, "%d//%d %d//%d %d//%d\n", &vIndex[0], &vtIndex[0], &vIndex[1], &vtIndex[1], &vIndex[2], &vtIndex[2]);
			//cout << lineHeader << vIndex[0] << " " << vIndex[1] << " " << vIndex[2] << "\n";
			vertIndex.push_back(vIndex[0]);
			vertIndex.push_back(vIndex[1]);
			vertIndex.push_back(vIndex[2]);

			normalIndex.push_back(vtIndex[0]);
			normalIndex.push_back(vtIndex[1]);
			normalIndex.push_back(vtIndex[2]);
			*/
		}
	}
	/*
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
		*/
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


	return false; 


}