#include <iostream>
#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "Triangle.h"
#include "OBJ_Loader.h"
using namespace std;
int main()
{
	
	std::vector<triangle*> trianglelist; //定义存储的结构
	
	//借用Games101中的Obj_Loader
	std::string filename = "output.png";
	objl::Loader Loader;
	std::string obj_path = "./models/spot/";
	bool loadout = Loader.LoadFile("./models/spot/Crate1.obj");

	//纹理路径
	auto texture_path = "spot_texture.png";

	for (auto mesh : Loader.LoadedMeshes)
	{
		for (int i = 0; i < mesh.Vertices.size(); i += 3)
		{
			triangle* t = new triangle();
			for (int j = 0; j < 3; j++)
			{
				t->set_vertexposition(j, Eigen::Vector4f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z, 1.0));
				t->set_normal(j, Eigen::Vector4f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z,0.0f));
				t->set_texcoord(j, Eigen::Vector2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));
			}
			trianglelist.push_back(t);
		}
	}
	int key = 0;
	int framecount = 0;
	//初始化光栅化示例
	rasterizer my_rasterizer;
	texture t(obj_path+texture_path); //初始化texture对象
	
	my_rasterizer.tex = &t; //将texture对象的地址传入
	
	float angle = 0;

	while (key != 27)
	{
		my_rasterizer.clear();
		//r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
		my_rasterizer.draw(trianglelist,angle);
		cv::Mat image(700, 700, CV_32FC3, my_rasterizer.my_framebuffer.colorbuffer.data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

		cv::imshow("image", image);
		cv::imwrite(filename, image);
		key = cv::waitKey(5);
		if (key == 'a')
		{
			angle -= 30;
		}
		else if (key == 'd')
		{
			angle +=30;
		}
	}
	return 0;

}