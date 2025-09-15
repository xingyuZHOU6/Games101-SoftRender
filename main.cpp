#include <iostream>
#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "Triangle.h"
#include "OBJ_Loader.h"

using namespace std;
int main()
{
	
	std::vector<triangle*> trianglelist; //����洢�Ľṹ
	std::vector<triangle*>planelist;
	
	//����Games101�е�Obj_Loader
	std::string filename = "output.png";
	objl::Loader Loader;
	std::string obj_path = "./models/spot/";
	bool loadout = Loader.LoadFile("./models/spot/spot_triangulated_good.obj");

	//����·��
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
	//���һ��ƽ�棬���ĸ����㹹�ɣ�Ϊʵ����ӰЧ����׼��
	vertex v0, v1, v2, v3;
	//����λ��
	v0.v = Eigen::Vector4f(-2.5f, -1.5f, -3.5f, 1.0f);
	v3.v = Eigen::Vector4f(2.5f, -1.5f, -3.5f, 1.0f);
	v2.v = Eigen::Vector4f(2.5f, -1.5f, 3.5f, 1.0f);
	v1.v = Eigen::Vector4f(-2.5f, -1.5f,3.5f, 1.0f);
	//���ߣ�ȫ����ֱ����
	v0.normal = Eigen::Vector4f(0, 0.0f,-1.0f, 0);
	v1.normal = Eigen::Vector4f(0, 0.0f,-1.0f, 0);
	v2.normal = Eigen::Vector4f(0, 0.0f,-1.0f, 0);
	v3.normal = Eigen::Vector4f(0, 0.0f, -1.0f, 0);
	
	//��ɫ�뷨�߶�����Ҫ���ã�testshader�н�ֱ�ӷ�����ɫ
	triangle* tri1 = new triangle();
	tri1->trivertex[0] = v0;
	tri1->trivertex[1] = v1;
	tri1->trivertex[2] = v2;
	planelist.push_back(tri1);
	triangle* tri2 = new triangle();
	tri2->trivertex[0] = v0;
	tri2->trivertex[1] = v2;
	tri2->trivertex[2] = v3;
	planelist.push_back(tri2);
	
	
	int key = 0;
	int framecount = 0;
	//��ʼ����դ��ʾ��
	rasterizer my_rasterizer;
	texture t(obj_path+texture_path); //��ʼ��texture����
	
	my_rasterizer.tex = &t; //��texture����ĵ�ַ����
	
	float angle = 0;

	while (key != 27)
	{
		
		my_rasterizer.clear();

		//1.����Ⱦģ��֮ǰ��������Ҫ�ȶԹ�Դ����һ���դ������ù�Դ�������ͼ������������ʹ��ƽ�й�
		std::cout << "��Ⱦģ��shadow mapping..." << std::endl;
		my_rasterizer.drawF(trianglelist, angle);
		std::cout << std::endl;
		std::cout << "��Ⱦƽ��shadow mapping..." << std::endl;
		my_rasterizer.drawF(planelist, angle);
		std::cout << std::endl;

		//2.ͨ���������Ⱦģ��
		std::cout << "��Ⱦģ��..." << std::endl;
		my_rasterizer.draw(trianglelist,angle,select_fragmentshader::phong_shader);//����phong shaderȥ��Ⱦ������
		std::cout << std::endl;
		std::cout << "��Ⱦƽ��..." << std::endl;
		my_rasterizer.draw(planelist,angle,select_fragmentshader::test_shader);//����test shaderȥ��Ⱦƽ��

		//3.����ͼ��
		cv::Mat image(HEIGHT, WIDTH, CV_32FC3, my_rasterizer.my_framebuffer.colorbuffer.data());
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