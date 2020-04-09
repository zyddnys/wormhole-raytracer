#pragma once

#include "Vector4.h"

namespace Math
{
	class Ray
	{
	public:
		void GetPointOnRay( double dis, Vector4& result ) const
		{
			Vector4Mul( dir, dis, result );
			Vector4Add( result, origin, result );
		}
	public:
		Ray( const Vector4& o, const Vector4& d ) :origin( o ), dir( d )
		{

		}
		Ray()
		{
		}
	public:
		Vector4 origin, dir;
	};
};
