#pragma once
#include <Eigen>

//�˽ṹ�����ڱ���ÿ��ƬԪ�Ĳ�ֵ��Ϣ
struct fragment_payload {
	Eigen::Vector4f view_pos;
	Eigen::Vector4f normal;
	Eigen::Vector2f texcoord;
	Eigen::Vector3f color;
};

//������Դ
struct light {
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;

};

//����ƽ�й�
struct dir_light {
	Eigen::Vector3f position;
	Eigen::Vector3f lightdir;
	Eigen::Vector3f intensity;

};


//����һ��ö���࣬����ѡ��ʹ���ĸ���ɫ��
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