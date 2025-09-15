#pragma once
#include <Eigen>
#include <algorithm>
class vertexshader {
public:
	Eigen::Matrix4f model_matrix; //ģ�ͱ任����
	Eigen::Matrix4f view_matrix; //��ͼ�任����
	Eigen::Matrix4f projection_matrix; //͸�ӱ任����
	Eigen::Matrix4f ortho_matrix; //�����任����
	Eigen::Matrix4f view_matrix_light; //��Դ�任����
	Eigen::Matrix4f rotation_matrix; //��ת����
	float init_angle = 150.0f;

	void set_model_matrix(); //����ģ�ͱ任����
	void set_view_matrix(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up); //������ͼ�任����
	void set_projection_matrix(float fov,float aspect,float zNear,float Zfar); //����͸�ӱ任����
	void set_ortho_matrix(float size, float aspect, float zNear, float zFar);
	void set_view_matrix_light(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up);
	void set_rotation_matrix(float angle);
	vertexshader();
};