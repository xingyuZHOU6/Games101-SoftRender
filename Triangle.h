#pragma once
#include <Eigen>
class triangle {
public:
	Eigen::Vector4f v[3]; //����
	Eigen::Vector4f normal[3]; //����
	Eigen::Vector2f texcoord[3]; //��������
	Eigen::Vector3f color[3]; //������ɫ

	void set_vertexposition(int v_ind,const Eigen::Vector4f& vt); //why need const?
	void set_normal(int v_ind, const Eigen::Vector4f& nt);
	void set_texcoord(int v_ind, const Eigen::Vector2f& tt);
	void set_color(int v_ind, const Eigen::Vector3f& ct);
};