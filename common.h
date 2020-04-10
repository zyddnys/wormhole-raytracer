#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <cmath>
#include <atlbase.h>
#include <wincodec.h>

#undef min
#undef max

template<typename T>
inline T constexpr clamp(T a, T min_v, T max_v)
{
	return std::max(std::min(a, max_v), min_v);
}

//inline float constexpr saturate(float a)
//{
//	return std::max(std::min(a, 1.0f), 0.0f);
//}

inline double constexpr saturate(double a)
{
	return std::max(std::min(a, 1.), 0.);
}

template<typename T, typename T2>
inline auto constexpr lerp(T2 t, T v0, T v1)
{
	return (1 - t) * v0 + t * v1;
}

inline std::uint8_t constexpr float2byte(float a, float scale = 255.0)
{
	float v(clamp(a * scale, 0.0f, 255.0f));
	return static_cast<std::uint8_t>(v);
}

struct COMScope
{
	COMScope()
	{
		CoInitialize(nullptr);
	}
	~COMScope()
	{
		CoUninitialize();
	}
};

template<typename TNumber, typename TFunc, typename... Args>
TNumber runge_kutta_4_step(double x, TNumber y, double h, TFunc func, Args &&... args)
{
	TNumber k1(h * func(x, y, std::forward<Args>(args)...));
	TNumber k2(h * func(x + 0.5 * h, y + 0.5 * k1, std::forward<Args>(args)...));
	TNumber k3(h * func(x + 0.5 * h, y + 0.5 * k2, std::forward<Args>(args)...));
	TNumber k4(h * func(x + h, y + k3, std::forward<Args>(args)...));

	return y + (1.0 / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}

double const g_pi = std::acos(-1.);
double const g_2pi = 2 * std::acos(-1.);
