#include "time.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(glm::vec3 minV, glm::vec3 maxV, float minS, float maxS, float minL, float maxL)
{
	active = false;
	this->minV = minV;
	this->maxV = maxV;
	attachedModel = NULL;
	minSize = minS;
	maxSize = maxS;
	minLife = minL;
	maxLife = maxL;

	srand(clock());
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::update(float dt)
{
	if (!active)
	{
		return;
	}
	for (auto &p : particles)
	{
		p.pos += dt*p.velocity;
		p.age += dt;
		//p.lifeTime -= dt;
		if (p.lifeTime<=p.age)
		{
			p = generateParticle();
		}
		p.velocity = p.velocity - glm::vec3{ 0, 5, 0 }*dt; //重力加速度
	}
	if (particles.size()<MAX_PARTICAL_COUNT)
	{
		particles.push_back(generateParticle());
	}
}

Particle ParticleSystem::generateParticle()
{
	glm::vec3 colors[2] = { { 0, 0, 1 }, { 1, 0, 1 } };
	if (!attachedModel)
	{
		return {
			{ randomValue(0, 1), randomValue(0, 1), randomValue(0, 1) },//color
			{ 0,0,0 },//pos
			{ randomValue(minV.x, maxV.x), randomValue(minV.y, maxV.y), randomValue(minV.z, maxV.z) },//velocity
			randomValue(minSize, maxSize),	//size
			randomValue(minLife, maxLife),	//lifetime
			0,//age
		};
	}
	else
	{
		int faceIndex = rand() % (attachedModel->faces.size());
		const auto &f = attachedModel->faces[faceIndex];
		glm::vec3 weight = { randomValue(0, 1) + 0.0001, randomValue(0, 1) + 0.0001, randomValue(0, 1) + 0.0001 };
		weight /= dot(weight,vec3{ 1.0f, 1.0f, 1.0f });
		auto pos = attachedModel->oriVertice[f.vertexIndex[0]] * weight.x + attachedModel->oriVertice[f.vertexIndex[1]] * weight.y + attachedModel->oriVertice[f.vertexIndex[2]] * weight.z;
		auto velocity = attachedModel->oriFaceNormals[faceIndex];
		if (dot(velocity,pos)<0.0)
		{
			velocity = -velocity;
		}
		velocity *= randomValue(length(maxV) / 5.0, length(maxV));
		return{
			{ randomValue(0, 1), randomValue(0, 1), randomValue(0, 1) },//color
			//colors[rand() % 2],  //color
			pos,//pos
			velocity,//velocity
			randomValue(minSize, maxSize),	//size
			randomValue(minLife, maxLife),	//lifetime
			0,  //age
		};

	}

}

float ParticleSystem::randomValue(float min, float max)
{
	return min + (max - min)*0.001*(rand() % 1000);
}

void ParticleSystem::render()
{
	
	glDisable(GL_TEXTURE_2D);
	for (auto &p:particles)
	{
		if (p.lifeTime>p.age)
		{
			float alpha = 1 - p.age / p.lifeTime;
			glPushMatrix();
			glTranslatef(p.pos.x, p.pos.y, p.pos.z );
			glColor4f((float)p.color[0],(float)p.color[1],(float)p.color[2],alpha);
			glutSolidSphere(p.size<p.lifeTime?p.size:p.lifeTime, 10, 10);
			glPopMatrix();
		}
	}
	glEnable(GL_TEXTURE_2D);
}

void ParticleSystem::setActive(bool act)
{
	active = act;
	if (!active)
	{
		particles.clear();
	}
}

void ParticleSystem::attachToModel(Mesh *model)
{
	attachedModel = model;
}

bool ParticleSystem::ifActive(){
	return active;
}
