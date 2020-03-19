#pragma once
#include <mutex>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include "mem_class.hpp"


namespace th = std::this_thread;
namespace ch = std::chrono;

namespace impl
{
	std::unique_ptr<memory_mgr> memory = nullptr;



	// ESP
	static bool showPlayers = true;
	static bool skeleton = false;
	static bool boxESP = false;
	static bool showScientists = false;
	static bool showStashes = false;
	// Collectable ESP
	static bool showHemp = false;
	static bool showMushrooms = false;
	static bool showPumpkins = false;
	static bool showCorn = false;
	// Ores
	static bool showWoodC = false;
	static bool showStoneC = false;
	static bool showMetalC = false;
	static bool showSulfurC = false;
	/* non collectable ores */
	static bool showStone = false;
	static bool showMetal = false;
	static bool showSulfur = false;
	// Crate ESP
	static bool showToolsCrate = false;
	static bool showNormalCrate = false;
	static bool showMilitaryCrate = false;
	static bool showEliteCrate = false;
	// Special Crate(s) ESP
	static bool showBarrelCrate = false;
	static bool showCodelockCrate = false;
	static bool showAirdrop = false;
	static bool showHeliCrate = false;
	static bool showBradleyCrate = false;

	// Weapons
	static bool doAntiRecoil = false;
	static bool bulletThickness = false;
	static bool alltimeEoka = false;
	static bool doInstantCharge = false;
	static bool doRemoveMovePenalty = false;
	static bool forceAutomatic = false;

	// Others
	static bool doAdminFlag = false;
	static bool doSpiderman = false;
	static bool extendMeeleRange = false;

	static std::uint64_t local_player = 0;

	// Players
	std::mutex player_list_mutex{};
	std::vector<uint64_t> player_list{};

	// Scientists
	std::mutex scientists_list_mutex{};
	std::vector<uint64_t> scientists_list{};

	// Stashes
	std::mutex stash_list_mutex{};
	std::vector<uint64_t> stash_list{};

	// Collectables
	std::mutex collectible_list_mutex{};
	std::vector<uint64_t> collectible_list{};

	// Ores
	std::mutex ore_list_mutex{};
	std::vector<uint64_t> ore_list{};

	// Boxes
	std::mutex crate_list_mutex{};
	std::vector<uint64_t> crate_list{};

	std::atomic<uint64_t> camera_instance{};
	std::atomic<bool> thread_ejection{};

	std::atomic<bool> render_ejected{};
	std::atomic<bool> iter_ejected{};

	float width = 0.f;
	float height = 0.f;

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
}