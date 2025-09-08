#include <iostream>
#include "Rasterizer.h"



//�����޳�
bool rasterizer::isbacking(Eigen::Vector4f newtri[3])
{
	Eigen::Vector4f edge1 = newtri[1] - newtri[0];
	Eigen::Vector3f edge1_ = Eigen::Vector3f(edge1.x(), edge1.y(), edge1.z());
	Eigen::Vector4f edge2 = newtri[2] - newtri[0];
	Eigen::Vector3f edge2_ = Eigen::Vector3f(edge2.x(), edge2.y(), edge2.z());
	//���߷�������Ϊ������ָ�������������ߵĹ۲췽����һ����
	Eigen::Vector3f viewdir = Eigen::Vector3f(-newtri[0].x(), -newtri[0].y(), -newtri[0].z()); //����ָ�������ε�һ���㼴��
	return edge1_.cross(edge2_).dot(viewdir) <= 0;
}


void rasterizer::draw(std::vector<triangle*> trianglelist,float angle)
{
	//draw������ִ��������Ⱦ����
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
		triangle newtri = *tri; //�Ȱ�triangleָ��ת����ֵ


		//1.����MVP�任
		int index = 0;
		for (auto& newvertex : newtri.v)
		{
			//������ͼ�ռ��µ�������
			view_pos[index] = (my_vertexshader.view_matrix * rotation_matrix * my_vertexshader.model_matrix * newvertex);
			index++;
			//MVP�任
			newvertex = MVP * newvertex; 
			

		}
		
		

		//2.�ü������������������ٸĽ���ʱ�򣬲�Ҫֱ��ʹ�������δ洢����ʹ�ö��㣬��ֱ��ʹ��������
		// ���޷����вü���
		
		//3.͸�ӳ���
		for (auto& newvertex : newtri.v)
		{
			newvertex.x() /= newvertex.w();
			newvertex.y() /= newvertex.w();
			newvertex.z() /= newvertex.w();
		}
		//4.ת��Ϊ��Ļ�ռ�
		for (auto& newvertex : newtri.v)
		{
			newvertex.x() = (newvertex.x() + 1.0f) /2.0f* WIDTH;
			newvertex.y() = (newvertex.y() + 1.0f) / 2.0f*HEIGHT;
			newvertex.z() = (newvertex.z() + 1.0f) / 2.0f;
		}
		//5.ͼԪװ�䣬����ɢ�Ķ���ת��Ϊ�����Σ������Ѿ�ʵ�֣���Ϊ����ʹ�������ν��д洢��

		//6.��դ��
		raster(newtri);


		
	}
	//7.���г�����
	sampling();
}
rasterizer::rasterizer()
{
	my_framebuffer.colorbuffer.resize(WIDTH * HEIGHT, Eigen::Vector3f(0, 0, 0));
	my_framebuffer.depthbuffer.resize(WIDTH * HEIGHT, std::numeric_limits<float>::max());

	//��������ʼ��
	my_framebuffer.ssaa_colorbuffer.resize(WIDTH * HEIGHT*ssaa_factor*ssaa_factor, Eigen::Vector3f(0, 0, 0));
	my_framebuffer.ssaa_depthbuffer.resize(WIDTH * HEIGHT*ssaa_factor*ssaa_factor, std::numeric_limits<float>::max());
}
//ͨ����˵ķ����жϵ��Ƿ��������ε��ڲ�
bool isinserttriangle(float pixel_x, float pixel_y, triangle& newtri) //����һ����float���ͣ������int����SSAAʧЧ
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
//������������������.
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

//����colorbuffer��depthbuffer������
 int rasterizer::get_index(int x, int y)
{
	return (HEIGHT - 1 -y)*WIDTH + x;
}

 int rasterizer::get_index_ssaa(int x, int y) {
	 return (HEIGHT * ssaa_factor - 1 - y) * WIDTH * ssaa_factor + x;
 }

//��դ������ (ʹ����SSAA)
void rasterizer::raster(triangle& newtri) {
	//��ȷ��AABB��Χ��
	float xmin = std::max(0.0f, std::min(newtri.v[0].x(), std::min(newtri.v[1].x(), newtri.v[2].x())))*ssaa_factor;
	float xmax = std::min(float(WIDTH-1), std::max(newtri.v[0].x(), std::max(newtri.v[1].x(), newtri.v[2].x())))*ssaa_factor;
	float ymin = std::max(0.0f, std::min(newtri.v[0].y(), std::min(newtri.v[1].y(), newtri.v[2].y())))*ssaa_factor;
	float ymax = std::min(float(HEIGHT - 1), std::max(newtri.v[0].y(), std::max(newtri.v[1].y(), newtri.v[2].y())))*ssaa_factor;

	//�������е�����
	for (int y = floor(ymin); y < ceil(ymax); y++)
	{
		for (int x = floor(xmin); x < ceil(xmax); x++)
		{
			float pixel_x = (x + 0.5)/ssaa_factor;
			float pixel_y =(y + 0.5)/ssaa_factor;

			//�жϵ��Ƿ��������ε��ڲ�
			if (isinserttriangle(pixel_x, pixel_y, newtri)) {
				

				//�������������
				auto [alpha,gamma,beta] = barycentic(pixel_x,pixel_y,newtri);
				//�����ʵ�Ĳ�ֵ����
				float wp = 1.0f/(alpha / newtri.v[0].w() + beta / newtri.v[1].w() + gamma / newtri.v[2].w());
				float wa = alpha / newtri.v[0].w() * wp;
				float wb = beta / newtri.v[1].w() * wp;
				float wc = gamma / newtri.v[2].w() * wp;
				//�Ȼ�ò�ֵ��Zֵ�������ж��Ƿ��ܽ�����ɫ
				float zdepth = newtri.v[0].z() * wa + newtri.v[1].z() * wb + newtri.v[2].z() * wc;
				
				//���б����޳�
				if (isbacking(view_pos))
				{
					continue;
				}
				//�������������ʱ���ɽ�����Ⱦ
				if (zdepth < my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)])
				{
					//��ȸ���
					my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)] = zdepth;

					//�˴����в�ֵ���ԣ�Ȼ�����ƬԪ��ɫ
					
					//-1 ��ֵ����
					Eigen::Vector4f lerp_view_pos = view_pos[0] * wa + view_pos[1] * wb + view_pos[2] * wc;

					//-2 ��ֵ����
					Eigen::Vector4f lerp_normal = newtri.normal[0] * wa + newtri.normal[1] * wb + newtri.normal[2] * wc;

					// -3 ��ֵ��������
					
					Eigen::Vector2f lerp_texcoord = newtri.texcoord[0] * wa + newtri.texcoord[1] * wb + newtri.texcoord[2] * wc;

					fragment_payload payload;
					payload.view_pos = lerp_view_pos;
					payload.normal = lerp_normal;
					payload.texcoord = lerp_texcoord;
					payload.color = tex->getcolor(lerp_texcoord.x(), lerp_texcoord.y());
					//���ò�ͬ��ƬԪ��ɫ��
					Eigen::Vector3f fragment_color = my_fragmentshader.fragment_phong_shader(payload);
					
					//���յ���ɫȷ��
					my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x, y)] =fragment_color;
				}
			}

		}
	}
}

void rasterizer::clear() {
	//ˢ��֡����
	std::fill(my_framebuffer.colorbuffer.begin(), my_framebuffer.colorbuffer.end(), Eigen::Vector3f(0, 0, 0));
	std::fill(my_framebuffer.depthbuffer.begin(), my_framebuffer.depthbuffer.end(), std::numeric_limits<float>::max());
	
	//������
	std::fill(my_framebuffer.ssaa_colorbuffer.begin(), my_framebuffer.ssaa_colorbuffer.end(), Eigen::Vector3f(0, 0, 0));
	std::fill(my_framebuffer.ssaa_depthbuffer.begin(), my_framebuffer.ssaa_depthbuffer.end(), std::numeric_limits<float>::max());
}

//��������
void rasterizer::sampling() {
	//���ssaa_factorΪ1����ô����ֱ�ӽ�ssaa_colorbuffer���Ƶ�colorbuffer�У�������Ҫ��ƽ��
	if (ssaa_factor == 1) {
		std::copy(my_framebuffer.ssaa_colorbuffer.begin(), my_framebuffer.ssaa_colorbuffer.end(), my_framebuffer.colorbuffer.begin());
		return;
	}
	//���ssaa_factor��Ϊ1������Ҫ����ɫ����ƽ��
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			//����Ҫ�ȵõ�ssaa_colorbuffer�µ���ɫֵ
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
			//ƽ��avg_color
			avg_color = Eigen::Vector3f(avg_color.x() / ssaa_factor / ssaa_factor, avg_color.y() / ssaa_factor / ssaa_factor, avg_color.z() / ssaa_factor / ssaa_factor);
			my_framebuffer.colorbuffer[get_index(x, y)] = avg_color;
		}
	}


}
