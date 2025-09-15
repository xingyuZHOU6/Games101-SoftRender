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
		create_mipmap_texture();//minmap�㼶����
	}
	int height;
	int width;
	std::vector<cv::Mat> mipmap_texture_data;
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
	//ʵ��˫���Բ�ֵ
	Eigen::Vector3f getcolor_lerpbinary(float u,float v)
	{
		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);
		float u_img = u * width;
		float v_img = (1 - v) * height;
		int u0 = static_cast<int>(floor(u_img)); //�õ�u_img v_img ��Χ�ĸ��������
		int u1 = static_cast<int>(ceil(u_img));
		int v0 = static_cast<int>(floor(v_img));
		int v1 = static_cast<int>(ceil(v_img));
		
		u0 = std::clamp(u0, 0, width - 1);
		u1 = std::clamp(u1, 0, width - 1);
		v0 = std::clamp(v0, 0, height - 1);
		v1 = std::clamp(v1, 0, height - 1);

		auto color00 = img_data.at<cv::Vec3b>(v0, u0); //��ȡ�ĸ����ص����ɫֵ,ע���������y��x������x��y
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
	
	


	//mipmap�㼶���������
	void create_mipmap_texture()
	{
		//1.�ȵõ��㼶��Ŀ
		int mipmap_count = floor(std::log2(std::max(height, width))) + 1;
		//2.���ɲ�ͬ�Ĳ㼶����

		//��0��ֱ����ӣ������������
		mipmap_texture_data.push_back(img_data.clone());

		
		for (int i = 1; i < mipmap_count; i++)
		{
			//��ȡ��һ��Ŀ��,���ﲻҪ�������Ʒ�Χ
			int curr_width = std::max(1, mipmap_texture_data[i - 1].cols / 2);
			int curr_height = std::max(1, mipmap_texture_data[i - 1].rows / 2);

			// ������ǰ��Ŀ�ͼ��
			cv::Mat current_level(curr_height, curr_width, img_data.type());

			//ʹ��2*2�ĺ��ӽ���ƽ��
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
			if (curr_height == 1 && curr_width == 1)break;//��ǰ����ѭ��
		}

	}

	//mipmap�����»�ȡ��ɫ
	Eigen::Vector3f get_color_mipmap(float u, float v,int mipmap)
	{
		if (mipmap < 0 || mipmap >= mipmap_texture_data.size()) {
			// ������Ч��Mipmap����
			return Eigen::Vector3f(0, 0, 0);
		}

		cv::Mat& current_mipmap = mipmap_texture_data[mipmap];
		int current_width = current_mipmap.cols;
		int current_height = current_mipmap.rows;


		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);
		float u_img = u * current_width;
		float v_img = (1-v) * current_height;
		int x = static_cast<int>(u_img); //ת��Ϊintֵ
		int y = static_cast<int>(v_img);

		x = std::clamp(x, 0, current_width - 1); //����x��y��ֵʹ��Ϸ�
		y = std::clamp(y, 0, current_height - 1);

		auto color = mipmap_texture_data[mipmap].at<cv::Vec3b>(y, x); //��ȡmipmap�㼶�µ���ɫ

		return Eigen::Vector3f(color[0], color[1], color[2]);
	}
	

};