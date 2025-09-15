#pragma once
#include <Eigen>
class framebuffer {
	//����֡�����࣬��Ҫ���˳�ʼ��colorbuffer��depthbuffer
public:
	std::vector<Eigen::Vector3f> colorbuffer;
	std::vector<float> depthbuffer;

	//�������洢ͼ
	std::vector<Eigen::Vector3f> ssaa_colorbuffer;
	std::vector<float> ssaa_depthbuffer;

	//�洢shadow mapping
	std::vector<float>ssaa_shadowbuffer;
	framebuffer();
};