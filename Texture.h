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
		create_mipmap_texture();//minmap层级创建
	}
	int height;
	int width;
	std::vector<cv::Mat> mipmap_texture_data;
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
	//实现双线性插值
	Eigen::Vector3f getcolor_lerpbinary(float u,float v)
	{
		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);
		float u_img = u * width;
		float v_img = (1 - v) * height;
		int u0 = static_cast<int>(floor(u_img)); //得到u_img v_img 周围四个点的坐标
		int u1 = static_cast<int>(ceil(u_img));
		int v0 = static_cast<int>(floor(v_img));
		int v1 = static_cast<int>(ceil(v_img));
		
		u0 = std::clamp(u0, 0, width - 1);
		u1 = std::clamp(u1, 0, width - 1);
		v0 = std::clamp(v0, 0, height - 1);
		v1 = std::clamp(v1, 0, height - 1);

		auto color00 = img_data.at<cv::Vec3b>(v0, u0); //获取四个像素点的颜色值,注意输入的是y，x，不是x，y
		auto color01 = img_data.at<cv::Vec3b>(v1, u0); 
		auto color10 = img_data.at<cv::Vec3b>(v0, u1);
		auto color11 = img_data.at<cv::Vec3b>(v1, u1);

		float u_ratio = u_img - u0;
		float v_ratio = v_img - v0;

		auto color1 = color00 * (1 - u_ratio) + color10 * u_ratio;
		auto color2 = color01 * (1 - u_ratio) + color11 * u_ratio;

		auto color = color1 * (1 - v_ratio) + color2 * v_ratio;
		return Eigen::Vector3f(color[0],color[1],color[2]);

	}
	
	


	//mipmap层级纹理的生成
	void create_mipmap_texture()
	{
		//1.先得到层级数目
		int mipmap_count = floor(std::log2(std::max(height, width))) + 1;
		//2.生成不同的层级纹理

		//第0层直接添加，方便后续迭代
		mipmap_texture_data.push_back(img_data.clone());

		
		for (int i = 1; i < mipmap_count; i++)
		{
			//获取上一层的宽高,这里不要忘了限制范围
			int curr_width = std::max(1, mipmap_texture_data[i - 1].cols / 2);
			int curr_height = std::max(1, mipmap_texture_data[i - 1].rows / 2);

			// 创建当前层的空图像
			cv::Mat current_level(curr_height, curr_width, img_data.type());

			//使用2*2的盒子进行平均
			for (int y = 0; y < curr_height; y++)
			{
				for (int x = 0; x < curr_width; x++)
				{
					int last_x_0 = x * 2 + 0;
					int last_x_1 = std::min(x * 2 + 1, mipmap_texture_data[i - 1].cols -1);
					int last_y_0 = y *2 + 0;
					int last_y_1 = std::min(y * 2 + 1, mipmap_texture_data[i - 1].rows -1);
					
					auto color00 = mipmap_texture_data[i - 1].at<cv::Vec3b>(last_y_0, last_x_0);
					auto color10 = mipmap_texture_data[i - 1].at<cv::Vec3b>(last_y_0, last_x_1);
					auto color01 = mipmap_texture_data[i - 1].at<cv::Vec3b>(last_y_1, last_x_0);
					auto color11 = mipmap_texture_data[i - 1].at<cv::Vec3b>(last_y_1, last_x_1);
					
					cv::Vec3b avg_color = (color00 + color01 + color10 + color11) / 4;
					current_level.at<cv::Vec3b>(y, x) = avg_color;
					
				}
			}
			//3.
			mipmap_texture_data.push_back(current_level);
			if (curr_height == 1 && curr_width == 1)break;//提前结束循环
		}

	}

	//mipmap方法下获取颜色
	Eigen::Vector3f get_color_mipmap(float u, float v,int mipmap)
	{
		if (mipmap < 0 || mipmap >= mipmap_texture_data.size()) {
			// 处理无效的Mipmap级别
			return Eigen::Vector3f(0, 0, 0);
		}

		cv::Mat& current_mipmap = mipmap_texture_data[mipmap];
		int current_width = current_mipmap.cols;
		int current_height = current_mipmap.rows;


		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);
		float u_img = u * current_width;
		float v_img = (1-v) * current_height;
		int x = static_cast<int>(u_img); //转换为int值
		int y = static_cast<int>(v_img);

		x = std::clamp(x, 0, current_width - 1); //限制x，y的值使其合法
		y = std::clamp(y, 0, current_height - 1);

		auto color = mipmap_texture_data[mipmap].at<cv::Vec3b>(y, x); //获取mipmap层级下的颜色

		return Eigen::Vector3f(color[0], color[1], color[2]);
	}
	

};