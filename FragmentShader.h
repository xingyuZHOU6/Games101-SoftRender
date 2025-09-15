#pragma once
#include <Eigen>

//此结构体用于保存每个片元的插值信息
struct fragment_payload {
	Eigen::Vector4f view_pos;
	Eigen::Vector4f normal;
	Eigen::Vector2f texcoord;
	Eigen::Vector3f color;
};

//定义点光源
struct light {
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;

};

//定义平行光
struct dir_light {
	Eigen::Vector3f position;
	Eigen::Vector3f lightdir;
	Eigen::Vector3f intensity;

};


//定义一个枚举类，可以选择使用哪个着色器
enum select_fragmentshader {
	test_shader,
	phong_shader,

};
class fragmentshader {
	
public:
	//test shader
	Eigen::Vector3f testshader();

	//phong shader
	Eigen::Vector3f fragment_phong_shader(fragment_payload payload);

	fragmentshader();
};