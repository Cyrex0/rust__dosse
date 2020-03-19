#pragma once
#include "other_globals.hpp"
#include <stdio.h>
#include <emmintrin.h>	

enum BoneList : int
{
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,
	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement
};

namespace impl
{

	std::uint64_t get_base_player( std::uint64_t entity )
	{
		const auto unk1 = memory->read( entity + 0x18 );

		if ( !unk1 )
			return 0;
		return memory->read( unk1 + 0x28 );
	}

	bool is_local_player(std::uint64_t entity)
	{
		const auto base_player = get_base_player(entity);

		if (!base_player)
			return false;

		const auto player_model = memory->read(base_player + 0x520);

		if (!player_model)
			return false;

		return memory->read<bool>(player_model + 0x209);
	}

	glm::vec3 midPoint(glm::vec3 v1,glm::vec3 v2)
	{
		return glm::vec3((v1.x + v2.x) / 2, (v1.y + v2.y) / 2, (v1.z + v2.z) / 2);
	}

	void safe_memcpy_wrapper(ULONG64 Dst, ULONG64 Src, ULONG Sz)
	{
		if ((((ULONG64)Dst) > 0x400000) && (((ULONG64)Dst + Sz) < 0x00007FFFFFFF0000))
		{
			while (true)
			{
				//copy 8 byte
				if (Sz >= 8)
				{
					*(ULONG64*)Dst = impl::memory->read<ULONG64>(Src);
					Dst += 8; Src += 8; Sz -= 8;
				}

				//copy 4 byte
				else if (Sz >= 4)
				{
					*(ULONG*)Dst = impl::memory->read<ULONG>(Src);
					Dst += 4; Src += 4; Sz -= 4;
				}

				//copy 2 byte
				else if (Sz >= 2)
				{
					*(WORD*)Dst = impl::memory->read<WORD>(Src);
					Dst += 2; Src += 2; Sz -= 2;
				}

				//copy last byte
				else if (Sz)
				{
					*(BYTE*)Dst = impl::memory->read<BYTE>(Src);
					break;
				}

				//if(Sz == 0)
				else
					break;
			}
		}
	}

	#define safe_memcpy(Dst, Src, Size) safe_memcpy_wrapper(((ULONG64)Dst), ((ULONG64)Src), Size);

	glm::vec3 GetPosition(DWORD64 transform)
	{
		if (!transform) return glm::vec3{ 0.f, 0.f, 0.f };

		struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		int Index = impl::memory->read<int>(transform + 0x40);// *(PINT)(transform + 0x40);
		DWORD64 pTransformData = impl::memory->read<DWORD64>(transform + 0x38);
		DWORD64 transformData[2];
		//impl::memory->read(&transformData, (PVOID)(pTransformData + 0x18), 16);
		safe_memcpy(&transformData, (PVOID)(pTransformData + 0x18), 16);

		size_t sizeMatriciesBuf = 48 * Index + 48;
		size_t sizeIndicesBuf = 4 * Index + 4;

		PVOID pMatriciesBuf = malloc(sizeMatriciesBuf);
		PVOID pIndicesBuf = malloc(sizeIndicesBuf);

		if (pMatriciesBuf && pIndicesBuf)
		{
			// Read Matricies array into the buffer
			impl::memory->read(transformData[0], pMatriciesBuf, sizeMatriciesBuf);
			// Read Indices array into the buffer
			impl::memory->read(transformData[1], pIndicesBuf, sizeIndicesBuf);

			__m128 result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * Index);
			int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * Index);

			while (transformIndex >= 0)
			{
 				Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);
				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

				result = _mm_add_ps(
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
							tmp7)), *(__m128*)(&matrix34.vec0));
				try {
					transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
				}
				catch (...)
				{
					// Do nothing
				}
			}

			return glm::vec3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
		}
	}

	glm::vec3 get_bone_position(std::uint64_t entity,int id)
	{
		const auto base_player = get_base_player(entity);
		if (!base_player)
			return glm::vec3{ 0,0,0 };

		uintptr_t model = impl::memory->read<uintptr_t>(base_player + 0xB0);
		uintptr_t BoneTransforms = impl::memory->read<uintptr_t>(model + 0x48);
		uintptr_t entity_bone = impl::memory->read<uintptr_t>(BoneTransforms + (0x20 + (id * 0x8)));

		return GetPosition(impl::memory->read<DWORD64>(entity_bone + 0x10));
	}

	bool get_player_sleeping( std::uint64_t entity )
	{
		const auto base_player = get_base_player( entity );

		if ( !base_player )
			return 0;

		int flags = memory->read<int>(base_player + 0x480);

		return flags & 16;
	}

	std::uint32_t get_player_health( std::uint64_t entity )
	{
		const auto base_player = get_base_player( entity );

		if ( !base_player )
			return 0;

		const auto player_health = memory->read<float>( base_player + 0x1f4 );

		if ( player_health <= 0.8f )
			return 0;

		return std::lround( player_health );
	}

	std::wstring get_player_name(std::uint64_t entity)
	{
		const auto base_player = get_base_player(entity);

		if (!base_player)
			return {};

		const auto player_name_ptr = memory->read(base_player + 0x4e8);

		if (!player_name_ptr)
			return {};

		return memory->read_unicode(player_name_ptr + 0x14, 32);
	}

	std::uint64_t get_held_item(std::uint64_t entity)
	{
		const auto base_player = get_base_player(entity);

		if (!base_player)
			return 0;

		const auto active_item_id = memory->read<std::uint32_t>(base_player + 0x5e0);

		/* not holding anything */
		if (!active_item_id)
			return 0;

		/* now iterate through the player's belt */

		const auto player_inventory = memory->read(base_player + 0x490);

		if (!player_inventory)
			return 0;

		const auto inventory_belt = memory->read(player_inventory + 0x28);

		if (!inventory_belt)
			return 0;

		const auto belt_contents_list = memory->read(inventory_belt + 0x38);

		if (!belt_contents_list)
			return 0;

		const auto belt_contents_sz = memory->read<std::uint32_t>(belt_contents_list + 0x14);
		const auto belt_contents = memory->read(belt_contents_list + 0x10);

		if (!belt_contents)
			return 0;

		for (auto i = 0u; i < belt_contents_sz; i++)
		{
			const auto current_item = memory->read(belt_contents + (0x20 + (i * 8)));

			if (!current_item)
				continue;

			if (active_item_id == memory->read<std::uint32_t>(current_item + 0x20))
				return current_item;
		}

		return 0;
	}

	std::wstring get_player_item_name(std::uint64_t entity)
	{
		const auto held_item = get_held_item(entity);

		if (!held_item)
			return {};

		const auto item_definition = memory->read(held_item + 0x18);

		if (!item_definition)
			return {};

		const auto display_phrase = memory->read(item_definition + 0x28);

		if (!display_phrase)
			return {};

		const auto english_name = memory->read(display_phrase + 0x18);

		if (!english_name)
			return {};

		return memory->read_unicode(english_name + 0x14, 64);
	}

	std::wstring get_item_name(std::uint64_t item)
	{
		const auto item_definition = memory->read(item + 0x18);

		if (!item_definition)
			return {};

		const auto display_phrase = memory->read(item_definition + 0x28);

		if (!display_phrase)
			return {};

		const auto english_name = memory->read(display_phrase + 0x18);

		if (!english_name)
			return {};

		return memory->read_unicode(english_name + 0x14, 64);
	}

	glm::vec3 get_obj_pos(std::uint64_t entity)
	{
		const auto player_visual = memory->read(entity + 0x8);

		if (!player_visual)
			return {};

		const auto visual_state = memory->read(player_visual + 0x38);

		if (!visual_state)
			return {};

		return memory->read<glm::vec3>(visual_state + 0xb0);
	}

	bool is_same_team(std::uint64_t entity, std::uint64_t local_entity)
	{
		const auto base_player = get_base_player(entity);

		if (!base_player)
			return false;

		const auto base_player_local = get_base_player(local_entity);

		if (!base_player_local)
			return false;

		const auto local_team = memory->read<std::uint32_t>(base_player_local + 0x5a8);
		const auto enemy_team = memory->read<std::uint32_t>(base_player + 0x5a8);

		if (local_team == 0 || enemy_team == 0)
			return false;

		return local_team == enemy_team;
	}

	glm::vec3 get_player_pos( std::uint64_t entity )
	{
		return impl::midPoint(impl::get_bone_position(entity, l_foot), impl::get_bone_position(entity, r_foot));
	}

	inline std::array weapon_names
	{
		L"Rifle",
		L"MP5A4",
		L"SMG",
		L"Revolver",
		L"Pistol",
		L"Shotgun",
		L"M249",
		L"Eoka",
		L"Thompson",
		L"Spas-12 Shotgun",

	};

	void disable_deviations(std::uint64_t entity, bool noRecoil, bool alltimeEoka, bool instantCharge, bool noMovePenatly, bool forceAutomatic, bool bulletThickness, bool doExtendMeele)
	{
		if (!noRecoil && !alltimeEoka && !instantCharge && !noMovePenatly && !forceAutomatic && !bulletThickness && !doExtendMeele) {
			return;
		}
		const auto current_item = get_held_item(entity);

		if (!current_item)
			return;

		const auto held_item_name = get_item_name(current_item);

		auto is_weapon = false;

		for (const auto& element : weapon_names)
		{
			if (std::wcsstr(held_item_name.c_str(), element) != nullptr)
			{
				is_weapon = true;
				break;
			}
		}

		


		const auto base_projectile = memory->read(current_item + 0x90);

		if (!base_projectile)
			return;

		if (bulletThickness || noRecoil && (is_weapon || held_item_name.find(L"Bow") != std::string::npos)) {
			const auto projectile_list = impl::memory->read(base_projectile + 0x338);

			if (!projectile_list)
				return;

			const auto projectile_array = impl::memory->read(projectile_list + 0x10);

			if (!projectile_array)
				return;

			const auto projectile_list_size = impl::memory->read<int>(get_base_player(entity) + 0x618);
			try {
				for (auto i = 0u; i < projectile_list_size; i++)
				{
				
						const auto current_projectile = impl::memory->read(projectile_array + (0x20 + (i * 8)));

						if (!current_projectile)
							continue;

						if (bulletThickness) {
							const auto old_thickness = memory->read<float>(current_projectile + 0x2C);


							if (old_thickness <= 0.f || old_thickness >= 1.f)
								continue;

							printf("[Bullet Thickness] Changed %f.f to 1.f \n", old_thickness);
							memory->write<float>(current_projectile + 0x2C, 1.f);

						}

						if (noRecoil) {

							const auto mod_projectile = memory->read(current_projectile + 0xE8); // public ItemModProjectile mod; // 0xE8

							if (!mod_projectile)
								continue;
							memory->write<float>(mod_projectile + 0x30, 0.f); // public float projectileSpread; // 0x30
							memory->write<float>(mod_projectile + 0x38, 0.f); // public float projectileVelocitySpread; // 0x38

						}




						th::sleep_for(ch::milliseconds(3));
				
				}
			}
			catch (...) {}
		}

		if (forceAutomatic && is_weapon) {
			memory->write<bool>(base_projectile + 0x270, true); // public bool automatic; // 0x270
		}
		if (noRecoil && is_weapon) {
			/* + 0x2d0 spread, + 0x2d4 spread hip */

			memory->write<float>(base_projectile + 0x2d0, 0.f);
			memory->write<float>(base_projectile + 0x2d4, 0.f);

			const auto recoil_properties = memory->read(base_projectile + 0x2c0);

			if (!recoil_properties)
				return;

			/* + 0x1C yaw max, + 0x24 pitch max */

			memory->write<float>(recoil_properties + 0x1C, 0.f); //	public float recoilYawMax; // 0x1C
			memory->write<float>(recoil_properties + 0x24, 0.f); // public float recoilPitchMax; // 0x24	
			memory->write<float>(recoil_properties + 0x18, 0.f); // 
			memory->write<float>(recoil_properties + 0x20, 0.f); // 
		}



		if (alltimeEoka) {
			if (std::wcsstr(held_item_name.c_str(), L"Eoka") != nullptr)
			{
				memory->write<float>(base_projectile + 0x340, 1.0f);
			}
		}
		if (instantCharge) {
			memory->write<float>(base_projectile + 0x3A0, 1.f); // private float currentHoldProgress; // 0x3A0
		}
		if (noMovePenatly) {
			memory->write<float>(base_projectile + 0x398, 0.f); // protected float movementPenalty; // 0x398
		}		

		if (!is_weapon && doExtendMeele) {
			memory->write<float>(base_projectile + 0x270, 3.f); // public float maxDistance; // 0x270
		}

	}


	void do_admin_flag(std::uint64_t entity, bool is_enabled = true)
	{
		const auto base_player = get_base_player(entity);

		if (!base_player)
			return;

		auto player_flags = memory->read(base_player + 0x480);

		if (is_enabled)
			player_flags |= 4;
		else
			player_flags &= ~4;

		memory->write<std::uint64_t>(base_player + 0x480, player_flags);
	}

	void do_spiderman(std::uint64_t entity) {

		const auto base_player = get_base_player(entity);
		if (!base_player)
			return;

		const auto base_movement = memory->read(base_player + 0x4B0); // public BaseMovement movement; // 0x4B0
		if (!base_movement)
			return;
		
		memory->write<float>(base_movement + 0xAC, 0.f); // private float groundAngle; // 0xAC
		memory->write<float>(base_movement + 0xB0, 0.f); // private float groundAngleNew; // 0xB0
	}

	

}