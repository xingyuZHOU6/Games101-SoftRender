#pragma once
#include "Texture.h"
#include "Triangle.h"
#include "VertexShader.h"
#include "FrameBuffer.h"
#include "Global.h"
#include "FragmentShader.h"


class rasterizer {
public:
	//���Զ�����Ļ���
	vertexshader my_vertexshader; //���嶥����ɫ��

	fragmentshader my_fragmentshader; //����ƬԪ��ɫ��

	framebuffer my_framebuffer; //����֡����

	Eigen::Vector4f view_pos[3]; //��ͼ�ռ��λ��Ӧ�ñ����棬��phong shader��isbackingʱ����

	texture* tex = nullptr; //����
	void draw(std::vector<triangle*> trianglelist, float angle);
	rasterizer();
	void raster(triangle& newtri);
	int get_index(int x, int y);
	int get_index_ssaa(int x, int y);//��ó�����������
	void clear();
	bool isbacking(Eigen::Vector4f newtri[3]);
	void sampling(); //����SSAA����

	int ssaa_factor = 2; //������ϵ��

};