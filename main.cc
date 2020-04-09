
#include "common.h"

#include <omp.h>
#include <cstdio>
#include <vector>
#include <tuple>
#include <cmath>

#include "vec.h"
#include "Texture.h"
#include "TextureCube.h"
#include "Camera.h"
#include "WormholeODE.h"

#include "AESRNG.h"

std::uint32_t constexpr SAMPLE_PER_PIXEL = 4;
std::uint32_t constexpr IMAGE_WIDTH = 1920;
std::uint32_t constexpr IMAGE_HEIGHT = 1080;

void build_camera_local_transformation(Math::Vector4 const &ray_origin, Math::Vector4 const &ray_dir, Math::Matrix4x4 &ans, Math::Matrix4x4 &inverse)
{
	using namespace Math;
	Vector4 new_x, new_y, new_z(0, 0, 1, 0);
	new_x.x = -ray_origin.x;
	new_x.y = -ray_origin.y;
	new_x.z = -ray_origin.z;
	new_x.w = 0.0;
	Vector4Normalize(new_x);

	Vector4 tmp;
	Vector4Dot(new_x, ray_dir, tmp);

	if (abs(abs(tmp.x) - 1.0) < 1e-8) // BUG: nan if ray direction and z axis are collinear
	{
		Vector4Cross(new_x, new_z, new_y);
		Vector4Cross(new_y, new_x, new_z);
	}
	else
	{
		Vector4Cross(ray_dir, new_x, new_z);
		Vector4Cross(new_x, new_z, new_y);
	}


	Vector4Normalize(new_y);
	Vector4Normalize(new_z);

	ans._11 = new_x.x; ans._12 = new_x.y; ans._13 = new_x.z; ans._14 = 0;
	ans._21 = new_y.x; ans._22 = new_y.y; ans._23 = new_y.z; ans._24 = 0;
	ans._31 = new_z.x; ans._32 = new_z.y; ans._33 = new_z.z; ans._34 = 0;
	ans._41 = 0; ans._42 = 0; ans._43 = 0; ans._44 = 1.0;

	Matrix4x4Inverse(ans, inverse);
}



int main()
{
	using namespace Math;

	COMScope com_s;

	TextureCube skymap1;
	skymap1.Load(L"skybox_earth.png");

	TextureCube skymap2;
	skymap2.Load(L"skybox_star.png");

	Math::Camera cam;
	cam.LookAt(4, 4, 0, 0, 0, 0);
	cam.SetAspectFOV(65, IMAGE_WIDTH, IMAGE_HEIGHT);

	Texture bmp(IMAGE_WIDTH, IMAGE_HEIGHT);
	Texture dir(IMAGE_WIDTH, IMAGE_HEIGHT);
	int row, col;

	omp_set_num_threads(24);

	double const wUnit = 2.0 / bmp.width, hUnit = 2.0 / bmp.height;

#pragma omp parallel for schedule(dynamic, 1) private(row)
	for (row = 0; row < bmp.height; ++row)
	{
		Utils::StandardRNG rng;
		fprintf(stderr, "\rRendering %5.3f%%", 100. * row / (bmp.height - 1));
		for (col = 0; col < bmp.width; ++col)
		{
			double x = (double)col / (double)bmp.width * 2.0 - 1.0;
			double y = (double)row / (double)bmp.height;

			y = y * 2.0 - 1.0;

			x -= wUnit / 2.0;
			y -= hUnit / 2.0;

			float32x4 finalVal;

			for (int i = 0; i < SAMPLE_PER_PIXEL; ++i)
			{
				Math::Vector4 ray_origin, ray_dir;
				double x_offset, y_offset;
				rng.NextDoubles(x_offset, y_offset);
				//x_offset = y_offset = 0;
				x_offset *= wUnit;
				y_offset *= hUnit;
				cam.GenerateRay(x + x_offset, y + y_offset, ray_origin, ray_dir);

				Matrix4x4 camera_frame_to_global_frame, global_frame_to_camera_frame;
				build_camera_local_transformation(ray_origin, ray_dir, camera_frame_to_global_frame, global_frame_to_camera_frame);

				Vector4 ray_dir_camera_frame;
				Matrix4x4VectorMulVec(ray_dir, global_frame_to_camera_frame, ray_dir_camera_frame);
				double ray_phi_camera = std::atan2(ray_dir_camera_frame.y, ray_dir_camera_frame.x);

				auto [traced_ray_phi, traced_ray_l] = equatorial_phi_mapping(ray_phi_camera, Math::Vector4Length(ray_origin), 0, 0.1, 0.7);
				//traced_ray_phi= ray_phi_camera

				double local_x = -std::cos(traced_ray_phi);
				double local_y = -std::sin(traced_ray_phi);
				double local_z = 0;

				if (traced_ray_l > 0) {
					Vector4 traced_ray_dir_camera(local_x, local_y, local_z, 0); // camera frame

					Vector4 traced_ray_dir;
					Matrix4x4VectorMulVec(traced_ray_dir_camera, camera_frame_to_global_frame, traced_ray_dir); // to global frame

					finalVal = finalVal + skymap1.Sample(traced_ray_dir.x, traced_ray_dir.y, traced_ray_dir.z); // local sky
					dir(row, col) = float32x4(traced_ray_dir.x, traced_ray_dir.y, traced_ray_dir.z);
				}
				else
				{
					Vector4 traced_ray_dir_camera(local_x, local_y, local_z, 0); // camera frame

					Vector4 traced_ray_dir;
					Matrix4x4VectorMulVec(traced_ray_dir_camera, camera_frame_to_global_frame, traced_ray_dir); // to global frame

					finalVal = finalVal + skymap2.Sample(traced_ray_dir.x, traced_ray_dir.y, traced_ray_dir.z); // local sky
					dir(row, col) = float32x4(traced_ray_dir.x, traced_ray_dir.y, traced_ray_dir.z);
				}
			}

			finalVal = finalVal / SAMPLE_PER_PIXEL;
			bmp(row, col) = finalVal;
		}
	}

	bmp.Save(L"result.bmp");
	dir.Save(L"dir.bmp", true);

	return 0;
}

/*
int main()
{
	COMScope com_s;

	TextureCube skymap1;
	skymap1.Load(L"skybox_earth.png");

	Math::Camera cam;
	cam.LookAt(4, 0, 0, 0, 0.3, 0);
	cam.SetAspectFOV(65, IMAGE_WIDTH, IMAGE_HEIGHT);

	Texture bmp(IMAGE_WIDTH, IMAGE_HEIGHT);
	Texture dir(IMAGE_WIDTH, IMAGE_HEIGHT);
	int row, col;

	omp_set_num_threads(24);

	double const wUnit = 2.0 / bmp.width, hUnit = 2.0 / bmp.height;

#pragma omp parallel for schedule(dynamic, 1) private(row)
	for (row = 0; row < bmp.height; ++row)
	{
		Utils::StandardRNG rng;
		fprintf(stderr, "\rRendering %5.3f%%", 100. * row / (bmp.height - 1));
		for (col = 0; col < bmp.width; ++col)
		{
			double x = (double)col / (double)bmp.width * 2.0 - 1.0;
			double y = (double)row / (double)bmp.height;

			y = y * 2.0 - 1.0;

			//y = 1.0;
			//x = -0.1;

			x -= wUnit / 2.0;
			y -= hUnit / 2.0;

			//x = y = 1;
			//x = 0;

			float32x4 finalVal;

			for (int i = 0; i < SAMPLE_PER_PIXEL; ++i)
			{
				Math::Vector4 ray_origin, ray_dir;
				double x_offset, y_offset;
				//rng.NextDoubles(x_offset, y_offset);
				x_offset = y_offset = 0.5;
				x_offset *= wUnit;
				y_offset *= hUnit;
				cam.GenerateRay(x + x_offset, y + y_offset, ray_origin, ray_dir);

				//

				//auto [local_x, local_y, local_z, l] = trace(ray_origin, ray_dir);

				//if (l > 0) {
				//	finalVal = finalVal + skymap1.Sample(local_x, local_y, local_z); // local sky
				//	dir(row, col) = float32x4(local_x, local_y, local_z);
				//}
				//else
				//	dir(row, col) = float32x4(0); // otherside of space
				//finalVal = finalVal + skymap1.Sample(ray_dir.x, ray_dir.y, ray_dir.z);
				//dir(row, col) = float32x4(ray_dir.x, ray_dir.y, ray_dir.z);
			}

			//x = .1;

			////x -= wUnit / 2.0;
			////y -= hUnit / 2.0;

			//{
			//	Math::Vector4 ray_origin, ray_dir;
			//	cam.GenerateRay(x + 0, y + 0, ray_origin, ray_dir);

			//	auto [local_x, local_y, local_z, l] = trace(ray_origin, ray_dir);
			//	if (l > 0) {
			//		finalVal = finalVal + skymap1.Sample(local_x, local_y, local_z); // local sky
			//		dir(row, col) = float32x4(local_x, local_y, local_z);
			//	}
			//	else
			//		dir(row, col) = float32x4(0); // otherside of space
			//}

			finalVal = finalVal / SAMPLE_PER_PIXEL;
			bmp(row, col) = finalVal;
		}
	}

	bmp.Save(L"result.bmp");
	dir.Save(L"dir.bmp", true);

	return 0;
}
*/
int main3()
{
	using namespace Math;

	__m128 v1{ 1.0f,2.f,3.f,4.f };
	__m128 v2{ 5.0f,6.f,7.f,8.f };

	auto ret = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(2, 3, 1, 3));

	Vector4 v1d{ 1.0,2.,3.,4. };
	Vector4 v2d{ 5.0,6.,7.,8. };

	auto retd = Vector4Shuffle<2, 3, 1, 3>(v1d, v2d);

	Vector4 pos(1, 1, 0, 1);
	Vector4 dir(-0.6, 1.8, -8.1, 0);
	Vector4Normalize(dir);

	Matrix4x4 ans, inv;
	build_camera_local_transformation(pos, dir, ans, inv);

	Vector4 v_f2;
	Matrix4x4VectorMulVec(dir, inv, v_f2);

	//v_f2.x = v_f2.y = v_f2.z = 1;
	//Vector4Normalize(v_f2);

	double phi_cs = std::atan2(v_f2.y, v_f2.x);
	double theta_cs = std::acos(v_f2.z); // always pi/2

	// reconstruct dir from ans and phi_cs

	double local_x = std::cos(phi_cs);
	double local_y = std::sin(phi_cs);
	double local_z = 0;

	Vector4 v_f2_recon(local_x, local_y, local_z, 0);

	Vector4 v_f1;
	Matrix4x4VectorMulVec(v_f2_recon, ans, v_f1);

	double dot = Vector4Dot(dir, v_f1);//should be 1

	return 0;
}