#pragma once

#include "Texture.h"

#include <tuple>

struct TexturePanoramic : Texture
{
	template<typename... Args>
	TexturePanoramic(Args &&... args) :Texture(std::forward<Args>(args)...)
	{
		;
	}

	float32x4 Sample(double x, double y, double z)
	{
		if (std::abs(std::abs(z) - 1) < 1e-8)
		{
			// coord singularity
			if (z > 0)
				return reinterpret_cast<Texture *>(this)->Sample(0, 0);
			else
				return reinterpret_cast<Texture *>(this)->Sample(0, 1);
		}

		double phi = std::atan2(y, x);
		//double theta = std::acos(ray_dir_cs.z);
		phi = std::fmod(phi + g_2pi, g_2pi) / g_2pi;
		//std::printf("%lf\n", phi);
		z = (z + 1) * 0.5;

		return reinterpret_cast<Texture *>(this)->Sample(phi, z);
	}
};

