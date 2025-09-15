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

//点是否可见
bool isvisable(Eigen::Vector4f v)
{
	return (std::abs(v.x()) <= v.w() && std::abs(v.y()) <= v.w() && std::abs(v.z()) <= v.w());
}

//检查点是否在平面内
bool isinsertplane(Eigen::Vector4f v,Plane plane)
{
	switch (plane)
	{
	case Positive_w:
		return v.w() >= 0;
	case Positvie_x:
		return v.x() <= v.w();
	case Negative_x:
		return v.x() >= -v.w();
	case Positvie_y:
		return v.y() <= v.w();
	case Negative_y:
		return v.y() >= -v.w();
	case Positive_z:
		return v.z() <= v.w();
	case Negative_z:
		return v.z() >= -v.w();
	}
}
//求交点
float getratio(vertex prevertex,vertex curvertex,Plane plane)
{
	switch (plane)
	{
	case Positive_w:
		return (prevertex.v.w() - 0.0f) / (prevertex.v.w() - curvertex.v.w());
	case Positvie_x:
		return (prevertex.v.w() - prevertex.v.x()) / ((prevertex.v.w() - prevertex.v.x()) - (curvertex.v.w() - curvertex.v.x()));
	case Negative_x:
		return (prevertex.v.w() + prevertex.v.x()) / ((prevertex.v.w() + prevertex.v.x()) - (curvertex.v.w() + curvertex.v.x()));
	case Positvie_y:
		return (prevertex.v.w() - prevertex.v.y()) / ((prevertex.v.w() - prevertex.v.y()) - (curvertex.v.w() - curvertex.v.y()));
	case Negative_y:
		return (prevertex.v.w() + prevertex.v.y()) / ((prevertex.v.w() + prevertex.v.y()) - (curvertex.v.w() + curvertex.v.y()));
	case Positive_z:
		return (prevertex.v.w() - prevertex.v.z()) / ((prevertex.v.w() - prevertex.v.z()) - (curvertex.v.w() - curvertex.v.z()));
	case Negative_z:
		return (prevertex.v.w() + prevertex.v.z()) / ((prevertex.v.w() + prevertex.v.z()) - (curvertex.v.w() + curvertex.v.z()));
	}

}
vertex lerpvertex(vertex prevertex, vertex curvertex,float ratio)
{
	//插值属性
	vertex result;
	result.v = prevertex.v * (1 - ratio) + curvertex.v * ratio;
	result.normal = prevertex.normal * (1 - ratio) + curvertex.normal * ratio;
	result.texcoord = prevertex.texcoord * (1 - ratio) + curvertex.texcoord * ratio;
	result.color = prevertex.color * (1 - ratio) + curvertex.color * ratio;
	return result;
}
//按照面进行裁剪
void clipagainstplane(std::vector<vertex>& clipangle_,std::vector<vertex>& clipangle,Plane plane)
{
	//临时的存储结构
	
	for (int i = 0; i < clipangle.size(); i++)
	{
		int prevertex = (i - 1 + clipangle.size()) % clipangle.size();
		int curvertex = i;
		bool preinsertplane = isinsertplane(clipangle[prevertex].v, plane);
		bool curinsertplane = isinsertplane(clipangle[curvertex].v, plane);

		if (preinsertplane != curinsertplane) {
		//进行插值运算，产生新的顶点
			float ratio = getratio(clipangle[prevertex], clipangle[curvertex],plane);

			vertex newvertex = lerpvertex(clipangle[prevertex], clipangle[curvertex], ratio);
			clipangle_.push_back(newvertex);
		}

		//如果此点在面的内部，则需要将此点添加
		if (curinsertplane)
		{
			clipangle_.push_back(clipangle[curvertex]);
		}
	}
	//当我们将数据都导入clipangle_后，需要清空clipangle，以便进行下一次循环
	clipangle.clear();
}


//裁剪
std::vector<triangle*> rasterizer::clip(triangle newtri) {
	//最终结果
	std::vector<triangle*> result;

	bool v0 = isvisable(newtri.trivertex[0].v);
	bool v1 = isvisable(newtri.trivertex[1].v);
	bool v2 = isvisable(newtri.trivertex[2].v);

	//新建一个对象，必须new一个，否则返回的时候会被销毁
	triangle* nt = new triangle();
	nt->trivertex[0] = newtri.trivertex[0];
	nt->trivertex[1] = newtri.trivertex[1];
	nt->trivertex[2] = newtri.trivertex[2];
	result.push_back(nt);
	if (v0 && v1 && v2)return result;
	
	//定义存储结构，用于存储顶点数据,采用双缓冲的方式，进行交替更新
	std::vector<vertex> cliptriangle;
	std::vector<vertex> cliptriangle_;
	
	//先将cliptriangle输入原数据,将三个顶点数据输入
	cliptriangle.push_back(newtri.trivertex[0]);
	cliptriangle.push_back(newtri.trivertex[1]);
	cliptriangle.push_back(newtri.trivertex[2]);


	//接下来就要按面裁剪，一共需要裁剪7次，第一次将w<0的情况裁剪掉
	//+w
	clipagainstplane(cliptriangle_, cliptriangle, Plane::Positive_w);
	if (cliptriangle_.size() == 0)return result;
	//+x 不要忘了替换两个容器
	clipagainstplane(cliptriangle, cliptriangle_, Plane::Positvie_x);
	if (cliptriangle_.size() == 0)return result;
	//-x
	clipagainstplane(cliptriangle_, cliptriangle, Plane::Negative_x);
	if (cliptriangle_.size() == 0)return result;
	//+y 
	clipagainstplane(cliptriangle, cliptriangle_, Plane::Positvie_y);
	if (cliptriangle_.size() == 0)return result;
	//-y
	clipagainstplane(cliptriangle_, cliptriangle, Plane::Negative_y);
	if (cliptriangle_.size() == 0)return result;
	//+z 
	clipagainstplane(cliptriangle, cliptriangle_, Plane::Positive_z);
	if (cliptriangle_.size() == 0)return result;
	//-z
	clipagainstplane(cliptriangle_, cliptriangle, Plane::Negative_z);
	if (cliptriangle_.size() == 0)return result;

	//裁剪完成后，重新组成三角形,将组成顶点数-2个三角形
	
	
	for (int i = 0; i < cliptriangle_.size() - 2; i++)
	{
		triangle* tri = new triangle(); //创建一个triangle对象
		tri->trivertex[0] = cliptriangle_[0]; //所有三角形的第一个顶点始终为cliptriangle中的第0个顶点
		for (int j = 1; j < 3; j++)
		{
			tri->trivertex[j] = cliptriangle_[j + i];
		}
		result.push_back(tri);
	}
	return result;


}

//这里有一个瑕疵，按理说这里输入的应该是顶点数据，不需要组成三角形，等到图元装配时，在组成三角形
void rasterizer::draw(std::vector<triangle*> trianglelist,float angle, select_fragmentshader select_myshader)
{
	//draw函数将执行整个渲染流程
	
	
	Eigen::Matrix4f MVP;
	//通过不同的标签更新矩阵
	my_vertexshader.set_rotation_matrix(angle);
	MVP = my_vertexshader.projection_matrix * my_vertexshader.view_matrix * my_vertexshader.rotation_matrix * my_vertexshader.model_matrix;

	
	
	int index = 1; //模拟进度条
	for (auto tri : trianglelist)  
	{
		
		/*if (count == 300)break;
		count++;*/
		triangle newtri = *tri; //先把triangle指针转换到值

		for (auto& newvertex : newtri.trivertex)
		{
			//MVP变换
			newvertex.v = MVP * newvertex.v; 
		}
	
		

		//2.裁剪,拆解三角形
		std::vector<triangle*> cliptrianglelist  = clip(newtri);

		//模拟进度条
		std::cout << (1.0f / trianglelist.size() * index) * 100 << "%" <<" ";
		index++;
		
		//如果size是0，就意味着三角形的所有顶点被裁掉，着也就意味着这个三角形不应该被渲染
		if (cliptrianglelist.size() == 0)continue;

		//外部再添加一层for循环
		
		for (auto cliptriangle : cliptrianglelist)
		{
			triangle newcliptri = *cliptriangle;
			//在渲染此三角形之前，我们需先得到view_pos,在phong shader与背部剔除中要使用

			//这里进行了透视矩阵的逆矩阵，将其转换回视图空间
			view_pos[0] = my_vertexshader.projection_matrix.inverse() * newcliptri.trivertex[0].v;
			view_pos[1] = my_vertexshader.projection_matrix.inverse() * newcliptri.trivertex[1].v;
			view_pos[2] = my_vertexshader.projection_matrix.inverse() * newcliptri.trivertex[2].v;

			world_pos[0] = my_vertexshader.view_matrix.inverse() * view_pos[0];
			world_pos[1] = my_vertexshader.view_matrix.inverse() * view_pos[1];
			world_pos[2] = my_vertexshader.view_matrix.inverse() * view_pos[2];



			//3.透视除法
			for (auto& newvertex : newcliptri.trivertex)
			{
				newvertex.v.x() /= newvertex.v.w();
				newvertex.v.y() /= newvertex.v.w();
				newvertex.v.z() /= newvertex.v.w();
			}
			//4.转换为屏幕空间
			for (auto& newvertex : newcliptri.trivertex)
			{
				newvertex.v.x() = (newvertex.v.x() + 1.0f) / 2.0f * WIDTH;
				newvertex.v.y() = (newvertex.v.y() + 1.0f) / 2.0f * HEIGHT;
				newvertex.v.z() = (newvertex.v.z() + 1.0f) / 2.0f;
			}
			//5.图元装配，将离散的顶点转换为三角形，这里已经实现，因为就是使用三角形进行存储的


			//6.光栅化
			raster(newcliptri, select_myshader);
			
		}
		


		
	}
	//7.进行超采样
	sampling();
}
//渲染shadow mapping的drawF函数
void rasterizer::drawF(std::vector<triangle*> trianglelist, float angle)
{
	
	//draw函数将执行整个渲染流程
	my_vertexshader.set_rotation_matrix(angle);

	Eigen::Matrix4f MVP;

	my_vertexshader.set_view_matrix(eye, target, up);
	my_vertexshader.set_ortho_matrix(size, aspect, zNear, zFar);
	my_vertexshader.set_projection_matrix(fov,aspect,zNear,zFar);
	my_vertexshader.set_view_matrix_light(directionlight.position, directionlight.lightdir, Eigen::Vector3f(0, 1.0f, 0));
	MVP = my_vertexshader.ortho_matrix * my_vertexshader.view_matrix_light * my_vertexshader.rotation_matrix * my_vertexshader.model_matrix;



	int index = 1; //模拟进度条
	int i = 0;
	for (auto tri : trianglelist)
	{
		/*if (count == 300)break;
		count++;*/
		triangle newtri = *tri; //先把triangle指针转换到值

		for (auto& newvertex : newtri.trivertex)
		{
			//MVP变换
			newvertex.v = MVP * newvertex.v;
		}



		//2.裁剪,拆解三角形
		std::vector<triangle*> cliptrianglelist = clip(newtri);

		//模拟进度条
		std::cout << std::endl;
		std::cout << (1.0f / trianglelist.size() * index) * 100 << "%" << " ";

		index++;

		//如果size是0，就意味着三角形的所有顶点被裁掉，着也就意味着这个三角形不应该被渲染
		if (cliptrianglelist.size() == 0)continue;

		//外部再添加一层for循环

		for (auto cliptriangle : cliptrianglelist)
		{
			triangle newcliptri = *cliptriangle;
			//在渲染此三角形之前，我们需先得到view_pos,在phong shader与背部剔除中要使用

		
			//4.转换为屏幕空间
			for (auto& newvertex : newcliptri.trivertex)
			{
				newvertex.v.x() = (newvertex.v.x() + 1.0f) / 2.0f * WIDTH;
				newvertex.v.y() = (newvertex.v.y() + 1.0f) / 2.0f * HEIGHT;
				newvertex.v.z() = (newvertex.v.z() + 1.0f) / 2.0f;
			}
			//5.图元装配，将离散的顶点转换为三角形，这里已经实现，因为就是使用三角形进行存储的


			//6.光栅化
			rasterF(newcliptri);

		}

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

	//初始化shadow mapping
	my_framebuffer.ssaa_shadowbuffer.resize(WIDTH * HEIGHT * ssaa_factor * ssaa_factor,std::numeric_limits<float>::max());
}
//通过叉乘的方法判断点是否在三角形的内部
bool isinserttriangle(float pixel_x, float pixel_y, triangle& newtri) //这里一定是float类型，如果是int，则SSAA失效
{
	Eigen::Vector2f p(pixel_x, pixel_y);
	Eigen::Vector2f a = Eigen::Vector2f(newtri.trivertex[0].v.x(), newtri.trivertex[0].v.y());
	Eigen::Vector2f b = Eigen::Vector2f(newtri.trivertex[1].v.x(), newtri.trivertex[1].v.y());
	Eigen::Vector2f c = Eigen::Vector2f(newtri.trivertex[2].v.x(), newtri.trivertex[2].v.y());
	float cross1 = (a - b).x() * (p - a).y() - (a - b).y() * (p - a).x();
	float cross2 = (b-c).x() * (p - b).y() - (b - c).y() * (p -b).x();
	float cross3 = (c-a).x() * (p - c).y() - (c - a).y() * (p - c).x();
	return ((cross1>=0&& cross2>= 0&&cross3>0)||
		(cross1 <= 0 && cross2 <= 0 && cross3 <=0));
}
//重心坐标求三个比例.
std::tuple<float, float, float> barycentic(float pixel_x, float pixel_y, triangle& newtri)
{
	Eigen::Vector2f ap = Eigen::Vector2f(pixel_x - newtri.trivertex[0].v.x(), pixel_y - newtri.trivertex[0].v.y());
	Eigen::Vector2f ac = Eigen::Vector2f(newtri.trivertex[2].v.x() - newtri.trivertex[0].v.x(), newtri.trivertex[2].v.y() - newtri.trivertex[0].v.y());
	Eigen::Vector2f ab = Eigen::Vector2f(newtri.trivertex[1].v.x() - newtri.trivertex[0].v.x(), newtri.trivertex[1].v.y() - newtri.trivertex[0].v.y());
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
void rasterizer::raster(triangle& newtri,select_fragmentshader select_myshader) {
	//先确定AABB包围盒
	float xmin = std::max(0.0f, std::min(newtri.trivertex[0].v.x(), std::min(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x())))*ssaa_factor;
	float xmax = std::min(float(WIDTH-1), std::max(newtri.trivertex[0].v.x(), std::max(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x())))*ssaa_factor;
	float ymin = std::max(0.0f, std::min(newtri.trivertex[0].v.y(), std::min(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y())))*ssaa_factor;
	float ymax = std::min(float(HEIGHT - 1), std::max(newtri.trivertex[0].v.y(), std::max(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y())))*ssaa_factor;
	

	//遍历所有的像素
	int count = 0;
	for (int y = floor(ymin); y < ceil(ymax); y++)
	{
		for (int x = floor(xmin); x < ceil(xmax); x++)
		{
			float pixel_x = (x + 0.5)/ssaa_factor;
			float pixel_y =(y + 0.5)/ssaa_factor;

			//进行背部剔除
			if (isbacking(view_pos))
			{
				continue;
			}
			//判断点是否在三角形的内部
			if (isinserttriangle(pixel_x, pixel_y, newtri)) {
				

				//重心坐标求比例
				auto [alpha,beta,gamma] = barycentic(pixel_x,pixel_y,newtri);
				//获得真实的插值比例
				float wp = 1.0f/(alpha / newtri.trivertex[0].v.w() + beta / newtri.trivertex[1].v.w() + gamma / newtri.trivertex[2].v.w());
				float wa = alpha / newtri.trivertex[0].v.w() * wp;
				float wb = beta / newtri.trivertex[1].v.w() * wp;
				float wc = gamma / newtri.trivertex[2].v.w() * wp;
				//先获得插值的Z值，用于判断是否能进行着色
				float zdepth = newtri.trivertex[0].v.z() * wa + newtri.trivertex[1].v.z() * wb + newtri.trivertex[2].v.z() * wc;

				
				
				

				//这里是将我们插值的世界空间坐标转换到光源空间
				Eigen::Vector4f lerp_world_pos = world_pos[0] * wa + world_pos[1] * wb + world_pos[2] * wc;
				//test
				//std::cout << lerp_world_pos << std::endl;
				//end
				Eigen::Vector4f lerp_clip_light_pos = my_vertexshader.ortho_matrix * my_vertexshader.view_matrix_light * lerp_world_pos;

				//注意：x，y都要转到光源空间下 这里是拿到光源空间的索引，x，y
				int screen_light_x = static_cast<int>((lerp_clip_light_pos.x() / lerp_clip_light_pos.w() + 1.0f) / 2 * WIDTH * ssaa_factor);
				int screen_light_y = static_cast<int>((lerp_clip_light_pos.y() / lerp_clip_light_pos.w() + 1.0f) / 2 * HEIGHT * ssaa_factor);
				screen_light_x = std::clamp(screen_light_x, 0, WIDTH * ssaa_factor - 1);
				screen_light_y = std::clamp(screen_light_y, 0, HEIGHT * ssaa_factor - 1);

				float zdepth_sm = (lerp_clip_light_pos.z() / lerp_clip_light_pos.w() + 1.0f) / 2; //得到屏幕空间的Z值
				float bias = 0.005f; //深度位置偏差
				if (zdepth_sm > my_framebuffer.ssaa_shadowbuffer[get_index_ssaa(screen_light_x, screen_light_y)]+bias)
				{
					//test
					count++;

					continue;
					//end
				}

				//仅当满足此条件时，可进行渲染
				if (zdepth < my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)])
				{


					//深度更新
					my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)] = zdepth;



					Eigen::Vector3f fragment_color;
					//使用switch语句，可以决定我们调用哪个着色器
					switch (select_myshader)
					{
					case test_shader:
						fragment_color = my_fragmentshader.testshader();
						break;

					case phong_shader:
						


						//-1 插值坐标
						Eigen::Vector4f lerp_view_pos = view_pos[0] * wa + view_pos[1] * wb + view_pos[2] * wc;

						//-2 插值法线
						Eigen::Vector4f lerp_normal = newtri.trivertex[0].normal * wa + newtri.trivertex[1].normal * wb + newtri.trivertex[2].normal * wc;

						// -3 插值纹理坐标

						Eigen::Vector2f lerp_texcoord = newtri.trivertex[0].texcoord * wa + newtri.trivertex[1].texcoord * wb + newtri.trivertex[2].texcoord * wc;


						fragment_payload payload;
						payload.view_pos = lerp_view_pos;
						payload.normal = lerp_normal;
						payload.texcoord = lerp_texcoord;

						if (ismimmap)//如果开启了minmap，则使用minmap层级
						{
							
							int minmap_index = get_minmap_index(newtri, wa, wb, wc);//此函数用于获取mimmap层级
							
							payload.color = tex->get_color_mipmap(lerp_texcoord.x(), lerp_texcoord.y(), minmap_index); 
																									

							
						}																		

						
						//如果不开启minmap，则使用双线性插值
						if(!ismimmap)payload.color = tex->getcolor_lerpbinary(lerp_texcoord.x(), lerp_texcoord.y());
						fragment_color = my_fragmentshader.fragment_phong_shader(payload);
						break;
					}
						//最终的颜色确定
						my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x, y)] = fragment_color;
					}
				
			}

		}
	}
	std::cout << count << std::endl;

}

//渲染shadow mapping的光栅化函数
void rasterizer::rasterF(triangle& newtri)
{
	//先确定AABB包围盒
	float xmin = std::max(0.0f, std::min(newtri.trivertex[0].v.x(), std::min(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x()))) * ssaa_factor;
	float xmax = std::min(float(WIDTH - 1), std::max(newtri.trivertex[0].v.x(), std::max(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x()))) * ssaa_factor;
	float ymin = std::max(0.0f, std::min(newtri.trivertex[0].v.y(), std::min(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y()))) * ssaa_factor;
	float ymax = std::min(float(HEIGHT - 1), std::max(newtri.trivertex[0].v.y(), std::max(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y()))) * ssaa_factor;

	
	//遍历所有的像素
	for (int y = floor(ymin); y < ceil(ymax); y++)
	{
		for (int x = floor(xmin); x < ceil(xmax); x++)
		{
			
			float pixel_x = (x + 0.5) / ssaa_factor;
			float pixel_y = (y + 0.5) / ssaa_factor;



			//判断点是否在三角形的内部
			if (isinserttriangle(pixel_x, pixel_y, newtri)) {


				//重心坐标求比例
				auto [alpha, beta, gamma] = barycentic(pixel_x, pixel_y, newtri);
				//获得真实的插值比例
				//正交投影将取消修正，修正是透视投影所做的
				float wa = alpha;
				float wb = beta;
				float wc = gamma;
				//先获得插值的Z值，用于判断是否能进行着色
				//std::cout << newtri.trivertex[0].v.z() <<" " << newtri.trivertex[1].v.z()<<" " << newtri.trivertex[2].v.z() << std::endl;
				float zdepth = newtri.trivertex[0].v.z() * wa + newtri.trivertex[1].v.z() * wb + newtri.trivertex[2].v.z() * wc; //得到屏幕空间的Z值
				
				//仅当满足此条件时，可进行渲染
				if (zdepth < my_framebuffer.ssaa_shadowbuffer[get_index_ssaa(x, y)])
				{
					//深度更新
					my_framebuffer.ssaa_shadowbuffer[get_index_ssaa(x, y)] = zdepth;
					//my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x, y)] = Eigen::Vector3f(zdepth*255*7, zdepth*255*5*7, zdepth*255*5*7);
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

	//shadow mapping
	std::fill(my_framebuffer.ssaa_shadowbuffer.begin(), my_framebuffer.ssaa_shadowbuffer.end(), std::numeric_limits<float>::max());
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
int rasterizer::get_minmap_index(triangle newtri,float wa,float wb,float wc)
{
	// 1. 获取三角形三个顶点的屏幕坐标和纹理坐标
	Eigen::Vector2f p0(newtri.trivertex[0].v.x(), newtri.trivertex[0].v.y()); // 屏幕坐标(x,y)
	Eigen::Vector2f p1(newtri.trivertex[1].v.x(), newtri.trivertex[1].v.y());
	Eigen::Vector2f p2(newtri.trivertex[2].v.x(), newtri.trivertex[2].v.y());
	Eigen::Vector2f t0(newtri.trivertex[0].texcoord.x(), newtri.trivertex[0].texcoord.y()); // 纹理坐标(u,v)
	Eigen::Vector2f t1(newtri.trivertex[1].texcoord.x(), newtri.trivertex[1].texcoord.y());
	Eigen::Vector2f t2(newtri.trivertex[2].texcoord.x(), newtri.trivertex[2].texcoord.y());

	// 2. 解线性方程组，计算纹理坐标对屏幕坐标的偏导 (du/dx, du/dy, dv/dx, dv/dy)
	// 基于公式：u = a*x + b*y + c，代入三个顶点消去c，解a和b
	float dx0 = p0.x() - p2.x(); // x0 - x2
	float dy0 = p0.y() - p2.y(); // y0 - y2
	float dx1 = p1.x() - p2.x(); // x1 - x2
	float dy1 = p1.y() - p2.y(); // y1 - y2
	float du0 = t0.x() - t2.x(); // u0 - u2
	float du1 = t1.x() - t2.x(); // u1 - u2
	float dv0 = t0.y() - t2.y(); // v0 - v2
	float dv1 = t1.y() - t2.y(); // v1 - v2

	// 系数矩阵行列式（避免除零）
	float det = dx0 * dy1 - dy0 * dx1;
	if (std::abs(det) < 1e-6) {
		return 0; // 三角形退化，返回最低层级
	}

	// 计算u的偏导：a=du/dx, b=du/dy
	float a = (du0 * dy1 - dy0 * du1) / det;
	float b = (dx0 * du1 - du0 * dx1) / det;
	// 计算v的偏导：d=dv/dx, e=dv/dy
	float d = (dv0 * dy1 - dy0 * dv1) / det;
	float e = (dx0 * dv1 - dv0 * dx1) / det;

	// 3. 计算屏幕1个像素对应的纹理纹素数量（rho_x, rho_y）
	// a是归一化纹理u对屏幕x的偏导，乘以纹理宽度得到纹素数量
	float rho_x = std::sqrt(std::pow(a * tex->width, 2) + std::pow(d * tex->width, 2));
	float rho_y = std::sqrt(std::pow(b * tex->height, 2) + std::pow(e * tex->height, 2));

	// 4. 计算max_dis和lod层级
	float max_dis = std::max(rho_x, rho_y);
	float lod = std::log2(std::max(max_dis, 1.0f)); // 确保lod >=0（避免log2(0)）
	int minmap_level = std::clamp(static_cast<int>(lod), 0, static_cast<int>(tex->mipmap_texture_data.size() - 1));

	return minmap_level;
}
