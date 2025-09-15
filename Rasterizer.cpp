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

//���Ƿ�ɼ�
bool isvisable(Eigen::Vector4f v)
{
	return (std::abs(v.x()) <= v.w() && std::abs(v.y()) <= v.w() && std::abs(v.z()) <= v.w());
}

//�����Ƿ���ƽ����
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
//�󽻵�
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
	//��ֵ����
	vertex result;
	result.v = prevertex.v * (1 - ratio) + curvertex.v * ratio;
	result.normal = prevertex.normal * (1 - ratio) + curvertex.normal * ratio;
	result.texcoord = prevertex.texcoord * (1 - ratio) + curvertex.texcoord * ratio;
	result.color = prevertex.color * (1 - ratio) + curvertex.color * ratio;
	return result;
}
//��������вü�
void clipagainstplane(std::vector<vertex>& clipangle_,std::vector<vertex>& clipangle,Plane plane)
{
	//��ʱ�Ĵ洢�ṹ
	
	for (int i = 0; i < clipangle.size(); i++)
	{
		int prevertex = (i - 1 + clipangle.size()) % clipangle.size();
		int curvertex = i;
		bool preinsertplane = isinsertplane(clipangle[prevertex].v, plane);
		bool curinsertplane = isinsertplane(clipangle[curvertex].v, plane);

		if (preinsertplane != curinsertplane) {
		//���в�ֵ���㣬�����µĶ���
			float ratio = getratio(clipangle[prevertex], clipangle[curvertex],plane);

			vertex newvertex = lerpvertex(clipangle[prevertex], clipangle[curvertex], ratio);
			clipangle_.push_back(newvertex);
		}

		//����˵�������ڲ�������Ҫ���˵����
		if (curinsertplane)
		{
			clipangle_.push_back(clipangle[curvertex]);
		}
	}
	//�����ǽ����ݶ�����clipangle_����Ҫ���clipangle���Ա������һ��ѭ��
	clipangle.clear();
}


//�ü�
std::vector<triangle*> rasterizer::clip(triangle newtri) {
	//���ս��
	std::vector<triangle*> result;

	bool v0 = isvisable(newtri.trivertex[0].v);
	bool v1 = isvisable(newtri.trivertex[1].v);
	bool v2 = isvisable(newtri.trivertex[2].v);

	//�½�һ�����󣬱���newһ�������򷵻ص�ʱ��ᱻ����
	triangle* nt = new triangle();
	nt->trivertex[0] = newtri.trivertex[0];
	nt->trivertex[1] = newtri.trivertex[1];
	nt->trivertex[2] = newtri.trivertex[2];
	result.push_back(nt);
	if (v0 && v1 && v2)return result;
	
	//����洢�ṹ�����ڴ洢��������,����˫����ķ�ʽ�����н������
	std::vector<vertex> cliptriangle;
	std::vector<vertex> cliptriangle_;
	
	//�Ƚ�cliptriangle����ԭ����,������������������
	cliptriangle.push_back(newtri.trivertex[0]);
	cliptriangle.push_back(newtri.trivertex[1]);
	cliptriangle.push_back(newtri.trivertex[2]);


	//��������Ҫ����ü���һ����Ҫ�ü�7�Σ���һ�ν�w<0������ü���
	//+w
	clipagainstplane(cliptriangle_, cliptriangle, Plane::Positive_w);
	if (cliptriangle_.size() == 0)return result;
	//+x ��Ҫ�����滻��������
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

	//�ü���ɺ��������������,����ɶ�����-2��������
	
	
	for (int i = 0; i < cliptriangle_.size() - 2; i++)
	{
		triangle* tri = new triangle(); //����һ��triangle����
		tri->trivertex[0] = cliptriangle_[0]; //���������εĵ�һ������ʼ��Ϊcliptriangle�еĵ�0������
		for (int j = 1; j < 3; j++)
		{
			tri->trivertex[j] = cliptriangle_[j + i];
		}
		result.push_back(tri);
	}
	return result;


}

//������һ��覴ã�����˵���������Ӧ���Ƕ������ݣ�����Ҫ��������Σ��ȵ�ͼԪװ��ʱ�������������
void rasterizer::draw(std::vector<triangle*> trianglelist,float angle, select_fragmentshader select_myshader)
{
	//draw������ִ��������Ⱦ����
	
	
	Eigen::Matrix4f MVP;
	//ͨ����ͬ�ı�ǩ���¾���
	my_vertexshader.set_rotation_matrix(angle);
	MVP = my_vertexshader.projection_matrix * my_vertexshader.view_matrix * my_vertexshader.rotation_matrix * my_vertexshader.model_matrix;

	
	
	int index = 1; //ģ�������
	for (auto tri : trianglelist)  
	{
		
		/*if (count == 300)break;
		count++;*/
		triangle newtri = *tri; //�Ȱ�triangleָ��ת����ֵ

		for (auto& newvertex : newtri.trivertex)
		{
			//MVP�任
			newvertex.v = MVP * newvertex.v; 
		}
	
		

		//2.�ü�,���������
		std::vector<triangle*> cliptrianglelist  = clip(newtri);

		//ģ�������
		std::cout << (1.0f / trianglelist.size() * index) * 100 << "%" <<" ";
		index++;
		
		//���size��0������ζ�������ε����ж��㱻�õ�����Ҳ����ζ����������β�Ӧ�ñ���Ⱦ
		if (cliptrianglelist.size() == 0)continue;

		//�ⲿ�����һ��forѭ��
		
		for (auto cliptriangle : cliptrianglelist)
		{
			triangle newcliptri = *cliptriangle;
			//����Ⱦ��������֮ǰ���������ȵõ�view_pos,��phong shader�뱳���޳���Ҫʹ��

			//���������͸�Ӿ��������󣬽���ת������ͼ�ռ�
			view_pos[0] = my_vertexshader.projection_matrix.inverse() * newcliptri.trivertex[0].v;
			view_pos[1] = my_vertexshader.projection_matrix.inverse() * newcliptri.trivertex[1].v;
			view_pos[2] = my_vertexshader.projection_matrix.inverse() * newcliptri.trivertex[2].v;

			world_pos[0] = my_vertexshader.view_matrix.inverse() * view_pos[0];
			world_pos[1] = my_vertexshader.view_matrix.inverse() * view_pos[1];
			world_pos[2] = my_vertexshader.view_matrix.inverse() * view_pos[2];



			//3.͸�ӳ���
			for (auto& newvertex : newcliptri.trivertex)
			{
				newvertex.v.x() /= newvertex.v.w();
				newvertex.v.y() /= newvertex.v.w();
				newvertex.v.z() /= newvertex.v.w();
			}
			//4.ת��Ϊ��Ļ�ռ�
			for (auto& newvertex : newcliptri.trivertex)
			{
				newvertex.v.x() = (newvertex.v.x() + 1.0f) / 2.0f * WIDTH;
				newvertex.v.y() = (newvertex.v.y() + 1.0f) / 2.0f * HEIGHT;
				newvertex.v.z() = (newvertex.v.z() + 1.0f) / 2.0f;
			}
			//5.ͼԪװ�䣬����ɢ�Ķ���ת��Ϊ�����Σ������Ѿ�ʵ�֣���Ϊ����ʹ�������ν��д洢��


			//6.��դ��
			raster(newcliptri, select_myshader);
			
		}
		


		
	}
	//7.���г�����
	sampling();
}
//��Ⱦshadow mapping��drawF����
void rasterizer::drawF(std::vector<triangle*> trianglelist, float angle)
{
	
	//draw������ִ��������Ⱦ����
	my_vertexshader.set_rotation_matrix(angle);

	Eigen::Matrix4f MVP;

	my_vertexshader.set_view_matrix(eye, target, up);
	my_vertexshader.set_ortho_matrix(size, aspect, zNear, zFar);
	my_vertexshader.set_projection_matrix(fov,aspect,zNear,zFar);
	my_vertexshader.set_view_matrix_light(directionlight.position, directionlight.lightdir, Eigen::Vector3f(0, 1.0f, 0));
	MVP = my_vertexshader.ortho_matrix * my_vertexshader.view_matrix_light * my_vertexshader.rotation_matrix * my_vertexshader.model_matrix;



	int index = 1; //ģ�������
	int i = 0;
	for (auto tri : trianglelist)
	{
		/*if (count == 300)break;
		count++;*/
		triangle newtri = *tri; //�Ȱ�triangleָ��ת����ֵ

		for (auto& newvertex : newtri.trivertex)
		{
			//MVP�任
			newvertex.v = MVP * newvertex.v;
		}



		//2.�ü�,���������
		std::vector<triangle*> cliptrianglelist = clip(newtri);

		//ģ�������
		std::cout << std::endl;
		std::cout << (1.0f / trianglelist.size() * index) * 100 << "%" << " ";

		index++;

		//���size��0������ζ�������ε����ж��㱻�õ�����Ҳ����ζ����������β�Ӧ�ñ���Ⱦ
		if (cliptrianglelist.size() == 0)continue;

		//�ⲿ�����һ��forѭ��

		for (auto cliptriangle : cliptrianglelist)
		{
			triangle newcliptri = *cliptriangle;
			//����Ⱦ��������֮ǰ���������ȵõ�view_pos,��phong shader�뱳���޳���Ҫʹ��

		
			//4.ת��Ϊ��Ļ�ռ�
			for (auto& newvertex : newcliptri.trivertex)
			{
				newvertex.v.x() = (newvertex.v.x() + 1.0f) / 2.0f * WIDTH;
				newvertex.v.y() = (newvertex.v.y() + 1.0f) / 2.0f * HEIGHT;
				newvertex.v.z() = (newvertex.v.z() + 1.0f) / 2.0f;
			}
			//5.ͼԪװ�䣬����ɢ�Ķ���ת��Ϊ�����Σ������Ѿ�ʵ�֣���Ϊ����ʹ�������ν��д洢��


			//6.��դ��
			rasterF(newcliptri);

		}

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

	//��ʼ��shadow mapping
	my_framebuffer.ssaa_shadowbuffer.resize(WIDTH * HEIGHT * ssaa_factor * ssaa_factor,std::numeric_limits<float>::max());
}
//ͨ����˵ķ����жϵ��Ƿ��������ε��ڲ�
bool isinserttriangle(float pixel_x, float pixel_y, triangle& newtri) //����һ����float���ͣ������int����SSAAʧЧ
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
//������������������.
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

//����colorbuffer��depthbuffer������
 int rasterizer::get_index(int x, int y)
{
	return (HEIGHT - 1 -y)*WIDTH + x;
}

 int rasterizer::get_index_ssaa(int x, int y) {
	 return (HEIGHT * ssaa_factor - 1 - y) * WIDTH * ssaa_factor + x;
 }

//��դ������ (ʹ����SSAA)
void rasterizer::raster(triangle& newtri,select_fragmentshader select_myshader) {
	//��ȷ��AABB��Χ��
	float xmin = std::max(0.0f, std::min(newtri.trivertex[0].v.x(), std::min(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x())))*ssaa_factor;
	float xmax = std::min(float(WIDTH-1), std::max(newtri.trivertex[0].v.x(), std::max(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x())))*ssaa_factor;
	float ymin = std::max(0.0f, std::min(newtri.trivertex[0].v.y(), std::min(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y())))*ssaa_factor;
	float ymax = std::min(float(HEIGHT - 1), std::max(newtri.trivertex[0].v.y(), std::max(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y())))*ssaa_factor;
	

	//�������е�����
	int count = 0;
	for (int y = floor(ymin); y < ceil(ymax); y++)
	{
		for (int x = floor(xmin); x < ceil(xmax); x++)
		{
			float pixel_x = (x + 0.5)/ssaa_factor;
			float pixel_y =(y + 0.5)/ssaa_factor;

			//���б����޳�
			if (isbacking(view_pos))
			{
				continue;
			}
			//�жϵ��Ƿ��������ε��ڲ�
			if (isinserttriangle(pixel_x, pixel_y, newtri)) {
				

				//�������������
				auto [alpha,beta,gamma] = barycentic(pixel_x,pixel_y,newtri);
				//�����ʵ�Ĳ�ֵ����
				float wp = 1.0f/(alpha / newtri.trivertex[0].v.w() + beta / newtri.trivertex[1].v.w() + gamma / newtri.trivertex[2].v.w());
				float wa = alpha / newtri.trivertex[0].v.w() * wp;
				float wb = beta / newtri.trivertex[1].v.w() * wp;
				float wc = gamma / newtri.trivertex[2].v.w() * wp;
				//�Ȼ�ò�ֵ��Zֵ�������ж��Ƿ��ܽ�����ɫ
				float zdepth = newtri.trivertex[0].v.z() * wa + newtri.trivertex[1].v.z() * wb + newtri.trivertex[2].v.z() * wc;

				
				
				

				//�����ǽ����ǲ�ֵ������ռ�����ת������Դ�ռ�
				Eigen::Vector4f lerp_world_pos = world_pos[0] * wa + world_pos[1] * wb + world_pos[2] * wc;
				//test
				//std::cout << lerp_world_pos << std::endl;
				//end
				Eigen::Vector4f lerp_clip_light_pos = my_vertexshader.ortho_matrix * my_vertexshader.view_matrix_light * lerp_world_pos;

				//ע�⣺x��y��Ҫת����Դ�ռ��� �������õ���Դ�ռ��������x��y
				int screen_light_x = static_cast<int>((lerp_clip_light_pos.x() / lerp_clip_light_pos.w() + 1.0f) / 2 * WIDTH * ssaa_factor);
				int screen_light_y = static_cast<int>((lerp_clip_light_pos.y() / lerp_clip_light_pos.w() + 1.0f) / 2 * HEIGHT * ssaa_factor);
				screen_light_x = std::clamp(screen_light_x, 0, WIDTH * ssaa_factor - 1);
				screen_light_y = std::clamp(screen_light_y, 0, HEIGHT * ssaa_factor - 1);

				float zdepth_sm = (lerp_clip_light_pos.z() / lerp_clip_light_pos.w() + 1.0f) / 2; //�õ���Ļ�ռ��Zֵ
				float bias = 0.005f; //���λ��ƫ��
				if (zdepth_sm > my_framebuffer.ssaa_shadowbuffer[get_index_ssaa(screen_light_x, screen_light_y)]+bias)
				{
					//test
					count++;

					continue;
					//end
				}

				//�������������ʱ���ɽ�����Ⱦ
				if (zdepth < my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)])
				{


					//��ȸ���
					my_framebuffer.ssaa_depthbuffer[get_index_ssaa(x, y)] = zdepth;



					Eigen::Vector3f fragment_color;
					//ʹ��switch��䣬���Ծ������ǵ����ĸ���ɫ��
					switch (select_myshader)
					{
					case test_shader:
						fragment_color = my_fragmentshader.testshader();
						break;

					case phong_shader:
						


						//-1 ��ֵ����
						Eigen::Vector4f lerp_view_pos = view_pos[0] * wa + view_pos[1] * wb + view_pos[2] * wc;

						//-2 ��ֵ����
						Eigen::Vector4f lerp_normal = newtri.trivertex[0].normal * wa + newtri.trivertex[1].normal * wb + newtri.trivertex[2].normal * wc;

						// -3 ��ֵ��������

						Eigen::Vector2f lerp_texcoord = newtri.trivertex[0].texcoord * wa + newtri.trivertex[1].texcoord * wb + newtri.trivertex[2].texcoord * wc;


						fragment_payload payload;
						payload.view_pos = lerp_view_pos;
						payload.normal = lerp_normal;
						payload.texcoord = lerp_texcoord;

						if (ismimmap)//���������minmap����ʹ��minmap�㼶
						{
							
							int minmap_index = get_minmap_index(newtri, wa, wb, wc);//�˺������ڻ�ȡmimmap�㼶
							
							payload.color = tex->get_color_mipmap(lerp_texcoord.x(), lerp_texcoord.y(), minmap_index); 
																									

							
						}																		

						
						//���������minmap����ʹ��˫���Բ�ֵ
						if(!ismimmap)payload.color = tex->getcolor_lerpbinary(lerp_texcoord.x(), lerp_texcoord.y());
						fragment_color = my_fragmentshader.fragment_phong_shader(payload);
						break;
					}
						//���յ���ɫȷ��
						my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x, y)] = fragment_color;
					}
				
			}

		}
	}
	std::cout << count << std::endl;

}

//��Ⱦshadow mapping�Ĺ�դ������
void rasterizer::rasterF(triangle& newtri)
{
	//��ȷ��AABB��Χ��
	float xmin = std::max(0.0f, std::min(newtri.trivertex[0].v.x(), std::min(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x()))) * ssaa_factor;
	float xmax = std::min(float(WIDTH - 1), std::max(newtri.trivertex[0].v.x(), std::max(newtri.trivertex[1].v.x(), newtri.trivertex[2].v.x()))) * ssaa_factor;
	float ymin = std::max(0.0f, std::min(newtri.trivertex[0].v.y(), std::min(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y()))) * ssaa_factor;
	float ymax = std::min(float(HEIGHT - 1), std::max(newtri.trivertex[0].v.y(), std::max(newtri.trivertex[1].v.y(), newtri.trivertex[2].v.y()))) * ssaa_factor;

	
	//�������е�����
	for (int y = floor(ymin); y < ceil(ymax); y++)
	{
		for (int x = floor(xmin); x < ceil(xmax); x++)
		{
			
			float pixel_x = (x + 0.5) / ssaa_factor;
			float pixel_y = (y + 0.5) / ssaa_factor;



			//�жϵ��Ƿ��������ε��ڲ�
			if (isinserttriangle(pixel_x, pixel_y, newtri)) {


				//�������������
				auto [alpha, beta, gamma] = barycentic(pixel_x, pixel_y, newtri);
				//�����ʵ�Ĳ�ֵ����
				//����ͶӰ��ȡ��������������͸��ͶӰ������
				float wa = alpha;
				float wb = beta;
				float wc = gamma;
				//�Ȼ�ò�ֵ��Zֵ�������ж��Ƿ��ܽ�����ɫ
				//std::cout << newtri.trivertex[0].v.z() <<" " << newtri.trivertex[1].v.z()<<" " << newtri.trivertex[2].v.z() << std::endl;
				float zdepth = newtri.trivertex[0].v.z() * wa + newtri.trivertex[1].v.z() * wb + newtri.trivertex[2].v.z() * wc; //�õ���Ļ�ռ��Zֵ
				
				//�������������ʱ���ɽ�����Ⱦ
				if (zdepth < my_framebuffer.ssaa_shadowbuffer[get_index_ssaa(x, y)])
				{
					//��ȸ���
					my_framebuffer.ssaa_shadowbuffer[get_index_ssaa(x, y)] = zdepth;
					//my_framebuffer.ssaa_colorbuffer[get_index_ssaa(x, y)] = Eigen::Vector3f(zdepth*255*7, zdepth*255*5*7, zdepth*255*5*7);
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

	//shadow mapping
	std::fill(my_framebuffer.ssaa_shadowbuffer.begin(), my_framebuffer.ssaa_shadowbuffer.end(), std::numeric_limits<float>::max());
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
int rasterizer::get_minmap_index(triangle newtri,float wa,float wb,float wc)
{
	// 1. ��ȡ�����������������Ļ�������������
	Eigen::Vector2f p0(newtri.trivertex[0].v.x(), newtri.trivertex[0].v.y()); // ��Ļ����(x,y)
	Eigen::Vector2f p1(newtri.trivertex[1].v.x(), newtri.trivertex[1].v.y());
	Eigen::Vector2f p2(newtri.trivertex[2].v.x(), newtri.trivertex[2].v.y());
	Eigen::Vector2f t0(newtri.trivertex[0].texcoord.x(), newtri.trivertex[0].texcoord.y()); // ��������(u,v)
	Eigen::Vector2f t1(newtri.trivertex[1].texcoord.x(), newtri.trivertex[1].texcoord.y());
	Eigen::Vector2f t2(newtri.trivertex[2].texcoord.x(), newtri.trivertex[2].texcoord.y());

	// 2. �����Է����飬���������������Ļ�����ƫ�� (du/dx, du/dy, dv/dx, dv/dy)
	// ���ڹ�ʽ��u = a*x + b*y + c����������������ȥc����a��b
	float dx0 = p0.x() - p2.x(); // x0 - x2
	float dy0 = p0.y() - p2.y(); // y0 - y2
	float dx1 = p1.x() - p2.x(); // x1 - x2
	float dy1 = p1.y() - p2.y(); // y1 - y2
	float du0 = t0.x() - t2.x(); // u0 - u2
	float du1 = t1.x() - t2.x(); // u1 - u2
	float dv0 = t0.y() - t2.y(); // v0 - v2
	float dv1 = t1.y() - t2.y(); // v1 - v2

	// ϵ����������ʽ��������㣩
	float det = dx0 * dy1 - dy0 * dx1;
	if (std::abs(det) < 1e-6) {
		return 0; // �������˻���������Ͳ㼶
	}

	// ����u��ƫ����a=du/dx, b=du/dy
	float a = (du0 * dy1 - dy0 * du1) / det;
	float b = (dx0 * du1 - du0 * dx1) / det;
	// ����v��ƫ����d=dv/dx, e=dv/dy
	float d = (dv0 * dy1 - dy0 * dv1) / det;
	float e = (dx0 * dv1 - dv0 * dx1) / det;

	// 3. ������Ļ1�����ض�Ӧ����������������rho_x, rho_y��
	// a�ǹ�һ������u����Ļx��ƫ�������������ȵõ���������
	float rho_x = std::sqrt(std::pow(a * tex->width, 2) + std::pow(d * tex->width, 2));
	float rho_y = std::sqrt(std::pow(b * tex->height, 2) + std::pow(e * tex->height, 2));

	// 4. ����max_dis��lod�㼶
	float max_dis = std::max(rho_x, rho_y);
	float lod = std::log2(std::max(max_dis, 1.0f)); // ȷ��lod >=0������log2(0)��
	int minmap_level = std::clamp(static_cast<int>(lod), 0, static_cast<int>(tex->mipmap_texture_data.size() - 1));

	return minmap_level;
}
