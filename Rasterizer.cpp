#include <iostream>
#include "Rasterizer.h"



//背部剔除
bool rasterizer::isbacking(Eigen::Vector4f newtri[3])
{
	Eigen::Vector4f edge1 = newtri[1] - newtri[0];
	Eigen::Vector3f edge1_ = Eigen::Vector3f(edge1.x(), edge1.y(), edge1.z());
	Eigen::Vector4f edge2 = newtri[2] - newtri[0];
	Eigen::Vector3f edge2_ = Eigen::Vector3f(edge2.x(), edge2.y(), edge2.z());
	//视线方向，描述为三角形指向摄像机，与光线的观察方向是一样的
	Eigen::Vector3f viewdir = Eigen::Vector3f(-newtri[0].x(), -newtri[0].y(), -newtri[0].z()); //这里指向三角形的一个点即可
	return edge1_.cross(edge2_).dot(viewdir) <= 0;
}


void rasterizer::draw(std::vector<triangle*> trianglelist,float angle)
{
	//draw函数将执行整个渲染流程
	Eigen::Matrix4f rotation_matrix;
	rotation_matrix << cos((angle + 30.0f) * 3.1415926535 / 180.0f),0, sin((angle+30.0f) * 3.1415926535 / 180.0f), 0,
		0, 1, 0, 0,
		-sin((angle + 30.0f) * 3.1415926535 / 180.0f), 0, cos((angle + 30.0f) * 3.1415926535 / 180.0f), 0,
		0, 0, 0, 1;
	
	Eigen::Matrix4f MVP = my_vertexshader.projection_matrix *my_vertexshader.view_matrix * rotation_matrix* my_vertexshader.model_matrix;
	for (auto tri : trianglelist)  
	{
		
		/*if (count == 300)break;
		count++;*/
		triangle newtri = *tri; //先把triangle指针转换到值


		//1.进行MVP变换
		int index = 0;
		for (auto& newvertex : newtri.v)
		{
			//保存视图空间下的三角形
			view_pos[index] = (my_vertexshader.view_matrix * rotation_matrix * my_vertexshader.model_matrix * newvertex);
			index++;
			//MVP变换
			newvertex = MVP * newvertex; 
			

		}
		
		

		//2.裁剪步骤先跳过（后续再改进的时候，不要直接使用三角形存储，而使用顶点，若直接使用三角形
		// 则无法进行裁剪）
		
		//3.透视除法
		for (auto& newvertex : newtri.v)
		{
			newvertex.x() /= newvertex.w();
			newvertex.y() /= newvertex.w();
			newvertex.z() /= newvertex.w();
		}
		//4.转换为屏幕空间
		for (auto& newvertex : newtri.v)
		{
			newvertex.x() = (newvertex.x() + 1.0f) /2.0f* WIDTH;
			newvertex.y() = (newvertex.y() + 1.0f) / 2.0f*HEIGHT;
			newvertex.z() = (newvertex.z() + 1.0f) / 2.0f;
		}
		//5.图元装配，将离散的顶点转换为三角形，这里已经实现，因为就是使用三角形进行存储的

		//6.光栅化
		raster(newtri);


		
	}
	//7.进行超采样
	sampling();
}
rasterizer::rasterizer()
{
	my_framebuffer.colorbuffer.resize(WIDTH * HEIGHT, Eigen::Vector3f(0, 0, 0));
	my_framebuffer.depthbuffer.resize(WIDTH * HEIGHT, std::numeric_limits<float>::max());

	//超采样初始化
	my_framebuffer.ssaa_colorbuffer.resize(WIDTH * HEIGHT*ssaa_factor*ssaa_factor, Eigen::Vector3f(0, 0, 0));
	my_framebuffer.ssaa_depthbuffer.resize(WIDTH * HEIGHT*ssaa_factor*ssaa_factor, std::numeric_limits<float>::max());
}
//通过叉乘的方法判断点是否在三角形的内部
bool isinserttriangle(float pixel_x, float pixel_y, triangle& newtri) //这里一定是float类型，如果是int，则SSAA失效
{
	Eigen::Vector2f p(pixel_x, pixel_y);
	Eigen::Vector2f a = Eigen::Vector2f(newtri.v[0].x(), newtri.v[0].y());
	Eigen::Vector2f b = Eigen::Vector2f(newtri.v[1].x(), newtri.v[1].y());
	Eigen::Vector2f c = Eigen::Vector2f(newtri.v[2].x(), newtri.v[2].y());
	float cross1 = (a - b).x() * (p - a).y() - (a - b).y() * (p - a).x();
	float cross2 = (b-c).x() * (p - b).y() - (b - c).y() * (p -b).x();
	float cross3 = (c-a).x() * (p - c).y() - (c - a).y() * (p - c).x();
	return ((cross1>=0&& cross2>= 0&&cross3>0)||
		(cross1 <= 0 && cross2 <= 0 && cross3 <=0));
}
//重心坐标求三个比例.
std::tuple<float, float, float> barycentic(float pixel_x, float pixel_y, triangle& newtri)
{
	Eigen::Vector2f ap = Eigen::Vector2f(pixel_x - newtri.v[0].x(), pixel_y - newtri.v[0].y());
	Eigen::Vector2f ac = Eigen::Vector2f(newtri.v[2].x() - newtri.v[0].x(), newtri.v[2].y() - newtri.v[0].y());
	Eigen::Vector2f ab = Eigen::Vector2f(newtri.v[1].x() - newtri.v[0].x(), newtri.v[1].y() - newtri.v[0].y());
	float factor = (ac.x() * ab.y() - ac.y() * ab.x());
	float wc = (ap.x() * ab.y() - ap.y() * ab.x()) /factor ;
	float wb = (ap.y() * ac.x() -ap.x() * ac.y()) /factor;
	float wa = 1 - wc - wb;
	return std::tuple<float, float, float>(wa, wb, wc);
}

//返回colorbuffer或depthbuffer的索引
 int rasterizer::get_index(int x, int y)
{
	return (HEIGHT - 1 -y)*WIDTH + x;
}

 int rasterizer::get_index_ssaa(int x, int y) {
	 return (HEIGHT * ssaa_factor - 1 - y) * WIDTH * ssaa_factor + x;
 }

//光栅化函数 (使用了SSAA)
void rasterizer::raster(triangle& newtri) {
	//先确定AABB包围盒
	float xmin = std::max(0.0f, std::min(newtri.v[0].x(), std::min(newtri.v[1].x(), newtri.v[2].x())))*ssaa_factor;
	float xmax = std::min(float(WIDTH-1), std::max(newtri.v[0].x(), std::max(newtri.v[1].x(), newtri.v[2].x())))*ssaa_factor;
	float ymin = std::max(0.0f, std::min(newtri.v[0].y(), std::min(newtri.v[1].y(), newtri.v[2].y())))*ssaa_factor;
	float ymax = std::min(float(HEIGHT - 1), std::max(newtri.v[0].y(), std::max(newtri.v[1].y(), newtri.v[2].y())))*ssaa_factor;

	//遍历所有的像素
	for (int y = floor(ymin); y < ceil(ymax); y++)
	{
		for (int x = floor(xmin); x < ceil(xmax); x++)
		{
			float pixel_x = (x + 0.5)/ssaa_factor;
			float pixel_y =(y + 0.5)/ssaa_factor;

			//判断点是否在三角形的内部
			if (isinserttriangle(pixel_x, pixel_y, newtri)) {
				

				//重心坐标求比例
				auto [alpha,gamma,beta] = barycentic(pixel_x,pixel_y,newtri);
				//获得真实的插值比例
				float wp = 1.0f/(alpha / newtri.v[0].w() + beta / newtri.v[1].w() + gamma / newtri.v[2].w());
				float wa = alpha / newtri.v[0].w() * wp;
				float wb = beta / newtri.v[1].w() * wp;
				float wc = gamma / newtri.v[2].w() * wp;
				//先获得插值的Z值，用于判断是否能进行着色
				float zdepth = newtri.v[0].z() * wa + newtri.v[1].z() * wb + newtri.v[2].z() * wc;
				
				//进行背部剔除
				if (isbacking(view_pos))
				{
					continue;
				}
				//仅当满足此条件时，可进行渲染
				if (zdepth < my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)])
				{
					//深度更新
					my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)] = zdepth;

					//此处进行插值属性，然后调用片元着色
					
					//-1 插值坐标
					Eigen::Vector4f lerp_view_pos = view_pos[0] * wa + view_pos[1] * wb + view_pos[2] * wc;

					//-2 插值法线
					Eigen::Vector4f lerp_normal = newtri.normal[0] * wa + newtri.normal[1] * wb + newtri.normal[2] * wc;

					// -3 插值纹理坐标
					
					Eigen::Vector2f lerp_texcoord = newtri.texcoord[0] * wa + newtri.texcoord[1] * wb + newtri.texcoord[2] * wc;

					fragment_payload payload;
					payload.view_pos = lerp_view_pos;
					payload.normal = lerp_normal;
					payload.texcoord = lerp_texcoord;
					payload.color = tex->getcolor(lerp_texcoord.x(), lerp_texcoord.y());
					//调用不同的片元着色器
					Eigen::Vector3f fragment_color = my_fragmentshader.fragment_phong_shader(payload);
					
					//最终的颜色确定
					my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x, y)] =fragment_color;
				}
			}

		}
	}
}

void rasterizer::clear() {
	//刷新帧缓存
	std::fill(my_framebuffer.colorbuffer.begin(), my_framebuffer.colorbuffer.end(), Eigen::Vector3f(0, 0, 0));
	std::fill(my_framebuffer.depthbuffer.begin(), my_framebuffer.depthbuffer.end(), std::numeric_limits<float>::max());
	
	//超采样
	std::fill(my_framebuffer.ssaa_colorbuffer.begin(), my_framebuffer.ssaa_colorbuffer.end(), Eigen::Vector3f(0, 0, 0));
	std::fill(my_framebuffer.ssaa_depthbuffer.begin(), my_framebuffer.ssaa_depthbuffer.end(), std::numeric_limits<float>::max());
}

//采样函数
void rasterizer::sampling() {
	//如果ssaa_factor为1，那么可以直接将ssaa_colorbuffer复制到colorbuffer中，而不需要做平均
	if (ssaa_factor == 1) {
		std::copy(my_framebuffer.ssaa_colorbuffer.begin(), my_framebuffer.ssaa_colorbuffer.end(), my_framebuffer.colorbuffer.begin());
		return;
	}
	//如果ssaa_factor不为1，则需要对颜色进行平均
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			//我们要先得到ssaa_colorbuffer下的颜色值
			int x_ssaa = x * ssaa_factor;
			int y_ssaa = y * ssaa_factor;
			Eigen::Vector3f avg_color{ 0,0,0 };
			for (int dy = 0; dy < 2; dy++)
			{
				for (int dx = 0; dx < 2; dx++)
				{
					
					avg_color += my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x_ssaa + dx, y_ssaa + dy)];
				}
			}
			//平均avg_color
			avg_color = Eigen::Vector3f(avg_color.x() / ssaa_factor / ssaa_factor, avg_color.y() / ssaa_factor / ssaa_factor, avg_color.z() / ssaa_factor / ssaa_factor);
			my_framebuffer.colorbuffer[get_index(x, y)] = avg_color;
		}
	}


}
