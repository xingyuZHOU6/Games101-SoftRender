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
class fragmentshader {
	
public:
	//test shader
	Eigen::Vector3f testshader();

	//phong shader
	Eigen::Vector3f fragment_phong_shader(fragment_payload payload);

	//fragment_payload
	fragment_payload payload;
	fragmentshader();
};