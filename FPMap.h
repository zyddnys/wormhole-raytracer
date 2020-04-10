#pragma once

#include "common.h"

#include <memory>
#include <optional>

//template<typename T>
//struct FPCache
//{
//	T max_value;
//	std::size_t cache_size;
//
//	T scale;
//
//	std::unique_ptr<T> cache;
//	std::unique_ptr<std::uint32_t> cache_exist;
//
//	FPCache(T max_value, std::size_t cache_size) max_value(max_value), cache_size(cache_size)
//	{
//		scale = cache_size / max_value;
//		cache.reset(new T[cache_size]);
//		cache_exist.reset(new std::uint32_t[cache_size]);
//		std:memset(cache_exist.get(), 0, cache_size * sizeof(std::uint32_t));
//	}
//
//	std::optional<T> operator()(T key)
//	{
//		auto val = clamp(key, 0.0, max_value);
//		std::uint64_t val_int(val * scale);
//		if (cache_exist[val_int])
//			return { cache[val_int] };
//		else
//			return {};
//	}
//
//	void Put(T key, T val)
//	{
//		key = clamp(val, 0.0, max_value);
//		std::uint64_t key_int(key * scale);
//		cache[key_int] = val;
//		cache_exist[key_int] = 1;
//	}
//};

template<typename TValue>
struct FPMap
{
	// TODO: race condition with OpenMP
	std::unique_ptr<TValue[]> value;
	std::unique_ptr<std::uint32_t[]> valid;

	std::uint64_t subdivisions;

	FPMap(std::uint64_t subdivisions) : subdivisions(subdivisions)
	{
		value = std::make_unique<TValue[]>(subdivisions + 1);
		valid = std::make_unique<std::uint32_t[]>(subdivisions + 1);
		std::memset(valid.get(), 0, (subdivisions + 1) * sizeof(std::uint32_t));
	}

	void Put(double key, TValue const &val)
	{
		key = saturate(key);
		std::size_t key_pos(key * subdivisions);
		value[key_pos] = val;
		valid[key_pos] = 1;
	}

	std::optional<TValue> Get(double key)
	{
		key = saturate(key);
		std::size_t key_pos(key * subdivisions);
		if (valid[key_pos])
			return { value[key_pos] };
		else
			return {};
	}

	std::optional<TValue> GetInterpolated(double key)
	{
		key = saturate(key);
		return {};
	}
};
