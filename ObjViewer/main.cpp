#include <glut.h>
#include "mesh.h"
#include <time.h>

#include "glm.hpp"
#include "transform.hpp"
#include "ParticleSystem.h"

using namespace glm;

Mesh modelSrc;
Mesh modelDest;

ParticleSystem particleSystem({ -1, 3, -1 }, { 1,5, 1 }, 0.01, 0.05, 3, 5);

Mesh *drawMesh = NULL;

vec4 lookVec;
vec4 pitchAxis;
vec4 upAxis;

int lastX = 0, lastY = 0;

enum Mode
{
	MODEL_SRC,
	MODEL_DEST,
	MODEL_ANIMATION,
	PARTICLES,
	MODE_COUNT
};

Mode mode = MODEL_SRC;
bool textMode = false;
bool animateStop = true;

bool initialize(int argc, char **argv)	//初始化
{
	if (argc<2)
	{
		printf("usage ObjViewer objfile.obj\n");
		return false;
	}


	//读取模型
	printf("read obj %s\n", argv[1]);

	modelSrc.readObjFile(argv[1]);
	const static int steps =3000;
	float dt = 0.001;
	printf("compute morphed mesh for modelSrc...\n");
	for (int i = 0; i < steps; ++i)
	{
		modelSrc.updateMesh(dt);
		if (!(i%100))
		{
			printf("\rprocess:%d%%", i*100 / steps);
		}
	}
	modelSrc.generateNormals();
	if (argc>2)
	{
		printf("\nread obj %s\n", argv[2]);
		modelDest.readObjFile(argv[2]);
		printf("compute morphed mesh for modelDest...\n");
		for (int i = 0; i < steps; ++i)
		{
			modelDest.updateMesh(dt);
			if (!(i % 100))
			{
				printf("\rprocess:%d%%", i * 100 / steps);
			}
		}

		printf("compute target morphed mesh for modelDest...\n");
		modelSrc.computeMorphedMesh(modelDest);
		modelDest.computeMorphedMesh(modelSrc);
		modelSrc.generateNormals();


		modelDest.generateNormals();
	}

	//根据模型的大小设置观察的距离
	float r = 5.0;

	//设置观察相关向量
	lookVec = vec4(0, 0, r,0);
	pitchAxis=vec4(-1,0,0,0);
	upAxis = vec4(0, 1, 0,0);

	//设置光源属性

	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat sun_light_position[] = { 100.0f, 100.0f, 0.0f, 1.0f };
	GLfloat sun_light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat sun_light_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse);

	//设置材质数据
	GLfloat meshAmbient[] = { 0.2, 0.2, 0.2, 1.0f };
	GLfloat meshDiffuse[] = { 0.8, 0.8, 0.8, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, meshAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, meshDiffuse);


	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

// 	glEnable(GL_CULL_FACE);
// 	glCullFace(GL_BACK);

	return true;
}

void display()
{
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//设置观察矩阵
	gluLookAt(lookVec.x, lookVec.y,lookVec.z,
		0,0,0,
		upAxis.x, upAxis.y, upAxis.z);

	glColor3f(1.0, 1.0, 1.0);
	//绘图
	if (mode!=MODEL_ANIMATION)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else if (textMode)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else{
		glDisable(GL_TEXTURE_2D);
	}

	if (drawMesh)
	{
		drawMesh->drawMesh();
	}
	if (particleSystem.ifActive()){
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	particleSystem.render();

	glutSwapBuffers();
}

void resize(int w, int h)
{
	//设置视口大小和投影矩阵
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, w / float(h), 0.1, 10000000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouseMove(int x, int y)
{
	int deltaX = x - lastX;
	int deltaY = y - lastY;
	lastX = x;
	lastY = y;

	//计算旋转矩阵，更新观察相关向量
	mat4 rotateX = rotate<float>(deltaY / 200.0, vec3(pitchAxis.x, pitchAxis.y, pitchAxis.z));
	upAxis = rotateX*upAxis;
	lookVec = rotateX*lookVec;

	mat4 rotateY = rotate<float>(-deltaX / 200.0, vec3(0, 1, 0));
	lookVec = rotateY*lookVec;
	pitchAxis = rotateY*pitchAxis;
	upAxis = rotateY*upAxis;

}


float timeRatio = 1.0;

void update()
{
	static int updateTime = 0;
	static float weight = 0.0;
	static auto t=clock();
	float dt = (clock() - t) / 2000.0;
	t = clock();
	static const int animationDuration = 3000;
	int time = 0;
	/*if (clock() % (2 * animationDuration)<animationDuration)
	{
		time = clock() % animationDuration;
	}
	else
	{
		time = 2 * animationDuration - clock() % (2 * animationDuration);
	}*/
	time = clock() % animationDuration;
	
	switch (mode)
	{
	case MODEL_SRC:
		drawMesh = &modelSrc;
		drawMesh->weight = 0;
		break;
	case MODEL_DEST:
		drawMesh = &modelDest;
		drawMesh->weight = 0;
		break;
	case MODEL_ANIMATION:
		/*if (time < animationDuration / 2)
		{
			drawMesh = &modelSrc;
			drawMesh->weight = time / float(animationDuration / 2);
		}
		else
		{
			drawMesh = &modelSrc;
			drawMesh->weight = (animationDuration - time) / float(animationDuration / 2);
		}*/
		if (time>=(animationDuration -10)){
			/*if (animateStop){    
				
			}
		    animateStop = true;*/

			animateStop = false;  // 只变形一遍
			mode = MODEL_DEST;
			drawMesh = &modelDest;
			drawMesh->weight = 0;
			break;
		}
		drawMesh = &modelDest;
		drawMesh->weight = time / float(animationDuration);
		break;
	case PARTICLES:
		drawMesh = &modelDest;
		drawMesh->weight = 0;
		particleSystem.attachToModel(drawMesh);
		particleSystem.update(dt);
		break;
	case MODE_COUNT:
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);


	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("ObjViewer");

	if (!initialize(argc, argv))
	{
		return -1;
	}
	glutDisplayFunc(display);
	glutIdleFunc(update);

	glutMotionFunc(mouseMove);
	glutKeyboardFunc([](unsigned char key,int,int){
		switch (key)
		{
		case 'w':
			timeRatio *= 1.1;
			break;
		case 's':
			timeRatio /= 1.1;
			break;
		case 'z':
		{
			mode = MODEL_SRC;
			particleSystem.setActive(false);
			/*int m = mode;
			mode = Mode((m + 1) % MODE_COUNT);
			particleSystem.setActive(mode == PARTICLES);*/
			break;
		}
		case 'x':
		{
			mode = MODEL_DEST;
			particleSystem.setActive(false);
			break;
		}
		case 'c':
		{
			mode = MODEL_ANIMATION;
			particleSystem.setActive(false);
			break;
		}
		case 'v':
		{
			mode = PARTICLES;
			particleSystem.setActive(true);
			break;
		}
		case 'a':
		{
			textMode = (!textMode);
			break;
		}
		default:
			break;
		}
	});

	glutMouseFunc([](int, int, int x, int y){
		lastX = x, lastY = y;
	});


	glutReshapeFunc(resize);
	glutMainLoop();
	return 0;
}

