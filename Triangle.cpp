#include "Triangle.h"
void triangle::set_vertexposition(int v_ind, const Eigen::Vector4f& vt) {
	trivertex[v_ind].v = vt;
}
void triangle::set_normal(int v_ind, const Eigen::Vector4f& nt) {
	trivertex[v_ind].normal = nt;
}
void triangle::set_texcoord(int v_ind, const Eigen::Vector2f& tt) {
	trivertex[v_ind].texcoord = tt;
}
void triangle::set_color(int v_ind, const Eigen::Vector3f& ct) {
	trivertex[v_ind].color = ct;

}