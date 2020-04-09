#pragma once

#include <immintrin.h>
#include <cstdint>

struct float32x4
{
	float32x4() :x(0), y(0), z(0), w(0)
	{
		;
	}

	float32x4(float a, float b, float c, float d = 0.0f) :x(a), y(b), z(c), w(d)
	{

	}

	float32x4(float repeat) :x(repeat), y(repeat), z(repeat), w(repeat)
	{

	}

	float32x4(__m128 v) :vec(v)
	{
		;
	}

	operator const __m128()
	{
		return vec;
	}

	union alignas(32)
	{
		std::uint32_t iVal[4];
		float values[4];
		struct
		{
			float x, y, z, w;
		};
		__m128 vec;
	};
};

auto operator+(float32x4 const &lhs, float32x4 const &rhs)
{
	return float32x4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template<typename T>
auto operator+(float32x4 const &lhs, T rhs)
{
	return float32x4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
}

template<typename T>
auto operator+(T lhs, float32x4 const &rhs)
{
	return float32x4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
}

auto operator-(float32x4 const &lhs, float32x4 const &rhs)
{
	return float32x4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

template<typename T>
auto operator-(float32x4 const &lhs, T rhs)
{
	return float32x4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
}

template<typename T>
auto operator-(T lhs, float32x4 const &rhs)
{
	return float32x4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
}

auto operator*(float32x4 const &lhs, float32x4 const &rhs)
{
	return float32x4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

template<typename T>
auto operator*(float32x4 const &lhs, T rhs)
{
	return float32x4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

template<typename T>
auto operator*(T lhs, float32x4 const &rhs)
{
	return float32x4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
}

auto operator/(float32x4 const &lhs, float32x4 const &rhs)
{
	return float32x4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

template<typename T>
auto operator/(float32x4 const &lhs, T rhs)
{
	return float32x4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}

template<typename T>
auto operator/(T lhs, float32x4 const &rhs)
{
	return float32x4(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
}

inline static float32x4 lerp(float x, float32x4 const &v0, float32x4 const &v1)
{
	__m128 tmp = _mm_sub_ps(v1.vec, v0.vec);
	__m128 _x = _mm_load1_ps(std::addressof(x));
	return _mm_fmadd_ps(_x, tmp, v0.vec);
}
