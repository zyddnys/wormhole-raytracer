#pragma once

#include "Texture.h"

#include <tuple>

struct TextureCube : Texture
{
	template<typename... Args>
	TextureCube(Args &&... args) :Texture(std::forward<Args>(args)...)
	{
		;
	}

	template<typename T>
	float32x4 Sample( T x,T y,T z )
	{
		auto ans = convert_xyz_to_cube_uv( x, y, z );
		auto index = std::get<0>( ans );
		auto v = std::get<1>( ans );
		auto u = std::get<2>( ans );
		u = 1.0 - u;
		switch ( index )
		{
		case 0:
			u += 1.0;
			v += 2.0;
			break;
		case 1:
			u += 1.0;
			v += 0.0;
			break;
		case 2:
			u += 0.0;
			v += 1.0;
			break;
		case 3:
			u += 2.0;
			v += 1.0;
			break;
		case 4:
			u += 1.0;
			v += 1.0;
			break;
		case 5:
			u += 1.0;
			v += 3.0;
			break;
		}
		u /= 3.0;
		v /= 4.0;
		return reinterpret_cast< Texture* >( this )->Sample( v, u );
	}
protected:
	template<typename T>
	std::tuple< int, T, T > convert_xyz_to_cube_uv(T x2, T y2, T z2)
	{
		auto x = y2;
		auto y = z2;
		auto z = x2;
		using namespace std;
		T absX = abs(x);
		T absY = abs(y);
		T absZ = abs(z);

		bool isXPositive = x > 0;
		bool isYPositive = y > 0;
		bool isZPositive = z > 0;

		T maxAxis = 0.0, uc = 0.0, vc = 0.0;
		int idx = 0;

		// POSITIVE X
		if (isXPositive && absX >= absY && absX >= absZ)
		{
			// u (0 to 1) goes from +z to -z
			// v (0 to 1) goes from -y to +y
			maxAxis = absX;
			uc = -z;
			vc = y;
			idx = 0;
		}
		// NEGATIVE X
		if (!isXPositive && absX >= absY && absX >= absZ)
		{
			// u (0 to 1) goes from -z to +z
			// v (0 to 1) goes from -y to +y
			maxAxis = absX;
			uc = z;
			vc = y;
			idx = 1;
		}
		// POSITIVE Y
		if (isYPositive && absY >= absX && absY >= absZ)
		{
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from +z to -z
			maxAxis = absY;
			uc = x;
			vc = -z;
			idx = 2;
		}
		// NEGATIVE Y
		if (!isYPositive && absY >= absX && absY >= absZ)
		{
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from -z to +z
			maxAxis = absY;
			uc = x;
			vc = z;
			idx = 3;
		}
		// POSITIVE Z
		if (isZPositive && absZ >= absX && absZ >= absY)
		{
			// u (0 to 1) goes from -x to +x
			// v (0 to 1) goes from -y to +y
			maxAxis = absZ;
			uc = x;
			vc = y;
			idx = 4;
		}
		// NEGATIVE Z
		if (!isZPositive && absZ >= absX && absZ >= absY)
		{
			// u (0 to 1) goes from +x to -x
			// v (0 to 1) goes from -y to +y
			maxAxis = absZ;
			uc = -x;
			vc = y;
			idx = 5;
		}
		// Convert range from -1 to 1 to 0 to 1
		return std::make_tuple(idx, 0.5 * (uc / maxAxis + 1.0), 0.5 * (vc / maxAxis + 1.0));
	}
};

