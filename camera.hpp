#pragma once
#include "other_globals.hpp"

namespace impl
{
	glm::mat4x4 view_matrix( )
	{
		if ( !impl::camera_instance.load( ) )
			return {};

		return memory->read<glm::mat4x4>( impl::camera_instance.load( ) + 0x2e4 );
	}

	bool world_to_screen( const glm::vec3& world, glm::vec2* screen )
	{
		const auto matrix = glm::transpose( view_matrix( ) );

		const auto pos_vec = glm::vec3{ matrix[ 3 ][ 0 ], matrix[ 3 ][ 1 ], matrix[ 3 ][ 2 ] };

		const auto z = glm::dot( pos_vec, world ) + matrix[ 3 ][ 3 ];

		if ( z < 0.098f )
			return false;

		const auto x = glm::dot( glm::vec3{ matrix[ 0 ][ 0 ], matrix[ 0 ][ 1 ], matrix[ 0 ][ 2 ] }, world ) + matrix[ 0 ][ 3 ];
		const auto y = glm::dot( glm::vec3{ matrix[ 1 ][ 0 ], matrix[ 1 ][ 1 ], matrix[ 1 ][ 2 ] }, world ) + matrix[ 1 ][ 3 ];

		static const auto screen_center_x = impl::width * 0.5f;
		static const auto screen_center_y = impl::height * 0.5f;

		if ( screen )
		{
			*screen =
			{
				screen_center_x * ( 1.f + x / z ),
				screen_center_y * ( 1.f - y / z )
			};
		}

		return true;
	}
}