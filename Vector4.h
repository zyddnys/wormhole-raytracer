#pragma once

#include <immintrin.h>
#include <cmath>
#include <stdint.h>

namespace Math
{

	class Vector4
	{
	public:

		Vector4()
		{
		}

		constexpr Vector4( double a, double b, double  c, double d ) :x( a ), y( b ), z( c ), w( d )
		{
		}

		constexpr Vector4( double a, double b, double  c ) : x( a ), y( b ), z( c ), w( 0.0 )
		{
		}

		constexpr Vector4( double a ) : x( a ), y( a ), z( a ), w( 0.0 )
		{
		}

		Vector4( const __m256d & vec2 ) :vec( vec2 )
		{
		}

		~Vector4()
		{
		}

		void Zero()
		{
			vec = _mm256_setzero_pd();
		}

		void SetXYZ( const Vector4& a )
		{
			x = a.x;
			y = a.y;
			z = a.z;
		}

		Vector4 Neg() const
		{
			Vector4 tmp;
			__m256d zeros = _mm256_setzero_pd();
			tmp.vec = _mm256_sub_pd( zeros, vec );
			return tmp;
		}
	public:
		operator const __m256d( )
		{
			return vec;
		}

	public:
		union alignas( 32 )
		{
			uint64_t iVal[4];
			double values[4];
			struct
			{
				double x, y, z, w;
			};
			__m256d vec;
		};
	};

	const Vector4 g_Vector4Inf = { 9218868437227405312ul,9218868437227405312ul ,9218868437227405312ul ,9218868437227405312ul };
	const Vector4 g_Vector4NaN = { 18444492273895866368ul,18444492273895866368ul ,18444492273895866368ul ,18444492273895866368ul };
	const Vector4 g_Vector4One = { 1.0,1.0 ,1.0 ,1.0 };

	double const g_PI = std::acos( -1. );

#define SHUFFLE( v, x ) _mm256_permute4x64_pd(v,x)

	inline static void Vector4Add( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		result.vec = _mm256_add_pd( lhs.vec, rhs.vec );
	}

	inline static void Vector4Add( const Vector4& lhs, const double& rhs, Vector4& result )
	{
		__m256d a = _mm256_broadcast_sd( &rhs );
		result.vec = _mm256_add_pd( lhs.vec, a );
	}

	inline static void Vector4Mul( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		result.vec = _mm256_mul_pd( lhs.vec, rhs.vec );
	}

	inline static void Vector4Mul( const Vector4& lhs, const double& rhs, Vector4& result )
	{
		__m256d a = _mm256_broadcast_sd( &rhs );
		result.vec = _mm256_mul_pd( lhs.vec, a );
	}

	inline static void Vector4Sub( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		result.vec = _mm256_sub_pd( lhs.vec, rhs.vec );
	}

	inline static void Vector4Sub( const Vector4& lhs, const double& rhs, Vector4& result )
	{
		__m256d a = _mm256_broadcast_sd( &rhs );
		result.vec = _mm256_sub_pd( lhs.vec, a );
	}

	inline static void Vector4Div( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		result.vec = _mm256_div_pd( lhs.vec, rhs.vec );
	}

	inline static void Vector4Div( const Vector4& lhs, const double& rhs, Vector4& result )
	{
		__m256d a = _mm256_broadcast_sd( &rhs );
		result.vec = _mm256_div_pd( lhs.vec, a );
	}

	inline static void Vector4Homogeneous( const Vector4& lhs, Vector4& result )
	{
		Vector4Div( lhs, lhs.w, result );
	}

	inline static void Vector4Homogeneous( Vector4& in_out )
	{
		Vector4Div( in_out, in_out.w, in_out );
	}

	static inline double horizontal_add( __m256d a )
	{
		__m256d t1 = _mm256_hadd_pd( a, a );
		__m128d t2 = _mm256_extractf128_pd( t1, 1 );
		__m128d t3 = _mm_add_sd( _mm256_castpd256_pd128( t1 ), t2 );
		return _mm_cvtsd_f64( t3 );
	}

	inline static void Vector4Sqr( const Vector4& lhs, Vector4& result )
	{
		result.vec = _mm256_mul_pd( lhs.vec, lhs.vec );
	}

	inline static void Vector4Dot( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		__m256d vDot = _mm256_mul_pd( lhs.vec, rhs.vec );
		// x=Dot.vector4_f32[1], y=Dot.vector4_f32[2]
		__m256d vTemp = SHUFFLE( vDot, _MM_SHUFFLE( 2, 1, 2, 1 ) );
		// Result.vector4_f32[0] = x+y
		vDot = _mm256_add_pd( vDot, vTemp );
		// x=Dot.vector4_f32[2]
		vTemp = SHUFFLE( vTemp, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		// Result.vector4_f32[0] = (x+y)+z
		vDot = _mm256_add_pd( vDot, vTemp );
		// Splat x
		result.vec = SHUFFLE( vDot, _MM_SHUFFLE( 0, 0, 0, 0 ) );
	}

	inline static double Vector4Dot( const Vector4& lhs, const Vector4& rhs )
	{
		Vector4 tmp;
		Vector4Dot( lhs, rhs, tmp );
		return tmp.x;
	}

	inline static void Vector4Dot4( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		double val = horizontal_add( _mm256_mul_pd( lhs.vec, rhs.vec ) );
		result.vec = _mm256_broadcast_sd( &val );
	}

	inline static void Vector4Dot4(
		const Vector4& lhs1, const Vector4& rhs1, Vector4& result1,
		const Vector4& lhs2, const Vector4& rhs2, Vector4& result2,
		const Vector4& lhs3, const Vector4& rhs3, Vector4& result3,
		const Vector4& lhs4, const Vector4& rhs4, Vector4& result4
	)
	{
		__m256d xy0 = _mm256_mul_pd( lhs1.vec, rhs1.vec );
		__m256d xy1 = _mm256_mul_pd( lhs2.vec, rhs2.vec );
		__m256d xy2 = _mm256_mul_pd( lhs3.vec, rhs3.vec );
		__m256d xy3 = _mm256_mul_pd( lhs4.vec, rhs4.vec );

		// low to high: xy00+xy01 xy10+xy11 xy02+xy03 xy12+xy13
		__m256d temp01 = _mm256_hadd_pd( xy0, xy1 );

		// low to high: xy20+xy21 xy30+xy31 xy22+xy23 xy32+xy33
		__m256d temp23 = _mm256_hadd_pd( xy2, xy3 );

		// low to high: xy02+xy03 xy12+xy13 xy20+xy21 xy30+xy31
		__m256d swapped = _mm256_permute2f128_pd( temp01, temp23, 0x21 );

		// low to high: xy00+xy01 xy10+xy11 xy22+xy23 xy32+xy33
		__m256d blended = _mm256_blend_pd( temp01, temp23, 0b1100 );

		__m256d dotproduct = _mm256_add_pd( swapped, blended );

		result1 = SHUFFLE( dotproduct, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		result2 = SHUFFLE( dotproduct, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		result3 = SHUFFLE( dotproduct, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		result4 = SHUFFLE( dotproduct, _MM_SHUFFLE( 3, 3, 3, 3 ) );
	}

	template<unsigned int imm1, unsigned int imm2, unsigned int imm3, unsigned int imm4>
	inline static auto Vector4Shuffle(const Vector4 &a, const Vector4 &b)
	{
		auto tmpA = _mm256_permute4x64_pd(a.vec, _MM_SHUFFLE(imm1, imm2, imm3, imm4));
		auto tmpB = _mm256_permute4x64_pd(b.vec, _MM_SHUFFLE(imm1, imm2, imm3, imm4));
		return Vector4{ { {
			tmpA.m256d_f64[0],
			tmpA.m256d_f64[1],
			tmpB.m256d_f64[2],
			tmpB.m256d_f64[3]
		} } };
	}

	inline static void Vector4Cross( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	{
		__m256d a, b;
		a = _mm256_loadu_pd( &lhs.x );
		b = _mm256_loadu_pd( &rhs.x );
		result.vec = _mm256_sub_pd(
			_mm256_mul_pd( SHUFFLE( a, _MM_SHUFFLE( 3, 0, 2, 1 ) ), SHUFFLE( b, _MM_SHUFFLE( 3, 1, 0, 2 ) ) ),
			_mm256_mul_pd( SHUFFLE( a, _MM_SHUFFLE( 3, 1, 0, 2 ) ), SHUFFLE( b, _MM_SHUFFLE( 3, 0, 2, 1 ) ) )
		);
	}

	inline static int Vector4MaxAxis3( const Vector4& v )
	{
		if ( v.x > v.y && v.x > v.z )
			return 0;
		if ( v.y > v.x && v.y > v.z )
			return 1;
		if ( v.z > v.y && v.z > v.x )
			return 2;
		return 0;
	}

	inline static auto Vector4MergeXY(const Vector4 &lhs, const Vector4 &rhs)
	{
		//return _mm256_unpacklo_pd(lhs.vec, rhs.vec);
		Vector4 Result = { { {
			lhs.x,
			rhs.x,
			lhs.y,
			rhs.y,
		} } };
		return Result;
	}

	inline static auto Vector4MergeZW(const Vector4 &lhs, const Vector4 &rhs)
	{
		//return _mm256_unpackhi_pd(lhs.vec, rhs.vec);
		Vector4 Result = { { {
			lhs.z,
			rhs.z,
			lhs.w,
			rhs.w,
		} } };
		return Result;
	}

	// this 
	//inline static void Vector4CrossV2( const Vector4& V1, const Vector4& V2, Vector4& result )
	//{
	//	result.x = ( V1.y * V2.z ) - ( V1.z * V2.y );
	//	result.y = ( V1.z * V2.x ) - ( V1.x * V2.z );
	//	result.z = ( V1.x * V2.y ) - ( V1.y * V2.x );
	//	result.w = 0;
	//}

	inline static void Vector4LengthSq( const Vector4& lhs, Vector4& result )
	{
		Vector4Dot( lhs, lhs, result );
	}

	inline static void Vector4Length( const Vector4& lhs, Vector4& result )
	{
		Vector4 sq;
		Vector4Dot( lhs, lhs, sq );
		result = _mm256_sqrt_pd( sq );
	}

	inline static double Vector4Length(const Vector4 &lhs)
	{
		Vector4 sq;
		Vector4Dot(lhs, lhs, sq);
		auto result = _mm256_sqrt_pd(sq);
		return result.m256d_f64[0];
	}

	inline static void Vector4Normalize( const Vector4& lhs, Vector4& result )
	{
		// Perform the dot product on x,y and z only
		Vector4 vLengthSq = _mm256_mul_pd( lhs.vec, lhs.vec );
		Vector4 vTemp = SHUFFLE( vLengthSq, _MM_SHUFFLE( 2, 1, 2, 1 ) );
		vLengthSq = _mm256_add_pd( vLengthSq, vTemp );
		vTemp = SHUFFLE( vTemp, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		vLengthSq = _mm256_add_pd( vLengthSq, vTemp );
		vLengthSq = SHUFFLE( vLengthSq, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		vTemp = _mm256_sqrt_pd( vLengthSq );

		result = _mm256_div_pd( lhs.vec, vTemp );

		//Vector4 vZeroMask = _mm256_setzero_pd();
		//// Test for a divide by zero (Must be FP to detect -0.0)
		//vZeroMask = _mm256_cmp_pd( vZeroMask, result, _CMP_NEQ_UQ );
		//// Failsafe on zero (Or epsilon) length planes
		//// If the length is infinity, set the elements to zero
		//vLengthSq = _mm256_cmp_pd( vLengthSq, g_Vector4Inf.vec , _CMP_NEQ_UQ );
		//// Divide to perform the normalization
		//result = _mm256_div_pd( lhs.vec, result );
		//// Any that are infinity, set to zero
		//result = _mm256_and_pd( result, vZeroMask );
		//// Select qnan or result based on infinite length
		//Vector4 vTemp1 = _mm256_andnot_pd( vLengthSq, g_Vector4NaN.vec );
		//Vector4 vTemp2 = _mm256_and_pd( result, vLengthSq );
		//result = _mm256_or_pd( vTemp1, vTemp2 );
	}

	inline static void Vector4Normalize( Vector4& ans )
	{
		Vector4Normalize( ans, ans );
	}

	inline static void Vector4Normalize4( const Vector4& lhs, Vector4& result )
	{
		Vector4Dot4( lhs, lhs, result );
		result = _mm256_sqrt_pd( result );

		result = _mm256_div_pd( lhs.vec, result );

		//Vector4 vZeroMask = _mm256_setzero_pd();
		//// Test for a divide by zero (Must be FP to detect -0.0)
		//vZeroMask = _mm256_cmp_pd( vZeroMask, result, _CMP_NEQ_UQ );
		//// Failsafe on zero (Or epsilon) length planes
		//// If the length is infinity, set the elements to zero
		//vLengthSq = _mm256_cmp_pd( vLengthSq, g_Vector4Inf.vec , _CMP_NEQ_UQ );
		//// Divide to perform the normalization
		//result = _mm256_div_pd( lhs.vec, result );
		//// Any that are infinity, set to zero
		//result = _mm256_and_pd( result, vZeroMask );
		//// Select qnan or result based on infinite length
		//Vector4 vTemp1 = _mm256_andnot_pd( vLengthSq, g_Vector4NaN.vec );
		//Vector4 vTemp2 = _mm256_and_pd( result, vLengthSq );
		//result = _mm256_or_pd( vTemp1, vTemp2 );
	}

	//void add( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	//{
	//	register __m256d a = _mm256_loadu_pd( &lhs.x );
	//	register __m256d b = _mm256_loadu_pd( &rhs.x );
	//	register __m256d c = _mm256_add_pd( a, b );
	//	_mm256_storeu_pd( &result.x, c );
	//}
	//
	//void add2( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	//{
	//	result.vec = _mm256_add_pd( lhs.vec, rhs.vec );
	//}
	//
	//void add3( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	//{
	//	__asm
	//	{
	//		mov			eax, dword ptr [lhs]
	//		mov			ecx, dword ptr [rhs]
	//		vmovupd		ymm0, ymmword ptr [eax]
	//		vmovupd		ymm1, ymmword ptr [ecx]
	//		vaddpd		ymm0, ymm0, ymm1
	//		mov			eax, dword ptr [result]
	//		vmovupd		ymmword ptr [eax], ymm0
	//	}
	//}
	//
	//void add4( const Vector4& lhs, const Vector4& rhs, Vector4& result )
	//{
	//	result.x = lhs.x + rhs.x;
	//	result.y = lhs.y + rhs.y;
	//	result.z = lhs.z + rhs.z;
	//	result.w = lhs.w + rhs.w;
	//}

	void Vector4Sin(const Vector4 &lhs, Vector4 &result)
	{
		result.vec = _mm256_sin_pd(lhs.vec);
	}

};

namespace Utils
{
	using namespace Math;
	inline static void lerp( const Vector4& v0, const Vector4& v1, double t, Vector4& result )
	{
		const static double one = 1.0;
		register __m256d tv = _mm256_broadcast_sd( &t );
		register __m256d tmp = _mm256_broadcast_sd( &one );
		tmp = _mm256_sub_pd( tmp, tv );
		tmp = _mm256_mul_pd( tmp, v0.vec );
		result = _mm256_fmadd_pd( tv, v1.vec, tmp );
	}
}
