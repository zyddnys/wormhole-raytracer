#pragma once

#include <algorithm>
#include "Vector4.h"
#include "Ray.h"

namespace Math
{
	class AABB
	{
	public:

		AABB()
		{
		}

		~AABB()
		{
		}
	public:
		AABB( const AABB& a, const AABB& b )
		{
			m_min.x = std::min( a.m_min.x, b.m_min.x );
			m_min.y = std::min( a.m_min.y, b.m_min.y );
			m_min.z = std::min( a.m_min.z, b.m_min.z );

			m_max.x = std::max( a.m_max.x, b.m_max.x );
			m_max.y = std::max( a.m_max.y, b.m_max.y );
			m_max.z = std::max( a.m_max.z, b.m_max.z );

			Vector4Add( m_max.vec, m_min.vec, m_center );
			Vector4Mul( m_center, 0.5, m_center );
		}
	public:
		static bool Collided( const AABB& a, const AABB& b ) // is collided
		{
			if ( a.m_max.x < b.m_min.x || a.m_min.x > b.m_max.x ) return false;
			if ( a.m_max.y < b.m_min.y || a.m_min.y > b.m_max.y ) return false;
			if ( a.m_max.z < b.m_min.z || a.m_min.z > b.m_min.z ) return false;

			return true;
		}
		bool Intersect( const Ray& ray ) const
		{
			double tmin = ( m_min.x - ray.origin.x ) / ray.dir.x;
			double tmax = ( m_max.x - ray.origin.x ) / ray.dir.x;

			if ( tmin > tmax ) std::swap(  tmin,  tmax );

			double tymin = ( m_min.y - ray.origin.y ) / ray.dir.y;
			double tymax = ( m_max.y - ray.origin.y ) / ray.dir.y;

			if ( tymin > tymax ) std::swap(  tymin,  tymax );

			if ( ( tmin > tymax ) || ( tymin > tmax ) )
				return false;

			if ( tymin > tmin )
				tmin = tymin;

			if ( tymax < tmax )
				tmax = tymax;

			double tzmin = ( m_min.z - ray.origin.z ) / ray.dir.z;
			double tzmax = ( m_max.z - ray.origin.z ) / ray.dir.z;

			if ( tzmin > tzmax ) std::swap(  tzmin,  tzmax );

			if ( ( tmin > tzmax ) || ( tzmin > tmax ) )
				return false;

			if ( tzmin > tmin )
				tmin = tzmin;

			if ( tzmax < tmax )
				tmax = tzmax;

			return true;
		}
	public:
		void SetMax( const Vector4& val )
		{
			m_max = val;
			Vector4Add( m_max.vec, m_min.vec, m_center );
			Vector4Mul( m_center, 0.5, m_center );
		}
		void SetMin( const Vector4& val )
		{
			m_min = val;
			Vector4Add( m_max.vec, m_min.vec, m_center );
			Vector4Mul( m_center, 0.5, m_center );
		}
		auto GetMax() const
		{
			return m_max;
		}
		auto GetMmin() const
		{
			return m_min;
		}


		void GetCenter( Vector4& ans ) const
		{
			ans = m_center;
		}
	protected:
		Vector4 m_center;
		Vector4 m_max, m_min;
	};

};