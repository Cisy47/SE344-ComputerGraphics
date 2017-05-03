#include "Mesh.h"
#include <algorithm>
#include <atlimage.h>
// using namespace std;

//������������s���зָ�ָ���Ϊdelim
vector<string> split(std::string& s, std::string& delim)
{
	vector<string> ret;
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
	return ret;
}


Mesh::Mesh()
{
	weight=0.0;
}

Mesh::~Mesh()
{
}

int Mesh::triangleCount()
{
	return faces.size();
}

void Mesh::computeMorphedMesh(const Mesh &target)  //����AB��չ�������ģ�Ͷ�Ӧ��ϵ
{
	for (int vIndex = 0; vIndex < morphedVertice.size();++vIndex)
	{
		auto &v=morphedVertice[vIndex];
		float len = length(v - target.morphedVertice[0]);
		auto destV = target.oriVertice[0];
		int destIndex = 0;
		for (int i = 0; i < target.morphedVertice.size();++i) //�ҵ���������ĵ�
		{
			float tmpLen = length(v - target.morphedVertice[i]);
			if (len>tmpLen)
			{
				destIndex = i;
				len = tmpLen;
				destV = target.oriVertice[i];
			}
		}
		v = destV;

		{
			int srcTI = 0, destTI = 0;
			for (auto &srcF : faces)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (srcF.vertexIndex[i] == vIndex)
					{
						srcTI = srcF.textCoordIndex[i];
						break;
					}
				}
			}
			for (auto &destF : target.faces)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (destF.vertexIndex[i] == vIndex)
					{
						destTI = destF.textCoordIndex[i];
						break;
					}
				}
			}
			morphedTextCoords[srcTI] = target.oriTextCoords[destTI];  //������Ӧ��ϵ
		}

	}
	targetTextureID = target.textureID;
}

void Mesh::readObjFile(const char *file, bool cw/*=true*/)
{
	ifstream ifs(file, ios::in);
	string dir = "";
	string mtlFileName;
	string mtlName;
	string textureName;
	char cpath[260] = "";
	strcpy_s(cpath, file);
	bool found = false;
	for (auto iter = strlen(cpath); iter >= 0; --iter)
	{
		if (!found)
		{
			found = cpath[iter] == '\\' || cpath[iter] == '/';
		}
		else if (cpath[iter] != '\\'&&cpath[iter] != '/')
		{
			cpath[iter+1] = 0;
			dir = cpath;
			dir += "\\";
			break;
		}
	}
	if (ifs)
	{
		faces.clear();
		oriVertice.clear();
// 		normals.clear();
		oriTextCoords.clear();
		string line;
		while (getline(ifs, line))//��ȡһ��
		{
			for (auto & c:line)
			{
				if (c=='\t')
				{
					c = ' ';
				}
			}
			auto items = split(line, string(" "));//�ָ�

			//ȥ���յķָ���
			for (auto iter = items.begin(); iter != items.end();)
			{
				if (*iter=="")
				{
					iter = items.erase(iter);
				}
				else
				{
					++iter;
				}
			}
			//v vt vn f �ָ������3
			if (items.size()>=3)
			{
				if (items[0] == "v")	//��ȡ����
				{
					assert(items.size() == 4);
					oriVertice.push_back({ atof(items[1].c_str()), atof(items[2].c_str()), atof(items[3].c_str())});
				}
// 				else if (items[0] == "vn")	//��ȡ����
// 				{
// 					assert(items.size() == 4);
// 					normals.push_back({ atof(items[1].c_str()), atof(items[2].c_str()), atof(items[3].c_str()) });
// 				}
				else if (items[0] == "vt")	//��ȡ��������
				{
					assert(items.size() >= 2);
					oriTextCoords.push_back({ atof(items[1].c_str()), atof(items[2].c_str()) });
				}
				else if (items[0] == "f")	//��ȡƬԪ
				{
					while(adjacentVertice.size()<oriVertice.size())
					{
						adjacentVertice.push_back(vector<int>());
					}
					assert(items.size() >= 4);
					vector<vector<string>> elements;
					for (int i = 1; i < items.size();++i)	//��ƬԪ��Ԫ�ؽ��зָ�
					{
						elements.push_back(split(items[i], string("/")));
					}
					assert(elements[0].size() == 3);
					for (int i = 1; i < elements.size() - 1;++i)	 //�������ƬԪ��תΪ������ƬԪ
					{
						Face face;
						//��һ����
						face.vertexIndex[0] = elements[0][0] == "" ? -1 : atoi(elements[0][0].c_str()) - 1;
						face.textCoordIndex[0] = elements[0][1] == "" ? -1 : atoi(elements[0][1].c_str()) - 1;
						face.normalIndex[0] = elements[0][2] == "" ? -1 : atoi(elements[0][2].c_str()) - 1;

						//�ڶ�����
						face.vertexIndex[1] = elements[i][0] == "" ? -1 : atoi(elements[i][0].c_str()) - 1;
						face.textCoordIndex[1] = elements[i][1] == "" ? -1 : atoi(elements[i][1].c_str()) - 1;
						face.normalIndex[1] = elements[i][2] == "" ? -1 : atoi(elements[i][2].c_str()) - 1;

						//��������
						face.vertexIndex[2] = elements[i + 1][0] == "" ? -1 : atoi(elements[i + 1][0].c_str()) - 1;
						face.textCoordIndex[2] = elements[i + 1][1] == "" ? -1 : atoi(elements[i + 1][1].c_str()) - 1;
						face.normalIndex[2] = elements[i + 1][2] == "" ? -1 : atoi(elements[i + 1][2].c_str()) - 1;

						faces.push_back(face);

						for (int i = 0; i < 3;++i)
						{
							adjacentVertice[face.vertexIndex[i % 3]].push_back(face.vertexIndex[(i + 1) % 3]);
							adjacentVertice[face.vertexIndex[i % 3]].push_back(face.vertexIndex[(i + 2) % 3]);
						}

					}
				}
			}
			else if(items.size()==2)
			{
				if (items[0] == "mtllib")	//��ȡmtl�ļ�
				{
					mtlFileName = items[1];
				}
				else if (items[0] == "usemtl")	//��ȡmtl����
				{
					mtlName = items[1];
				}
			}
		}
	}

	//��ȡ����
	ifstream mtlIF((dir+mtlFileName).c_str(), ios::in);
	if (mtlIF)
	{
		string line;
		while (getline(mtlIF, line))//��ȡһ��
		{
			for (auto & c : line)
			{
				if (c == '\t')
				{
					c = ' ';
				}
			}
			auto items = split(line, string(" "));//�ָ�

			//ȥ���յķָ���
			for (auto iter = items.begin(); iter != items.end();)
			{
				if (*iter == "")
				{
					iter = items.erase(iter);
				}
				else
				{
					++iter;
				}
			}
			static bool found = false;
			if (items.size() >= 2)
			{
				if (!found)
				{
					if (items[0] == "newmtl"&&items[1]==mtlName)	//��ȡ����
					{
						found = true;
					}
					continue;
				}
				else
				{
					if (items[0] == "map_Kd")
					{
						textureName = items[1];
						break;
					}
				}
			}
		}
		mtlIF.close();
	}

	if (!textureName.empty())
	{
		glGenTextures(1, &textureID);
		CImage texture;
		if (SUCCEEDED(texture.Load(CA2W((dir + textureName).c_str()))))
		{
			BITMAP bm;
			HBITMAP hbmp = texture;
			GetObject(hbmp, sizeof(bm), &bm);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glPixelStoref(GL_PACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits); //���ﲻ��GL_RGB  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	verticeVelocity.resize(oriVertice.size());
	morphedTextCoords.resize(oriTextCoords.size());
	calculateCenterAndRadius();

	targetRadius = 2.0;
	elasticLength = 0.5* sqrt((4 * 3.14*targetRadius*targetRadius) / (float)faces.size() * 4 / sqrt(3.0));

	if (!cw)
	{
		for (auto &f:faces)
		{
			swap(f.vertexIndex[1], f.vertexIndex[2]);
		}
	}

}

void Mesh::drawMesh()
{
	if (weight<0.5)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, targetTextureID);
	}

	//��ʼ��ͼ
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < faces.size();++i)
	{
		//���÷���
		vec3 n = (1.0f - weight)*oriFaceNormals[i] + weight*morphedFaceNormals[i];
		glNormal3fv((GLfloat *)&normalize(n));

		for (int j = 0; j < 3;++j)
		{
			//������������
			if (faces[i].textCoordIndex[j] >= 0 && faces[i].textCoordIndex[j] <oriTextCoords.size())
			{
				vec2 t = weight<0.5 ? oriTextCoords[faces[i].textCoordIndex[j]] : morphedTextCoords[faces[i].textCoordIndex[j]];
				glTexCoord2fv((GLfloat *)&t);
			}
			//���ö�������
			if (faces[i].vertexIndex[j] >= 0 && faces[i].vertexIndex[j] <morphedVertice.size())
			{
				vec3 v = (1.0f - weight)*oriVertice[faces[i].vertexIndex[j]] + /*0.3f**/weight*morphedVertice[faces[i].vertexIndex[j]];
				glVertex3fv((GLfloat *)&v);
			}
		}
	}
	glEnd();

}

void Mesh::calculateCenterAndRadius()
{
	vec3 center= { 0, 0, 0 };
	float radius= 0.0;
	for (const auto &v : oriVertice)
	{
		center += v;
	}
	center /= float(oriVertice.size());

	for (const auto &v : oriVertice)
	{
		radius += length(v - center);
	}
	radius /= float(oriVertice.size());

	for (auto & v:oriVertice)  //��һ�� ����ģ�͵ĵ㶼����+�ƶ�����׼��λ
	{
		v -= center;
		v /= radius;
	}
	morphedVertice = oriVertice;
}

float Mesh::getAverageLength(int index)  //����ĳ���㵽�ڵ��ƽ������
{
	const auto &adjacentIndex = adjacentVertice[index];
	float areaLength = 0.0;
	for (auto i : adjacentIndex)
	{
		areaLength += length(morphedVertice[index] - morphedVertice[i]); // ����ĳ���㵽�ܱ����е�ľ����
	}
	return areaLength /(float)adjacentIndex.size();
}

void Mesh::generateNormals()
{
	oriFaceNormals.resize(faces.size());
	morphedFaceNormals = oriFaceNormals;

	for (int i = 0; i < faces.size();++i)
	{
		const auto &f = faces[i];
		{
			vec3 v1 = oriVertice[f.vertexIndex[0]] - oriVertice[f.vertexIndex[1]];
			vec3 v2 = oriVertice[f.vertexIndex[0]] - oriVertice[f.vertexIndex[2]];
			oriFaceNormals[i] = normalize(cross(v1, v2));
		}
		{
			vec3 v1 = morphedVertice[f.vertexIndex[0]] - morphedVertice[f.vertexIndex[1]];
			vec3 v2 = morphedVertice[f.vertexIndex[0]] - morphedVertice[f.vertexIndex[2]];
			morphedFaceNormals[i] = normalize(cross(v1, v2));
		}
	}
}

void Mesh::updateMesh(float dt) //ͨ���ܱߵ����������ģ����ӳ�䵽������
{
	if (dt>0.01)
	{
		dt = 0.01;
	}
	const static float mass = 100.0;
	const static float frictionFactor = 0.1;

	static decltype(morphedVertice) tmpVertice;
	tmpVertice = morphedVertice;

	for (int i = 0; i < morphedVertice.size(); ++i)
	{
		const auto &v = morphedVertice[i];
		const auto &adjacentIndex = adjacentVertice[i];
		vec3 force = { 0, 0, 0 };

		for (auto index : adjacentIndex)   //�����ܱ�ÿ������˵
		{
			float elasticFacotor = 1.0;
			elasticFacotor = getAverageLength(index) / elasticLength; 
			if (elasticFacotor>1.0)
			{
				elasticFacotor *= elasticFacotor*elasticFacotor /**elasticFacotor / 10.0*elasticFacotor / 10.0*/;
			}

			auto posVector = morphedVertice[index] - v;
			auto posVectorLength = length(posVector)+0.5f;
			force += normalize(posVector)*posVectorLength*posVectorLength*posVectorLength*elasticFacotor; //��������
		}

		vec3 normal = normalize(v);
		vec3 friction = -verticeVelocity[i] * length(verticeVelocity[i])*frictionFactor;
		force += friction;

		force -= dot(force, normal)*normal;

		tmpVertice[i] = morphedVertice[i] + dt*verticeVelocity[i];  //������ʹ�����ƶ�

		tmpVertice[i] = normalize(tmpVertice[i])*targetRadius; //��׼��

		verticeVelocity[i] += force / mass;

	}
	morphedVertice = tmpVertice;
}
