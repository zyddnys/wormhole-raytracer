#pragma once

#include <cstdlib>
#include <wmmintrin.h>
#include <stdint.h>
#include <omp.h>

const double g_Uint64DoubleMax = 2e-53;

namespace Utils
{
static omp_lock_t g_RNGMUX;
template<int round>
class AESRNG
{
public:

	AESRNG()
	{
		//omp_set_lock( &g_RNGMUX );
		m_key.m128i_i32[0] = std::rand();
		m_key.m128i_i32[1] = std::rand();
		m_key.m128i_i32[2] = std::rand();
		m_key.m128i_i32[3] = std::rand();
		m_result.m128i_i32[0] = std::rand();
		m_result.m128i_i32[1] = std::rand();
		m_result.m128i_i32[2] = std::rand();
		m_result.m128i_i32[3] = std::rand();
		//omp_unset_lock( &g_RNGMUX );
	}

	~AESRNG()
	{
	}

	AESRNG(uint64_t key1, uint64_t key2, uint64_t iv1, uint64_t iv2)
	{
		m_key.m128i_u64[0] = key1;
		m_key.m128i_u64[1] = key1;
		m_result.m128i_u64[0] = iv1;
		m_result.m128i_u64[1] = iv2;
	}
public:
	void NextBlock(__m128i &result)
	{
		__m128i tmp = _mm_loadu_si128(&m_result);
		for (int i = 0; i < round; ++i)
			tmp = _mm_aesenc_si128(tmp, m_key);
		result = m_result = tmp;
	}
public:
	void NextDouble(double &result)
	{
		__m128i tmp;
		NextBlock(tmp);
		uint64_t num = (uint64_t)_mm_cvtsi128_si64(tmp);
		result = (num >> 3) * g_Uint64DoubleMax;
	}

	void NextDoubles(double &result1, double &result2)
	{
		__m128i tmp;
		NextBlock(tmp);;
		result1 = (tmp.m128i_u64[0] >> 3) * g_Uint64DoubleMax;
		result2 = (tmp.m128i_u64[1] >> 3) * g_Uint64DoubleMax;
	}
private:
	__m128i m_key;
	__m128i m_result;
};

using StandardRNG = AESRNG<8>;

};
