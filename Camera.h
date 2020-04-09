#pragma once

#include "Matrix4x4.h"

namespace Math
{
	class Camera
	{
	public:

		Camera()
		{
			m_lookDir = Vector4( 1, 0, 0 );
			m_up = Vector4( 0, 0, 1 );
			m_right = Vector4( 0, 1, 0 );

			m_fovX = Vector4( 65.0 * g_PI / 180.0 );
			m_fovY = m_fovX;
		}

		~Camera()
		{
		}

	public:
		void LookAt( double eye_x, double eye_y, double eye_z, double look_x, double look_y, double look_z )
		{
			m_position = Vector4( eye_x, eye_y, eye_z );
			Vector4 atPos( look_x, look_y, look_z );
			Vector4Sub( atPos, m_position, m_lookDir );
			Vector4Normalize( m_lookDir );
			Vector4Cross( m_up, m_lookDir, m_right );
			Vector4Normalize( m_right );
			Vector4Cross( m_lookDir, m_right, m_up );
		}
		void SetAspectFOV( double fov, int width, int height )
		{
			m_fovX = fov * g_PI / 180.0;
			Vector4Mul( m_fovX, ( double ) height / ( double ) width, m_fovY );
		}
	public:
		void GenerateRay( double x, double y, Vector4& pos, Vector4& dir )
		{
			Vector4 r, u, tmp;
			Vector4Mul( m_right, m_fovX, r );
			Vector4Mul( r, x * 0.5, r );
			Vector4Mul( m_up, m_fovY, u );
			Vector4Mul( u, y * 0.5, u );
			Vector4Add( r, u, tmp );
			Vector4Add( m_lookDir, tmp, tmp );
			Vector4Normalize( tmp, dir );
			pos = m_position;
		}
	private:
		Vector4 m_position, m_lookDir, m_up, m_right;
		Vector4 m_fovX, m_fovY;
	};

};
