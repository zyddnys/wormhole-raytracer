#pragma once

#include "Vector4.h"
#include <stdlib.h>

namespace Math
{

	class Matrix4x4
	{
	public:

		Matrix4x4()
		{
			_11 = 1; _12 = 0; _13 = 0; _14 = 0;
			_21 = 0; _22 = 1; _23 = 0; _24 = 0;
			_31 = 0; _32 = 0; _33 = 1; _34 = 0;
			_41 = 0; _42 = 0; _43 = 0; _44 = 1;
		}

		Matrix4x4( Vector4 a, Vector4 b, Vector4 c, Vector4 d )
		{
			r[0] = a;
			r[1] = b;
			r[2] = c;
			r[3] = d;
		}

		Matrix4x4( const double m_0, const double m_1, const double m_2, const double m_3,
			const double m_4, const double m_5, const double m_6, const double m_7,
			const double m_8, const double m_9, const double m_10, const double m_11,
			const double m_12, const double m_13, const double m_14, const double m_15 )
		{
			this->m0 = m_0;
			this->m1 = m_1;
			this->m2 = m_2;
			this->m3 = m_3;

			this->m4 = m_4;
			this->m5 = m_5;
			this->m6 = m_6;
			this->m7 = m_7;

			this->m8 = m_8;
			this->m9 = m_9;
			this->m10 = m_10;
			this->m11 = m_11;

			this->m12 = m_12;
			this->m13 = m_13;
			this->m14 = m_14;
			this->m15 = m_15;
		}

		~Matrix4x4()
		{
		}
	public:
		union __declspec( align( 32 ) ) // row first
		{
			Vector4 r[4];
			__m256d r2[4];
			struct
			{
				double _11, _12, _13, _14;
				double _21, _22, _23, _24;
				double _31, _32, _33, _34;
				double _41, _42, _43, _44;
			};
			struct
			{
				double m0;
				double m1;
				double m2;
				double m3;

				double m4;
				double m5;
				double m6;
				double m7;

				double m8;
				double m9;
				double m10;
				double m11;

				double m12;
				double m13;
				double m14;
				double m15;
			};
			double m[4][4];
		};
	};

	void Matrix4x4Mul( const Matrix4x4& lhs, const Matrix4x4& rhs, Matrix4x4& result )
	{
		// Use vW to hold the original row
		Vector4 vW = lhs.r2[0];
		// Splat the component X,Y,Z then W
		Vector4 vX = SHUFFLE( vW, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		Vector4 vY = SHUFFLE( vW, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		Vector4 vZ = SHUFFLE( vW, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		vW = SHUFFLE( vW, _MM_SHUFFLE( 3, 3, 3, 3 ) );
		// Perform the operation on the first row
		vX = _mm256_mul_pd( vX, rhs.r2[0] );
		vY = _mm256_mul_pd( vY, rhs.r2[1] );
		vZ = _mm256_mul_pd( vZ, rhs.r2[2] );
		vW = _mm256_mul_pd( vW, rhs.r2[3] );
		// Perform a binary add to reduce cumulative errors
		vX = _mm256_add_pd( vX, vZ );
		vY = _mm256_add_pd( vY, vW );
		vX = _mm256_add_pd( vX, vY );
		result.r2[0] = vX;
		// Repeat for the other 3 rows
		vW = lhs.r2[1];
		vX = SHUFFLE( vW, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		vY = SHUFFLE( vW, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		vZ = SHUFFLE( vW, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		vW = SHUFFLE( vW, _MM_SHUFFLE( 3, 3, 3, 3 ) );
		vX = _mm256_mul_pd( vX, rhs.r2[0] );
		vY = _mm256_mul_pd( vY, rhs.r2[1] );
		vZ = _mm256_mul_pd( vZ, rhs.r2[2] );
		vW = _mm256_mul_pd( vW, rhs.r2[3] );
		vX = _mm256_add_pd( vX, vZ );
		vY = _mm256_add_pd( vY, vW );
		vX = _mm256_add_pd( vX, vY );
		result.r2[1] = vX;
		vW = lhs.r2[2];
		vX = SHUFFLE( vW, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		vY = SHUFFLE( vW, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		vZ = SHUFFLE( vW, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		vW = SHUFFLE( vW, _MM_SHUFFLE( 3, 3, 3, 3 ) );
		vX = _mm256_mul_pd( vX, rhs.r2[0] );
		vY = _mm256_mul_pd( vY, rhs.r2[1] );
		vZ = _mm256_mul_pd( vZ, rhs.r2[2] );
		vW = _mm256_mul_pd( vW, rhs.r2[3] );
		vX = _mm256_add_pd( vX, vZ );
		vY = _mm256_add_pd( vY, vW );
		vX = _mm256_add_pd( vX, vY );
		result.r2[2] = vX;
		vW = lhs.r2[3];
		vX = SHUFFLE( vW, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		vY = SHUFFLE( vW, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		vZ = SHUFFLE( vW, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		vW = SHUFFLE( vW, _MM_SHUFFLE( 3, 3, 3, 3 ) );
		vX = _mm256_mul_pd( vX, rhs.r2[0] );
		vY = _mm256_mul_pd( vY, rhs.r2[1] );
		vZ = _mm256_mul_pd( vZ, rhs.r2[2] );
		vW = _mm256_mul_pd( vW, rhs.r2[3] );
		vX = _mm256_add_pd( vX, vZ );
		vY = _mm256_add_pd( vY, vW );
		vX = _mm256_add_pd( vX, vY );
		result.r2[3] = vX;
	}

	void Matrix4x4Transpose(const Matrix4x4 &M, Matrix4x4 &result)
	{
		Matrix4x4 P;
		P.r2[0] = Vector4MergeXY(M.r2[0], M.r2[2]); // m00m20m01m21
		P.r2[1] = Vector4MergeXY(M.r2[1], M.r2[3]); // m10m30m11m31
		P.r2[2] = Vector4MergeZW(M.r2[0], M.r2[2]); // m02m22m03m23
		P.r2[3] = Vector4MergeZW(M.r2[1], M.r2[3]); // m12m32m13m33

		Matrix4x4 MT;
		MT.r2[0] = Vector4MergeXY(P.r2[0], P.r2[1]); // m00m10m20m30
		MT.r2[1] = Vector4MergeZW(P.r2[0], P.r2[1]); // m01m11m21m31
		MT.r2[2] = Vector4MergeXY(P.r2[2], P.r2[3]); // m02m12m22m32
		MT.r2[3] = Vector4MergeZW(P.r2[2], P.r2[3]); // m03m13m23m33

		result = MT;
	}

	

	// vec * mat ( w = 0 )
	void Matrix4x4VectorMulVec( const Vector4& lhs, const Matrix4x4& rhs, Vector4& result )
	{
		__m256d r1 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		__m256d r2 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		__m256d r3 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		r1 = _mm256_mul_pd( r1, rhs.r2[0] );
		r2 = _mm256_mul_pd( r2, rhs.r2[1] );
		r3 = _mm256_mul_pd( r3, rhs.r2[2] );
		__m256d tmp = _mm256_add_pd( r1, r2 );
		result = _mm256_add_pd( tmp, r3 );
	}

	// vec * mat ( w = 1 )
	void Matrix4x4VectorMulCoord( const Vector4& lhs, const Matrix4x4& rhs, Vector4& result )
	{
		__m256d r1 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		__m256d r2 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		__m256d r3 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		__m256d r4 = rhs.r2[3];
		r1 = _mm256_mul_pd( r1, rhs.r2[0] );
		r2 = _mm256_mul_pd( r2, rhs.r2[1] );
		r3 = _mm256_mul_pd( r3, rhs.r2[2] );
		__m256d tmp = _mm256_add_pd( r1, r2 );
		tmp = _mm256_add_pd( tmp, r3 );
		result = _mm256_add_pd( tmp, r4 );
	}

	// vec * mat
	void Matrix4x4VectorMul( const Vector4& lhs, const Matrix4x4& rhs, Vector4& result )
	{
		__m256d r1 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		__m256d r2 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		__m256d r3 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
		__m256d r4 = SHUFFLE( lhs.vec, _MM_SHUFFLE( 3, 3, 3, 3 ) );
		r1 = _mm256_mul_pd( r1, rhs.r2[0] );
		r2 = _mm256_mul_pd( r2, rhs.r2[1] );
		r3 = _mm256_mul_pd( r3, rhs.r2[2] );
		r4 = _mm256_mul_pd( r4, rhs.r2[3] );
		__m256d tmp = _mm256_add_pd( r1, r2 );
		tmp = _mm256_add_pd( tmp, r3 );
		result = _mm256_add_pd( tmp, r4 );
	}

	// mat * vec ( w = 0 )
	void Matrix4x4VectorMulVec(const Matrix4x4 &lhs, const Vector4 &rhs, Vector4 &result)
	{
		Matrix4x4 transposed;
		Matrix4x4Transpose(lhs, transposed);
		Matrix4x4VectorMulVec(rhs, transposed, result);
	}

	// mat * vec ( w = 1 )
	void Matrix4x4VectorMulCoord(const Matrix4x4 &lhs, const Vector4 &rhs, Vector4 &result)
	{

	}

	void Matrix4x4MulV2( const Matrix4x4& M1, const Matrix4x4& M2, Matrix4x4& result )
	{
		double x = M1.m[0][0];
		double y = M1.m[0][1];
		double z = M1.m[0][2];
		double w = M1.m[0][3];
		// Perform the operation on the first row
		result.m[0][0] = ( M2.m[0][0] * x ) + ( M2.m[1][0] * y ) + ( M2.m[2][0] * z ) + ( M2.m[3][0] * w );
		result.m[0][1] = ( M2.m[0][1] * x ) + ( M2.m[1][1] * y ) + ( M2.m[2][1] * z ) + ( M2.m[3][1] * w );
		result.m[0][2] = ( M2.m[0][2] * x ) + ( M2.m[1][2] * y ) + ( M2.m[2][2] * z ) + ( M2.m[3][2] * w );
		result.m[0][3] = ( M2.m[0][3] * x ) + ( M2.m[1][3] * y ) + ( M2.m[2][3] * z ) + ( M2.m[3][3] * w );
		// Repeat for all the other rows
		x = M1.m[1][0];
		y = M1.m[1][1];
		z = M1.m[1][2];
		w = M1.m[1][3];
		result.m[1][0] = ( M2.m[0][0] * x ) + ( M2.m[1][0] * y ) + ( M2.m[2][0] * z ) + ( M2.m[3][0] * w );
		result.m[1][1] = ( M2.m[0][1] * x ) + ( M2.m[1][1] * y ) + ( M2.m[2][1] * z ) + ( M2.m[3][1] * w );
		result.m[1][2] = ( M2.m[0][2] * x ) + ( M2.m[1][2] * y ) + ( M2.m[2][2] * z ) + ( M2.m[3][2] * w );
		result.m[1][3] = ( M2.m[0][3] * x ) + ( M2.m[1][3] * y ) + ( M2.m[2][3] * z ) + ( M2.m[3][3] * w );
		x = M1.m[2][0];
		y = M1.m[2][1];
		z = M1.m[2][2];
		w = M1.m[2][3];
		result.m[2][0] = ( M2.m[0][0] * x ) + ( M2.m[1][0] * y ) + ( M2.m[2][0] * z ) + ( M2.m[3][0] * w );
		result.m[2][1] = ( M2.m[0][1] * x ) + ( M2.m[1][1] * y ) + ( M2.m[2][1] * z ) + ( M2.m[3][1] * w );
		result.m[2][2] = ( M2.m[0][2] * x ) + ( M2.m[1][2] * y ) + ( M2.m[2][2] * z ) + ( M2.m[3][2] * w );
		result.m[2][3] = ( M2.m[0][3] * x ) + ( M2.m[1][3] * y ) + ( M2.m[2][3] * z ) + ( M2.m[3][3] * w );
		x = M1.m[3][0];
		y = M1.m[3][1];
		z = M1.m[3][2];
		w = M1.m[3][3];
		result.m[3][0] = ( M2.m[0][0] * x ) + ( M2.m[1][0] * y ) + ( M2.m[2][0] * z ) + ( M2.m[3][0] * w );
		result.m[3][1] = ( M2.m[0][1] * x ) + ( M2.m[1][1] * y ) + ( M2.m[2][1] * z ) + ( M2.m[3][1] * w );
		result.m[3][2] = ( M2.m[0][2] * x ) + ( M2.m[1][2] * y ) + ( M2.m[2][2] * z ) + ( M2.m[3][2] * w );
		result.m[3][3] = ( M2.m[0][3] * x ) + ( M2.m[1][3] * y ) + ( M2.m[2][3] * z ) + ( M2.m[3][3] * w );
	}

	void Matrix4x4Inverse(Matrix4x4 const &M, Matrix4x4 &mResult)
	{
		Matrix4x4 MT;
		Matrix4x4Transpose(M, MT);
		Vector4 V00 = _mm256_permute4x64_pd(MT.r2[2], _MM_SHUFFLE(1, 1, 0, 0));
		Vector4 V10 = _mm256_permute4x64_pd(MT.r2[3], _MM_SHUFFLE(3, 2, 3, 2));
		Vector4 V01 = _mm256_permute4x64_pd(MT.r2[0], _MM_SHUFFLE(1, 1, 0, 0));
		Vector4 V11 = _mm256_permute4x64_pd(MT.r2[1], _MM_SHUFFLE(3, 2, 3, 2));
		Vector4 V02 = Vector4Shuffle<2, 0, 2, 0>(MT.r2[2], MT.r2[0]);
		Vector4 V12 = Vector4Shuffle<3, 1, 3, 1>(MT.r2[3], MT.r2[1]);

		Vector4 D0 = _mm256_mul_pd(V00, V10);
		Vector4 D1 = _mm256_mul_pd(V01, V11);
		Vector4 D2 = _mm256_mul_pd(V02, V12);

		V00 = _mm256_permute4x64_pd(MT.r2[2], _MM_SHUFFLE(3, 2, 3, 2));
		V10 = _mm256_permute4x64_pd(MT.r2[3], _MM_SHUFFLE(1, 1, 0, 0));
		V01 = _mm256_permute4x64_pd(MT.r2[0], _MM_SHUFFLE(3, 2, 3, 2));
		V11 = _mm256_permute4x64_pd(MT.r2[1], _MM_SHUFFLE(1, 1, 0, 0));
		V02 = Vector4Shuffle<3, 1, 3, 1>(MT.r2[2], MT.r2[0]);
		V12 = Vector4Shuffle<2, 0, 2, 0>(MT.r2[3], MT.r2[1]);

		V00 = _mm256_mul_pd(V00, V10);
		V01 = _mm256_mul_pd(V01, V11);
		V02 = _mm256_mul_pd(V02, V12);
		D0 = _mm256_sub_pd(D0, V00);
		D1 = _mm256_sub_pd(D1, V01);
		D2 = _mm256_sub_pd(D2, V02);
		// V11 = D0Y,D0W,D2Y,D2Y
		V11 = Vector4Shuffle<1, 1, 3, 1>(D0, D2);
		V00 = _mm256_permute4x64_pd(MT.r2[1], _MM_SHUFFLE(1, 0, 2, 1));
		V10 = Vector4Shuffle<0, 3, 0, 2>(V11, D0);
		V01 = _mm256_permute4x64_pd(MT.r2[0], _MM_SHUFFLE(0, 1, 0, 2));
		V11 = Vector4Shuffle<2, 1, 2, 1>(V11, D0);
		// V13 = D1Y,D1W,D2W,D2W
		Vector4 V13 = Vector4Shuffle<3, 3, 3, 1>(D1, D2);
		V02 = _mm256_permute4x64_pd(MT.r2[3], _MM_SHUFFLE(1, 0, 2, 1));
		V12 = Vector4Shuffle<0, 3, 0, 2>(V13, D1);
		Vector4 V03 = _mm256_permute4x64_pd(MT.r2[2], _MM_SHUFFLE(0, 1, 0, 2));
		V13 = Vector4Shuffle<2, 1, 2, 1>(V13, D1);

		Vector4 C0 = _mm256_mul_pd(V00, V10);
		Vector4 C2 = _mm256_mul_pd(V01, V11);
		Vector4 C4 = _mm256_mul_pd(V02, V12);
		Vector4 C6 = _mm256_mul_pd(V03, V13);

		// V11 = D0X,D0Y,D2X,D2X
		V11 = Vector4Shuffle<0, 0, 1, 0>(D0, D2);
		V00 = _mm256_permute4x64_pd(MT.r2[1], _MM_SHUFFLE(2, 1, 3, 2));
		V10 = Vector4Shuffle<2, 1, 0, 3>(D0, V11);
		V01 = _mm256_permute4x64_pd(MT.r2[0], _MM_SHUFFLE(1, 3, 2, 3));
		V11 = Vector4Shuffle<0, 2, 1, 2>(D0, V11);
		// V13 = D1X,D1Y,D2Z,D2Z
		V13 = Vector4Shuffle<2, 2, 1, 0>(D1, D2);
		V02 = _mm256_permute4x64_pd(MT.r2[3], _MM_SHUFFLE(2, 1, 3, 2));
		V12 = Vector4Shuffle<2, 1, 0, 3>(D1, V13);
		V03 = _mm256_permute4x64_pd(MT.r2[2], _MM_SHUFFLE(1, 3, 2, 3));
		V13 = Vector4Shuffle<0, 2, 1, 2>(D1, V13);

		V00 = _mm256_mul_pd(V00, V10);
		V01 = _mm256_mul_pd(V01, V11);
		V02 = _mm256_mul_pd(V02, V12);
		V03 = _mm256_mul_pd(V03, V13);
		C0 = _mm256_sub_pd(C0, V00);
		C2 = _mm256_sub_pd(C2, V01);
		C4 = _mm256_sub_pd(C4, V02);
		C6 = _mm256_sub_pd(C6, V03);

		V00 = _mm256_permute4x64_pd(MT.r2[1], _MM_SHUFFLE(0, 3, 0, 3));
		// V10 = D0Z,D0Z,D2X,D2Y
		V10 = Vector4Shuffle<1, 0, 2, 2>(D0, D2);
		V10 = _mm256_permute4x64_pd(V10, _MM_SHUFFLE(0, 2, 3, 0));
		V01 = _mm256_permute4x64_pd(MT.r2[0], _MM_SHUFFLE(2, 0, 3, 1));
		// V11 = D0X,D0W,D2X,D2Y
		V11 = Vector4Shuffle<1, 0, 3, 0>(D0, D2);
		V11 = _mm256_permute4x64_pd(V11, _MM_SHUFFLE(2, 1, 0, 3));
		V02 = _mm256_permute4x64_pd(MT.r2[3], _MM_SHUFFLE(0, 3, 0, 3));
		// V12 = D1Z,D1Z,D2Z,D2W
		V12 = Vector4Shuffle<3, 2, 2, 2>(D1, D2);
		V12 = _mm256_permute4x64_pd(V12, _MM_SHUFFLE(0, 2, 3, 0));
		V03 = _mm256_permute4x64_pd(MT.r2[2], _MM_SHUFFLE(2, 0, 3, 1));
		// V13 = D1X,D1W,D2Z,D2W
		V13 = Vector4Shuffle<3, 2, 3, 0>(D1, D2);
		V13 = _mm256_permute4x64_pd(V13, _MM_SHUFFLE(2, 1, 0, 3));

		V00 = _mm256_mul_pd(V00, V10);
		V01 = _mm256_mul_pd(V01, V11);
		V02 = _mm256_mul_pd(V02, V12);
		V03 = _mm256_mul_pd(V03, V13);
		Vector4 C1 = _mm256_sub_pd(C0, V00);
		C0 = _mm256_add_pd(C0, V00);
		Vector4 C3 = _mm256_add_pd(C2, V01);
		C2 = _mm256_sub_pd(C2, V01);
		Vector4 C5 = _mm256_sub_pd(C4, V02);
		C4 = _mm256_add_pd(C4, V02);
		Vector4 C7 = _mm256_add_pd(C6, V03);
		C6 = _mm256_sub_pd(C6, V03);

		C0 = Vector4Shuffle<3, 1, 2, 0>(C0, C1);
		C2 = Vector4Shuffle<3, 1, 2, 0>(C2, C3);
		C4 = Vector4Shuffle<3, 1, 2, 0>(C4, C5);
		C6 = Vector4Shuffle<3, 1, 2, 0>(C6, C7);
		C0 = _mm256_permute4x64_pd(C0, _MM_SHUFFLE(3, 1, 2, 0));
		C2 = _mm256_permute4x64_pd(C2, _MM_SHUFFLE(3, 1, 2, 0));
		C4 = _mm256_permute4x64_pd(C4, _MM_SHUFFLE(3, 1, 2, 0));
		C6 = _mm256_permute4x64_pd(C6, _MM_SHUFFLE(3, 1, 2, 0));
		// Get the determinate
		Vector4 vTemp;
		Vector4Dot(C0, MT.r2[0], vTemp);
		vTemp = _mm256_div_pd(g_Vector4One.vec, vTemp);
		mResult.r2[0] = _mm256_mul_pd(C0, vTemp);
		mResult.r2[1] = _mm256_mul_pd(C2, vTemp);
		mResult.r2[2] = _mm256_mul_pd(C4, vTemp);
		mResult.r2[3] = _mm256_mul_pd(C6, vTemp);
	}
};

namespace Matrix4x4Func
{
	using namespace Math;
	void GetCoordTransformGivenZ( const Vector4& z, Matrix4x4& ans )
	{
		Vector4 x( 1, 0, 0 );
		Vector4 y( 0, 1, 0 );
		Vector4 tmp;
		Vector4Dot( x, z, tmp );
		if ( abs( abs( tmp.x ) - 1.0 ) < 1e-8 )
		{
			Vector4Cross( y, z, x );
			Vector4Normalize( x );
			Vector4Cross( x, z, y );
			Vector4Normalize( y );
		}
		else
		{
			Vector4Cross( x, z, y );
			Vector4Normalize( y );
			Vector4Cross( y, z, x );
			Vector4Normalize( x );
		}

		ans._11 = x.x; ans._12 = x.y; ans._13 = x.z; ans._14 = 0;
		ans._21 = y.x; ans._22 = y.y; ans._23 = y.z; ans._24 = 0;
		ans._31 = z.x; ans._32 = z.y; ans._33 = z.z; ans._34 = 0;
		ans._41 = 0; ans._42 = 0; ans._43 = 0; ans._44 = 1.0;
	}
};