#pragma once
#include <Eigen>
#include <algorithm>
class vertexshader {
public:
	Eigen::Matrix4f model_matrix; //ģ�ͱ任����
	Eigen::Matrix4f view_matrix; //��ͼ�任����
	Eigen::Matrix4f projection_matrix; //͸�ӱ任����
	
	Eigen::Vector3f eye = Eigen::Vector3f(0, 0, 5); //�������λ��
	Eigen::Vector3f target = Eigen::Vector3f(0,0, - 1);//�������߷���
	Eigen::Vector3f up = Eigen::Vector3f(0, 1, 0);//������������Ϸ���
	float fov = 45.0f;
	float aspect = 1.0f;
	float zNear =0.1f;
	float zFar = 100.0f;

	void set_model_matrix(); //����ģ�ͱ任����
	void set_view_matrix(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up); //������ͼ�任����
	void set_projection_matrix(float fov,float aspect,float zNear,float Zfar); //����͸�ӱ任����

	vertexshader();
};