#pragma once
#include <Eigen>
class vertex {
public:
	Eigen::Vector4f v; //顶点
	Eigen::Vector4f normal; //法线
	Eigen::Vector2f texcoord; //纹理坐标
	Eigen::Vector3f color; //顶点颜色

	void set_vertexposition(const Eigen::Vector4f& vt); //why need const?
	void set_normal(const Eigen::Vector4f& nt);
	void set_texcoord(const Eigen::Vector2f& tt);
	void set_color(const Eigen::Vector3f& ct);
};