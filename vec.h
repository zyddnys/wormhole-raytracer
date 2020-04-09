#pragma once

#include <vector>
#include <array>

template<typename T, std::size_t N>
struct vec
{
	T v[N];

	T &operator[](std::size_t i)
	{
		return v[i];
	}

	T operator[](std::size_t i) const
	{
		return v[i];
	}
};

template<typename T, std::size_t N>
vec<T, N> operator+(vec<T, N> const &lhs, vec<T, N> const &rhs)
{
	vec<T, N> ret;
	for (auto i(0); i != N; ++i)
		ret[i] = lhs[i] + rhs[i];
	return ret;
}

template<typename T, std::size_t N>
vec<T, N> operator*(T const &lhs, vec<T, N> const &rhs)
{
	vec<T, N> ret;
	for (auto i(0); i != N; ++i)
		ret[i] = lhs * rhs[i];
	return ret;
}

using vec5d = vec<double, 5>;
using vec3d = vec<double, 3>;
