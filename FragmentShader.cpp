#include "FragmentShader.h"

fragmentshader::fragmentshader() {

}

Eigen::Vector3f  fragmentshader::testshader() {

	return Eigen::Vector3f(0.9f, 0.9f, 0.9f)*255.0f;//默认返回红色
}
Eigen::Vector3f fragmentshader::fragment_phong_shader(fragment_payload payload) {

	Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
	Eigen::Vector3f kd = payload.color;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

	////定义两个点光源(视图空间下)
	auto l1 = light{ Eigen::Vector3f(20,15,20),Eigen::Vector3f(500,500,500) };
	auto l2 = light{ Eigen::Vector3f(-30,20,10),Eigen::Vector3f(500,500,500) };

	std::vector<light> lights = { l1,l2 };
	Eigen::Vector3f amb_light_intensity{ 10,10,10 }; //环境光强度
	Eigen::Vector3f eye_pos{ 0,0,10 };

	float p = 150;
	Eigen::Vector3f color = payload.color; //转三维
	Eigen::Vector3f point = Eigen::Vector3f(payload.view_pos.x(), payload.view_pos.y(), payload.view_pos.z());
	Eigen::Vector3f normal = Eigen::Vector3f(payload.normal.x(),payload.normal.y(),payload.normal.z());
	Eigen::Vector3f result_color{ 0,0,0 };

	for (auto& light : lights)
	{
		Eigen::Vector3f lightdir = (point - light.position).normalized();
		float lightdis = (point - light.position).dot(point - light.position);
		Eigen::Vector3f eyedir = (point - eye_pos).normalized();
		Eigen::Vector3f normal_ = normal.normalized();
		Eigen::Vector3f half_vector = (lightdir + eyedir).normalized();
		//环境光
		Eigen::Vector3f ambint = Eigen::Vector3f(ka.x()*amb_light_intensity.x(), ka.y() * amb_light_intensity.y(),
			ka.z() * amb_light_intensity.z());

		//漫反射
		Eigen::Vector3f diffuse =Eigen::Vector3f(kd.x()*light.intensity.x()/lightdis*(normal_.dot(lightdir)),
			kd.y() * light.intensity.y() / lightdis * (normal_.dot(lightdir)),
			kd.z() * light.intensity.z() / lightdis * (normal_.dot(lightdir)));

		////镜面反射
		Eigen::Vector3f specular = Eigen::Vector3f(ks.x() * light.intensity.x() / lightdis * (normal_.dot(half_vector)),
			ks.y() * light.intensity.y() / lightdis * (normal_.dot(half_vector)),
			ks.z() * light.intensity.z() / lightdis * (normal_.dot(half_vector)));
		result_color += ambint + diffuse + specular;
	}
	return result_color;
}