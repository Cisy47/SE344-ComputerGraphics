#pragma once
#include <glut.h>
#include <glm.hpp>
#include <vector>
#include <string>
#include <fstream>

using namespace std;
using namespace glm;

class Mesh
{
	friend class ParticleSystem;
	struct Face	//三角形顶点对应的顶点 纹理 法线索引
	{
		int vertexIndex[3];
		int textCoordIndex[3];
		int normalIndex[3];
	};

	vector<vec3> oriFaceNormals;
	vector<vec3> morphedFaceNormals;

	vector<vec3> oriVertice;		//原始顶点集合
	vector<vec3> morphedVertice;		//顶点变换后的顶点集合
	vector<vec2> oriTextCoords;		//纹理坐标集合
	vector<vec2> morphedTextCoords;		//纹理坐标集合
// 	vector<vec3> normals;		//法线集合
	vector<Face> faces;		//三角形集合

	vector<vector<int>> adjacentVertice;	//相邻的顶点

	vector<vec3> verticeVelocity;		//顶点速度集合

	void calculateCenterAndRadius();

	float getAverageLength(int index);
	float elasticLength;
	float targetRadius;   //变换基础半径

	GLuint textureID;
	GLuint targetTextureID;


public:
	float weight;
	Mesh();
	~Mesh();
	int triangleCount();
	void computeMorphedMesh(const Mesh &target);
	void readObjFile(const char *file, bool cw = true);
	void updateMesh(float dt);
	void drawMesh();
	void generateNormals(); //生成法线
};

