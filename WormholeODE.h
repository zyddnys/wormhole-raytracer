#pragma once

#include "vec.h"
#include "FPMap.h"

vec5d geodesic_odes(
	double t,			// step t
	vec5d v,			// 5 variables
	double const a,		// wormhole length
	double const M,		// wormhole mass
	double const rho,	// wormhole radius
	double const B_sqr,	// B^2
	double const b		// b or p_phi
)
{
	double const &l(v[0]);
	double const &theta(v[1]);
	double const &phi(v[2]);
	double const &pl(v[3]);
	double const &ptheta(v[4]);

	std::uint64_t const outside_wormhole(std::abs(l) > a);

	double const x(2.0 * (std::abs(l) - a) / (g_pi * M));
	double const sin_theta(std::sin(theta)), cos_theta(std::cos(theta));
	double const b_sqr(b * b);

	// r (5)
	double const r(rho + outside_wormhole * (M * (x * std::atan(x) - 0.5 * std::log(1.0 + x * x))));
	// dr/dl
	double const dr_dl(outside_wormhole * (std::atan(x) * 2.0 * l / (g_pi * std::abs(l))));

	// dl/dt (A.7a)
	double const dl_dt(pl);
	// dθ/dt (A.7b)
	double const dtheta_dt(ptheta / (r * r));
	// dφ/dt (A.7c)
	double const dphi_dt(b / (r * r * sin_theta * sin_theta));
	// dpl/dt (A.7d)
	double const dpl_dt(B_sqr * dr_dl / (r * r * r));
	// dpθ/dt (A.7e)
	double const dptheta_dt(b_sqr * cos_theta / (sin_theta * sin_theta * sin_theta * r * r));

	return vec5d{ dl_dt, dtheta_dt, dphi_dt, dpl_dt, dptheta_dt };
}

vec3d geodesic_odes_2d(
	double t,			// step t
	vec3d v,			// 3 variables
	double const a,		// wormhole length
	double const M,		// wormhole mass
	double const rho,	// wormhole radius
	double const B_sqr,	// B^2
	double const b		// b or p_phi
)
{
	double const &l(v[0]);
	double const &phi(v[1]);
	double const &pl(v[2]);

	std::uint64_t const outside_wormhole(std::abs(l) > a);

	double const x(2.0 * (std::abs(l) - a) / (g_pi * M));
	double const b_sqr(b * b);

	// r (5)
	double const r(rho + outside_wormhole * (M * (x * std::atan(x) - 0.5 * std::log(1.0 + x * x))));
	// dr/dl
	double const dr_dl(outside_wormhole * (std::atan(x) * 2.0 * l / (g_pi * std::abs(l))));

	// dl/dt (A.7a)
	double const dl_dt(pl);
	// dφ/dt (A.7c)
	double const dphi_dt(b / (r * r));
	// dpl/dt (A.7d)
	double const dpl_dt(B_sqr * dr_dl / (r * r * r));

	return vec3d{ dl_dt, dphi_dt, dpl_dt };
}

std::tuple<double, double, double> trace_wormhole_geodesic(
	double h,			// step size
	std::uint64_t steps,// steps
	double a,			// wormhole length
	double M,			// wormhole mass
	double rho,			// wormhole radius
	double B_sqr,		// B^2
	double b,			// b or p_phi
	double l0,			// init l
	double theta0,		// init theta
	double phi0,		// init phi
	double p_l0,		// init p_l
	double p_theta0		// init p_theta
)
{
	vec5d y{ l0,theta0,phi0,p_l0,p_theta0 };
	double x = 0;

	for (std::uint64_t i{}; i != steps; ++i)
	{
		y = runge_kutta_4_step(x, y, h, geodesic_odes, a, M, rho, B_sqr, b);
		x += h;
	}

	double const &l(y[0]);
	double const &theta(y[1]);
	double const &phi(y[2]);
	double const &pl(y[3]);
	double const &ptheta(y[4]);

	return { theta, phi, l };
}

std::tuple<double, double> trace_wormhole_geodesic_2d(
	double h,			// step size
	std::uint64_t steps,// steps
	double a,			// wormhole length
	double M,			// wormhole mass
	double rho,			// wormhole radius
	double B_sqr,		// B^2
	double b,			// b or p_phi
	double l0,			// init l
	double phi0,		// init phi
	double p_l0			// init p_l
)
{
	vec3d y{ l0, phi0, p_l0 };
	double x = 0;

	for (std::uint64_t i{}; i != steps; ++i)
	{
		y = runge_kutta_4_step(x, y, h, geodesic_odes_2d, a, M, rho, B_sqr, b);
		x += h;
	}

	double const &l(y[0]);
	double const &phi(y[1]);
	double const &pl(y[2]);

	return { phi, l };
}

std::tuple<double, double, double, double> trace(Math::Vector4 const &ray_origin, Math::Vector4 const &ray_dir)
{
	Math::Vector4 ray_dir_cs;
	ray_dir_cs.x = -ray_dir.x;
	ray_dir_cs.y = ray_dir.y;
	ray_dir_cs.z = ray_dir.z;

	double phi_cs = std::atan2(ray_dir_cs.y, ray_dir_cs.x);
	double theta_cs = std::acos(ray_dir_cs.z);

	Math::Vector4 tmp;
	Math::Vector4Length(ray_origin, tmp);
	double l_c = tmp.x;
	double phi_c = std::atan2(ray_origin.y, ray_origin.x);
	double theta_c = std::acos(ray_origin.z / l_c);

	double n_l = -ray_dir_cs.x;
	double n_phi = -ray_dir_cs.y;
	double n_theta = ray_dir_cs.z;

	double p_l = n_l;
	double p_theta = l_c * n_theta;
	double p_phi = l_c * std::sin(theta_c) * n_phi;

	double b = p_phi;
	double B_sqr = l_c * l_c * (n_theta * n_theta + n_phi * n_phi);

	auto [theta, phi, l] = trace_wormhole_geodesic(
		0.1,
		100,
		0, // length
		0.1, // mass
		0.2, // radius
		B_sqr,
		b,
		l_c,
		theta_c,
		phi_c,
		p_l,
		p_theta
	);

	double local_x = std::sin(theta) * std::cos(phi);
	double local_y = std::sin(theta) * std::sin(phi);
	double local_z = std::cos(theta);

	//return{ ray_dir.x,ray_dir.y,ray_dir.z,l };
	return { local_x,local_y,local_z,l };
}

FPMap<std::pair<double, double>> g_phi_cache(65536);

std::pair<double, double> equatorial_phi_mapping(double phi, double r, double wh_length, double wh_mass, double wh_radius)
{
	phi = std::fmod(phi, g_2pi);
	auto cached_result(g_phi_cache.Get(phi / g_2pi));
	if (cached_result)
		return cached_result.value();

	double l_c = r;

	double n_l = -std::cos(phi);
	double n_phi = -std::sin(phi);

	double p_l = n_l;
	double p_phi = l_c * n_phi;

	double b = p_phi;
	double B_sqr = l_c * l_c * (n_phi * n_phi);

	auto [phi_traced, l] = trace_wormhole_geodesic_2d(
		0.01,	// step size
		10000,	// total steps
		wh_length, // length
		wh_mass, // mass
		wh_radius, // radius
		B_sqr,
		b,
		l_c,
		0,
		p_l
	);

	phi_traced = std::fmod(phi_traced, 2.0 * g_pi);

	g_phi_cache.Put(phi / g_2pi, { phi_traced, l });

	return { phi_traced, l }; // phi, l
}

