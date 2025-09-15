#pragma once
#include <Eigen>
#include "Vertex.h"
class triangle {
public:
	
	vertex trivertex[3];
	void set_vertexposition(int v_ind,const Eigen::Vector4f& vt); //why need const?
	void set_normal(int v_ind, const Eigen::Vector4f& nt);
	void set_texcoord(int v_ind, const Eigen::Vector2f& tt);
	void set_color(int v_ind, const Eigen::Vector3f& ct);
};