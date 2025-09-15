#pragma once
#include <Eigen>
#include <algorithm>
class vertexshader {
public:
	Eigen::Matrix4f model_matrix; //模型变换矩阵
	Eigen::Matrix4f view_matrix; //视图变换矩阵
	Eigen::Matrix4f projection_matrix; //透视变换矩阵
	Eigen::Matrix4f ortho_matrix; //正交变换矩阵
	Eigen::Matrix4f view_matrix_light; //光源变换矩阵
	Eigen::Matrix4f rotation_matrix; //旋转矩阵
	float init_angle = 150.0f;

	void set_model_matrix(); //设置模型变换矩阵
	void set_view_matrix(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up); //设置视图变换矩阵
	void set_projection_matrix(float fov,float aspect,float zNear,float Zfar); //设置透视变换矩阵
	void set_ortho_matrix(float size, float aspect, float zNear, float zFar);
	void set_view_matrix_light(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up);
	void set_rotation_matrix(float angle);
	vertexshader();
};