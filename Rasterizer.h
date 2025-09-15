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

	Eigen::Vector3f eye = Eigen::Vector3f(0, 0, 10); //定义相机位置
	Eigen::Vector3f target = Eigen::Vector3f(0, 0, -1);//定义视线方向
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);//定义相机的向上方向

	float size = 10.0f;
	float fov = 45.0f;
	float aspect = 1.0f;
	float zNear = 0.1f;
	float zFar = 100.0f;


	vertexshader my_vertexshader; //定义顶点着色器

	fragmentshader my_fragmentshader; //定义片元着色器

	framebuffer my_framebuffer; //定义帧缓冲

	Eigen::Vector4f world_pos[3];//保存世界空间的位置，为实现shadow mapping

	Eigen::Vector4f view_pos[3]; //视图空间的位置应该被保存，在phong shader或isbacking时调用

	Eigen::Vector4f clip_pos[3]; //裁剪空间


	texture* tex = nullptr; //纹理
	dir_light directionlight = dir_light{Eigen::Vector3f(0,10,10.0f),Eigen::Vector3f(0,0,-1),
		Eigen::Vector3f(500,500,500)}; //平行光


	rasterizer();
	void draw(std::vector<triangle*> trianglelist, float angle, select_fragmentshader select_myshader);
	void drawF(std::vector<triangle*> trianglelist, float angle);
	void raster(triangle& newtri, select_fragmentshader select_myshader);
	void rasterF(triangle& newtri);
	int get_index(int x, int y);
	int get_index_ssaa(int x, int y);//获得超采样的坐标
	void clear();
	bool isbacking(Eigen::Vector4f newtri[3]);
	void sampling(); //进行SSAA采样
	int ssaa_factor = 2; //超采样系数
	bool ismimmap = true;//是否开启mimmap
	std::vector<triangle*> clip(triangle newtri); //裁剪
	int get_minmap_index(triangle newtri, float wa, float wb, float wc); //获取minmap层级

};