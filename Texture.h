#pragma once
#include <Eigen>
#include <opencv2/opencv.hpp>
//用于导入纹理
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

	//通过UV坐标获取颜色
	Eigen::Vector3f getcolor(float u,float v) {
		u = std::clamp(u, 0.0f, 1.0f); //将uv坐标限制在0-1区间
		v = std::clamp(v, 0.0f, 1.0f);

		float u_img = u * width; //转换为图像的高度和宽度
		float v_img = (1 - v) * height;

		int x = static_cast<int>(u_img); //转换为int值
		int y = static_cast<int>(v_img);

		x = std::clamp(x, 0, width - 1); //限制x，y的值使其合法
		y = std::clamp(y, 0, height - 1);

		auto color = img_data.at<cv::Vec3b>(y,x);
		return Eigen::Vector3f(color[0], color[1], color[2]);

	}


};