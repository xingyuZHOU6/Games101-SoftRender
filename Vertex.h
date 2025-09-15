#pragma once
#include <Eigen>
class vertex {
public:
	Eigen::Vector4f v; //����
	Eigen::Vector4f normal; //����
	Eigen::Vector2f texcoord; //��������
	Eigen::Vector3f color; //������ɫ

	void set_vertexposition(const Eigen::Vector4f& vt); //why need const?
	void set_normal(const Eigen::Vector4f& nt);
	void set_texcoord(const Eigen::Vector2f& tt);
	void set_color(const Eigen::Vector3f& ct);
};