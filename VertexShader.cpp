#include "VertexShader.h"
void vertexshader::set_model_matrix()
{
	model_matrix = Eigen::Matrix4f::Identity(); //将模型变换设置为单位矩阵
}

void vertexshader::set_view_matrix(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up)
{
	//视图变换矩阵的实现
	Eigen::Vector3f forward = (target - eye).normalized(); //定义-Z方向
	Eigen::Vector3f t = up.normalized(); //规范上方向
	Eigen::Vector3f right = forward.cross(t).normalized(); //定义右方向
	Eigen::Vector3f realup = right.cross(forward).normalized(); //重新定义新的上方向
	view_matrix << right.x(), right.y(), right.z(), -eye.dot(right),
		realup.x(), realup.y(), realup.z(), -eye.dot(realup),
		-forward.x(), -forward.y(), -forward.z(), eye.dot(forward), 
	0, 0, 0, 1;
}
void vertexshader::set_view_matrix_light(Eigen::Vector3f eye, Eigen::Vector3f target, Eigen::Vector3f up)
{
	//视图变换矩阵的实现
	Eigen::Vector3f forward = (target - eye).normalized(); //定义-Z方向
	Eigen::Vector3f t = up.normalized(); //规范上方向
	Eigen::Vector3f right = forward.cross(t).normalized(); //定义右方向
	Eigen::Vector3f realup = right.cross(forward).normalized(); //重新定义新的上方向
	view_matrix_light << right.x(), right.y(), right.z(), -eye.dot(right),
		realup.x(), realup.y(), realup.z(), -eye.dot(realup),
		-forward.x(), -forward.y(), -forward.z(), eye.dot(forward),
		0, 0, 0, 1;
}
void vertexshader::set_projection_matrix(float fov, float aspect, float zNear, float zFar)
{
	//投影变换矩阵的实现
	float half_fov = std::tan(fov * 3.1415926535 / 180 / 2);
	projection_matrix << 1.0f / half_fov / aspect, 0, 0, 0,
		0, 1.0f / half_fov, 0, 0,
		0, 0, - (zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0, 0, -1, 0;
}

void vertexshader::set_ortho_matrix(float size,float aspect,float zNear,float zFar)
{
	//正交矩阵的实现
	float half_size = size / 2.0f;
	ortho_matrix << 1.0f / half_size / aspect, 0, 0, 0,
		0, 1.0f / half_size, 0, 0,
		0, 0, -2.0f / (zFar - zNear), -(zFar + zNear) / (zFar - zNear),
		0, 0, 0, 1;
}
void vertexshader::set_rotation_matrix(float angle)
{
	rotation_matrix << cos((angle + init_angle) * 3.1415926535 / 180.0f), 0, sin((angle + init_angle) * 3.1415926535 / 180.0f), 0,
		0, 1, 0, 0,
		-sin((angle + init_angle) * 3.1415926535 / 180.0f), 0, cos((angle + init_angle) * 3.1415926535 / 180.0f), 0,
		0, 0, 0, 1;
}

vertexshader::vertexshader()
{
	set_model_matrix();
}
