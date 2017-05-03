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
	struct Face	//�����ζ����Ӧ�Ķ��� ���� ��������
	{
		int vertexIndex[3];
		int textCoordIndex[3];
		int normalIndex[3];
	};

	vector<vec3> oriFaceNormals;
	vector<vec3> morphedFaceNormals;

	vector<vec3> oriVertice;		//ԭʼ���㼯��
	vector<vec3> morphedVertice;		//����任��Ķ��㼯��
	vector<vec2> oriTextCoords;		//�������꼯��
	vector<vec2> morphedTextCoords;		//�������꼯��
// 	vector<vec3> normals;		//���߼���
	vector<Face> faces;		//�����μ���

	vector<vector<int>> adjacentVertice;	//���ڵĶ���

	vector<vec3> verticeVelocity;		//�����ٶȼ���

	void calculateCenterAndRadius();

	float getAverageLength(int index);
	float elasticLength;
	float targetRadius;   //�任�����뾶

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
	void generateNormals(); //���ɷ���
};

