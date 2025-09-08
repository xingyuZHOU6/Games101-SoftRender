#pragma once
#include <Eigen>
class triangle {
public:
	Eigen::Vector4f v[3]; //顶点
	Eigen::Vector4f normal[3]; //法线
	Eigen::Vector2f texcoord[3]; //纹理坐标
	Eigen::Vector3f color[3]; //顶点颜色

	void set_vertexposition(int v_ind,const Eigen::Vector4f& vt); //why need const?
	void set_normal(int v_ind, const Eigen::Vector4f& nt);
	void set_texcoord(int v_ind, const Eigen::Vector2f& tt);
	void set_color(int v_ind, const Eigen::Vector3f& ct);
};