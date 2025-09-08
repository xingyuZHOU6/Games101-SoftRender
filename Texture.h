#pragma once
#include <Eigen>
#include <opencv2/opencv.hpp>
//���ڵ�������
class texture {
private:
	cv::Mat img_data;
public:
	texture(const std::string& name) {
		img_data = cv::imread(name);
		cv::cvtColor(img_data, img_data, cv::COLOR_RGB2BGR);
		width = img_data.cols;
		height = img_data.rows;

	}
	int height;
	int width;

	//ͨ��UV�����ȡ��ɫ
	Eigen::Vector3f getcolor(float u,float v) {
		u = std::clamp(u, 0.0f, 1.0f); //��uv����������0-1����
		v = std::clamp(v, 0.0f, 1.0f);

		float u_img = u * width; //ת��Ϊͼ��ĸ߶ȺͿ��
		float v_img = (1 - v) * height;

		int x = static_cast<int>(u_img); //ת��Ϊintֵ
		int y = static_cast<int>(v_img);

		x = std::clamp(x, 0, width - 1); //����x��y��ֵʹ��Ϸ�
		y = std::clamp(y, 0, height - 1);

		auto color = img_data.at<cv::Vec3b>(y,x);
		return Eigen::Vector3f(color[0], color[1], color[2]);

	}


};