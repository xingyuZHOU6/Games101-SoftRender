#include "Vertex.h"
void vertex::set_vertexposition(const Eigen::Vector4f& vt) {
	v = vt;
}
void vertex::set_normal(const Eigen::Vector4f& nt) {
	normal = nt;
}
void vertex::set_texcoord(const Eigen::Vector2f& tt) {
	texcoord = tt;
}
void vertex::set_color(const Eigen::Vector3f& ct) {
	color = ct;

}