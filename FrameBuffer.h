#pragma once
#include <Eigen>
class framebuffer {
	//定义帧缓冲类，不要忘了初始化colorbuffer和depthbuffer
public:
	std::vector<Eigen::Vector3f> colorbuffer;
	std::vector<float> depthbuffer;

	//超采样存储图
	std::vector<Eigen::Vector3f> ssaa_colorbuffer;
	std::vector<float> ssaa_depthbuffer;

	//存储shadow mapping
	std::vector<float>ssaa_shadowbuffer;
	framebuffer();
};