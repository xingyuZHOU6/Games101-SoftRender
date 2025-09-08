#pragma once
#include <Eigen>
#include <algorithm>
class vertexshader {
public:
	Eigen::Matrix4f model_matrix; //模型变换矩阵
	Eigen::Matrix4f view_matrix; //视图变换矩阵
	Eigen::Matrix4f projection_matrix; //透视变换矩阵
	
	Eigen::Vector3f eye = Eigen::Vector3f(0, 0, 5); //定义相机位置
	Eigen::Vector3f target = Eigen::Vector3f(0,0, - 1);//定义视线方向
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);//定义相机的向上方向
	float fov = 45.0f;
	float aspect = 1.0f;
	float zNear =0.1f;
	float zFar = 100.0f;

	void set_model_matrix(); //设置模型变换矩阵
	void set_view_matrix(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up); //设置视图变换矩阵
	void set_projection_matrix(float fov,float aspect,float zNear,float Zfar); //设置透视变换矩阵

	vertexshader();
};