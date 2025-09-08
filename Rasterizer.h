#pragma once
#include "Texture.h"
#include "Triangle.h"
#include "VertexShader.h"
#include "FrameBuffer.h"
#include "Global.h"
#include "FragmentShader.h"


class rasterizer {
public:
	//可自定义屏幕宽高
	vertexshader my_vertexshader; //定义顶点着色器

	fragmentshader my_fragmentshader; //定义片元着色器

	framebuffer my_framebuffer; //定义帧缓冲

	Eigen::Vector4f view_pos[3]; //视图空间的位置应该被保存，在phong shader或isbacking时调用

	texture* tex = nullptr; //纹理
	void draw(std::vector<triangle*> trianglelist, float angle);
	rasterizer();
	void raster(triangle& newtri);
	int get_index(int x, int y);
	int get_index_ssaa(int x, int y);//获得超采样的坐标
	void clear();
	bool isbacking(Eigen::Vector4f newtri[3]);
	void sampling(); //进行SSAA采样

	int ssaa_factor = 2; //超采样系数

};