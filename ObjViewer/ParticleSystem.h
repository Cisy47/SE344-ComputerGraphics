#pragma once
#include <glut.h>
#include <glm.hpp>
#include <vector>
#include "Mesh.h"

const static int MAX_PARTICAL_COUNT = 1000;

struct Particle 
{
	glm::vec3 color;
	glm::vec3 pos;
	glm::vec3 velocity;
	//Color c;
	float size;
	float lifeTime;
	float age;
};

class ParticleSystem
{
	std::vector<Particle> particles;
	glm::vec3 minV;
	glm::vec3 maxV;
	float maxSize;
	float minSize;
	float minLife;
	float maxLife;

	Mesh *attachedModel;

	bool active;

	Particle generateParticle();

	float randomValue(float min, float max);
public:
	ParticleSystem(glm::vec3 minV = { -1.0f, -1.0f, -1.0f }, glm::vec3 maxV = { 1.0f, 1.0f, 1.0f },
		float minS = 0.5, float maxS = 0.5, float minL = 0.5, float maxL = 2);
	~ParticleSystem();
	void update(float dt);
	void render();
	void setActive(bool act);
	bool ifActive();
	void attachToModel(Mesh *model);
};

