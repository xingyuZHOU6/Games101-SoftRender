#include "Triangle.h"
void triangle::set_vertexposition(int v_ind, const Eigen::Vector4f& vt) {
	v[v_ind] = vt;
}
void triangle::set_normal(int v_ind, const Eigen::Vector4f& nt) {
	normal[v_ind] = nt;
}
void triangle::set_texcoord(int v_ind, const Eigen::Vector2f& tt) {
	texcoord[v_ind] = tt;
}
void triangle::set_color(int v_ind, const Eigen::Vector3f& ct) {
	color[v_ind] = ct;

}