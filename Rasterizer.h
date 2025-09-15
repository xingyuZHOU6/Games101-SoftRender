#pragma once
#include "Texture.h"
#include "Triangle.h"
#include "VertexShader.h"
#include "FrameBuffer.h"
#include "Global.h"
#include "FragmentShader.h"
#include "Vertex.h"

enum Plane {
	Positive_w,
	Positvie_x,
	Negative_x,
	Positvie_y,
	Negative_y,
	Positive_z,
	Negative_z,

};

class rasterizer {
public:

	Eigen::Vector3f eye = Eigen::Vector3f(0, 0, 10); //�������λ��
	Eigen::Vector3f target = Eigen::Vector3f(0, 0, -1);//�������߷���
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);//������������Ϸ���

	float size = 10.0f;
	float fov = 45.0f;
	float aspect = 1.0f;
	float zNear = 0.1f;
	float zFar = 100.0f;


	vertexshader my_vertexshader; //���嶥����ɫ��

	fragmentshader my_fragmentshader; //����ƬԪ��ɫ��

	framebuffer my_framebuffer; //����֡����

	Eigen::Vector4f world_pos[3];//��������ռ��λ�ã�Ϊʵ��shadow mapping

	Eigen::Vector4f view_pos[3]; //��ͼ�ռ��λ��Ӧ�ñ����棬��phong shader��isbackingʱ����

	Eigen::Vector4f clip_pos[3]; //�ü��ռ�


	texture* tex = nullptr; //����
	dir_light directionlight = dir_light{Eigen::Vector3f(0,10,10.0f),Eigen::Vector3f(0,0,-1),
		Eigen::Vector3f(500,500,500)}; //ƽ�й�


	rasterizer();
	void draw(std::vector<triangle*> trianglelist, float angle, select_fragmentshader select_myshader);
	void drawF(std::vector<triangle*> trianglelist, float angle);
	void raster(triangle& newtri, select_fragmentshader select_myshader);
	void rasterF(triangle& newtri);
	int get_index(int x, int y);
	int get_index_ssaa(int x, int y);//��ó�����������
	void clear();
	bool isbacking(Eigen::Vector4f newtri[3]);
	void sampling(); //����SSAA����
	int ssaa_factor = 2; //������ϵ��
	bool ismimmap = true;//�Ƿ���mimmap
	std::vector<triangle*> clip(triangle newtri); //�ü�
	int get_minmap_index(triangle newtri, float wa, float wb, float wc); //��ȡminmap�㼶

};