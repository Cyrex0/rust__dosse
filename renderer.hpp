#pragma once
#include "other_globals.hpp"
#include "dx_renderer.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "dx_renderer.hpp"
#include <vector>
namespace impl
{

	std::unique_ptr<dx_renderer> renderer;

	namespace utility
	{
		static std::array<bool, 256u> previous_key_state{ };
		static std::array<bool, 256u> current_key_state{ };

		inline bool is_key_pressed(const std::uint32_t key)
		{
			return current_key_state.at(key) && !previous_key_state.at(key);
		}

		inline bool is_key_held(const std::uint32_t key)
		{
			return current_key_state.at(key) && previous_key_state.at(key);
		}
	}

	namespace defs
	{
		constexpr auto grey_color = glm::uvec3(200u, 200u, 200u);
		constexpr auto green_color = glm::uvec3{ 147u, 201u, 56u };
		constexpr auto pink_color = glm::uvec3{ 249u, 38u, 114u };
		constexpr auto red_color = glm::uvec3{ 212u, 32u, 52u };
		constexpr auto blue_color = glm::uvec3(52u, 152u, 235u);
		enum item_type_t : std::uint32_t
		{
			boolean = 0,
			integer_slider,
			float_slider
		};

		static auto sCounter = 0u;
	}

	struct item_t
	{
	public:
		std::wstring name;
		std::uint32_t index;
		std::uint32_t type;
		void* out_value;
		std::size_t size;

		item_t(const std::wstring& name_, void* out_, const std::size_t size_, const std::uint32_t type_)
		{
			if (!out_)
				return;

			name = name_;
			out_value = out_;
			type = type_;
			size = size_;
		}

		template <typename T> T get() const
		{
			T buffer{};
			std::memcpy(reinterpret_cast<void*>(&buffer), out_value, size);
			return buffer;
		}

		template <typename T> void set(const T& value)
		{
			std::memcpy(out_value, reinterpret_cast<const void*>(const_cast<T*>(&value)), size);
		}
	};

	struct section_t
	{
		std::wstring name;
		std::uint32_t index;
		std::vector<item_t> items{};
		std::uint32_t counter = 0;

		section_t(const std::wstring& name_, std::vector<item_t> items_ = {})
		{
			if (items_.size() <= 0)
				return;

			name = name_;
			index = defs::sCounter;
			items = items_;
			defs::sCounter++;
		}
	};


	std::vector<impl::item_t> PlayerESP{
						impl::item_t(L"Enabled", &impl::showPlayers, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Skeletons", &impl::skeleton, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"2D Box", &impl::boxESP, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> AiESP{
						impl::item_t(L"Scientists", &impl::showScientists, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> CollectibleESP{
						impl::item_t(L"Hemp", &impl::showHemp, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Mushroom", &impl::showMushrooms, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Pumpkin", &impl::showPumpkins, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Corn", &impl::showCorn, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> PickupESP{
						impl::item_t(L"Wood Pickup ESP", &impl::showWoodC, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Stone Pickup ESP", &impl::showStoneC, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Metal Pickup ESP", &impl::showMetalC, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Sulfur Pickup ESP", &impl::showSulfurC, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> OreESP{
						impl::item_t(L"Stone Ore ESP", &impl::showStone, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Metal Ore ESP", &impl::showMetal, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Sulfur Ore ESP", &impl::showSulfur, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> CrateESP{
						impl::item_t(L"Stash ESP", &impl::showStashes, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Tools Crate ESP", &impl::showToolsCrate, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Normal Crate ESP", &impl::showNormalCrate, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Military Crate ESP", &impl::showMilitaryCrate, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Elite Crate ESP", &impl::showEliteCrate, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Barrel ESP", &impl::showBarrelCrate, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> Combat{
						impl::item_t(L"No Recoil/Spread", &impl::doAntiRecoil, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Bullet Thickness", &impl::bulletThickness, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Alltime Eoka", &impl::alltimeEoka, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Instant Charge", &impl::doInstantCharge, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Block Move Penalty", &impl::doRemoveMovePenalty, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Force Automatic", &impl::forceAutomatic, sizeof(bool), impl::defs::boolean),
	};
	std::vector<impl::item_t> Others{
						impl::item_t(L"Admin Flag", &impl::doAdminFlag, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Spiderman", &impl::doSpiderman, sizeof(bool), impl::defs::boolean),
						impl::item_t(L"Extended Meele", &impl::extendMeeleRange, sizeof(bool), impl::defs::boolean),

	};

	std::vector<impl::section_t> MenuSections{

						  impl::section_t(L"Combat", Combat),
						  impl::section_t(L"Player ESP", PlayerESP),
						  impl::section_t(L"AI ESP", AiESP),
						  impl::section_t(L"Collectable ESP", CollectibleESP),						  
						  impl::section_t(L"Ore ESP", OreESP),
						  impl::section_t(L"Pickup ESP", PickupESP),
						  impl::section_t(L"Crate ESP", CrateESP),
						  impl::section_t(L"Others", Others),
	};


	class menu_t
	{
	private:
		std::vector<section_t> sections{};
	public:
		bool is_open = false;
		bool is_locked = false;

		std::uint32_t selected_item = 0u;
		std::uint32_t selected_section = 0u;

		menu_t() { }
	public:
		void push_sections(const std::vector<section_t>& sections_)
		{
			sections = sections_;
			for (auto section : sections)
			{
				int count = 0;
				for (auto item : section.items)
				{
					sections.at(section.index).items.at(count).index = count;
					count++;
				}
			}
		}

		void poll_keyboard()
		{
			static const auto game_window = FindWindowW(L"UnityWndClass", nullptr);

			if (GetForegroundWindow() != game_window)
				return;

			std::copy(utility::current_key_state.cbegin(), utility::current_key_state.cend(), utility::previous_key_state.begin());

			for (auto i = 0u; i < 256u; i++)
				utility::current_key_state.at(i) = GetAsyncKeyState(i);

			if (utility::is_key_pressed(VK_INSERT))
				is_open = !is_open;

			auto last_section = static_cast<std::uint32_t>(sections.size() - 1u);

			if (utility::is_key_pressed(VK_DOWN) && !is_locked)
			{
				selected_section++;

				if (selected_section > last_section)
					selected_section = 0u;
			}
			else if (utility::is_key_pressed(VK_UP) && !is_locked)
				selected_section--;
			else if (utility::is_key_pressed(VK_RIGHT) && !is_locked)
			{
				is_locked = true;
				selected_section = std::clamp(selected_section, 0u, last_section);
				return;
			}

			selected_section = std::clamp(selected_section, 0u, last_section);

			if (is_locked)
			{
				auto last_item = (uint32_t)(sections.at(selected_section).items.size() - 1u);
				if (utility::is_key_pressed(VK_DOWN))
				{
					selected_item++;

					if (selected_item > last_item)
						selected_item = 0u;
				}

				else if (utility::is_key_pressed(VK_UP))
					selected_item--;

				selected_item = std::clamp(selected_item, 0u, last_item);

				if (utility::is_key_pressed(VK_LEFT))
				{
					is_locked = false;
					selected_item = 0u;
				}

				auto item_data = sections.at(selected_section).items.at(selected_item);
				switch (item_data.type)
				{
				case defs::boolean:
				{
					if (!is_open) {
						return;
					}

					if (utility::is_key_pressed(VK_RETURN))
					{
						item_data.set(!item_data.get<bool>());
					}
					break;
				}

				case defs::float_slider:
				{
					if (!is_open) {
						return;
					}
					if (utility::is_key_pressed(VK_ADD))
						item_data.set(item_data.get<float>() + 0.5f);
					else if (utility::is_key_pressed(VK_SUBTRACT))
						item_data.set(item_data.get<float>() - 0.5f);

					break;
				}
				case defs::integer_slider:
				{
					if (!is_open) {
						return;
					}
					if (utility::is_key_pressed(VK_ADD))
						item_data.set(item_data.get<int>() + 1);
					else if (utility::is_key_pressed(VK_SUBTRACT))
						item_data.set(item_data.get<int>() - 1);

					break;
				}
				}
			}
		}



		void renderMenu() const
		{
			if (!is_open || sections.empty())
				return;

			for (auto section : sections)
			{
				const auto section_position = (35.f + (15.f * section.index)) + 10.f;

				const auto is_selected_section = section.index == this->selected_section;

				renderer->draw_text(
					{ 20.f,  section_position },
					is_selected_section ? defs::grey_color : defs::pink_color,
					11.f,
					section.name.c_str()
				);

				if (is_selected_section)
				{
					for (auto item : section.items)
					{
						auto item_position = (35.f + (15.f * item.index)) + 10.f + (15.f * section.index);

						auto is_selected_item = item.index == this->selected_item;

						renderer->draw_text(
							{ 100.f,item_position },
							is_selected_item ? defs::grey_color : defs::pink_color,
							11.f,
							item.name.c_str()
						);

						switch (item.type)
						{
						case defs::boolean:
						{
							const auto item_state = item.get<bool>();

							auto item_state_color = item_state ? defs::green_color : defs::red_color;

							if (is_selected_item)
								item_state_color = defs::grey_color;

							renderer->draw_text(
								{ 200.f, item_position },
								is_selected_item ? defs::grey_color : defs::pink_color,
								11.f,
								item_state ? (L"✔") : (L"❌")
							);

							break;
						}

						case defs::float_slider:
						{
							renderer->draw_text(
								{ 200.f, item_position },
								is_selected_item ? defs::grey_color : defs::pink_color,
								11.f,
								L"%.2f", item.get<float>()
							);

							break;
						}
						case defs::integer_slider:
						{
							renderer->draw_text(
								{ 200.f, item_position },
								is_selected_item ? defs::grey_color : defs::pink_color,
								11.f,
								L"%i.00", item.get<int>()
							);

							break;
						}
						default:break;
						}
					}
				}
			}
		}
	};



	namespace render
	{

		impl::menu_t men;
;

		void thread()
		{
			const auto game_window = FindWindowW(L"UnityWndClass", nullptr);

			if (!game_window)
				return;

			renderer = std::make_unique<dx_renderer>(game_window);

			if (!renderer)
				return;



			while (!thread_ejection.load())
			{
				renderer->begin_scene();

				renderer->draw_text<true>({ 20.f, 25.f }, { 52, 152, 235 }, 15.f, L"EV");

				men.poll_keyboard();
				men.renderMenu();



				if (impl::player_list.empty())
				{
					renderer->end_scene();
					continue;
				}
				

				if (showPlayers) {

					impl::player_list_mutex.lock();

					for (const auto& player : impl::player_list)
					{
						/* shouldn't be needed, but in case */
						if (!player)
							continue;

						if (local_player == NULL) {
							printf("local_player = null");
							continue;
						}
						glm::vec2 player_screen{};
						//const glm::vec3 player_position = get_player_pos(player);
						const glm::vec3 player_position = get_obj_pos(player);


						if (!impl::world_to_screen(player_position, &player_screen))
							continue;

						const auto player_health = impl::get_player_health(player);
						const auto player_name = impl::get_player_name(player);
						const auto player_item = impl::get_player_item_name(player);
						const auto player_sleeping = impl::get_player_sleeping(player);
						const int player_distance = (int)ceil(glm::distance(player_position, get_obj_pos(local_player)));
						const bool same_team = is_same_team(player, local_player);
						if (player_health == 0) {
							continue;
						}
						if (player_distance > 400)
							continue;

						if (!player_sleeping)
						{
							renderer->draw_text(
								{ player_screen.x - 5.f, player_screen.y - 10.f },
								{ same_team ? 147u, 201u, 56u : 255,0,0 },
								11.f,
								L"%ls \n%lu HP \n%ls%dm", player_name.c_str(), player_health, player_item.empty() ? L"" : (player_item + std::wstring(L"\n")).c_str(), player_distance
							);

							if (boxESP)
							{
								glm::vec2 tempFeetR, tempFeetL, tempHead;

								if (!impl::world_to_screen(impl::get_bone_position(player, r_foot), &tempFeetR) || !impl::world_to_screen(impl::get_bone_position(player, l_foot), &tempFeetL) || !impl::world_to_screen(impl::get_bone_position(player, jaw) + glm::vec3(0.f, 0.16f, 0.f), &tempHead))
									continue;

								glm::vec2 tempFeet = (tempFeetR + tempFeetL) / 2.f;
								float Entity_h = tempHead.y - tempFeet.y;
								float w = Entity_h / 4;
								float Entity_x = tempFeet.x - w;
								float Entity_y = tempFeet.y;
								float Entity_w = Entity_h / 2;
								renderer->rectangle(Entity_x, Entity_y, Entity_w, Entity_h, { 255,0,0 });
							}
						}
						else
						{

							renderer->draw_text(
								{ player_screen.x - 5.f, player_screen.y - 10.f },
								{ same_team ? 147u, 201u, 56u : 52, 152, 235 },
								11.f,
								L"%ls \n%lu HP \n%ls%dm", player_name.c_str(), player_health, L"", player_distance
							);
						}

						if (skeleton)
						{
							BoneList Bones[15] = {
								/*LF*/l_foot, l_knee, l_hip,
								/*RF*/r_foot, r_knee, r_hip,
								/*BD*/spine1, neck, head,
								/*LH*/l_upperarm, l_forearm, l_hand,
								/*RH*/r_upperarm, r_forearm, r_hand
							}; glm::vec2 BonesPos[15];
							bool dontDoBones = false;

							for (int j = 0; j < 15; j++) {
								if (!impl::world_to_screen(impl::get_bone_position(player, Bones[j]), &BonesPos[j]))
								{
									dontDoBones = true;
									break;
								}
							}

							if (!dontDoBones)
							{
								for (int j = 0; j < 15; j += 3) {
									renderer->Line({ BonesPos[j].x, BonesPos[j].y }, { BonesPos[j + 1].x, BonesPos[j + 1].y }, player_sleeping ? glm::uvec3{ 52, 152, 235 } : glm::uvec3{ 255, 0, 0 }, 1.0f);
									renderer->Line({ BonesPos[j + 1].x, BonesPos[j + 1].y }, { BonesPos[j + 2].x, BonesPos[j + 2].y }, player_sleeping ? glm::uvec3{ 52, 152, 235 } : glm::uvec3{ 255, 0, 0 }, 1.0f);
								}

								renderer->Line({ BonesPos[2].x, BonesPos[2].y }, { BonesPos[6].x, BonesPos[6].y }, player_sleeping ? glm::uvec3{ 52, 152, 235 } : glm::uvec3{ 255, 0, 0 }, 1.0f);
								renderer->Line({ BonesPos[5].x, BonesPos[5].y }, { BonesPos[6].x, BonesPos[6].y }, player_sleeping ? glm::uvec3{ 52, 152, 235 } : glm::uvec3{ 255, 0, 0 }, 1.0f);
								renderer->Line({ BonesPos[9].x, BonesPos[9].y }, { BonesPos[7].x, BonesPos[7].y }, player_sleeping ? glm::uvec3{ 52, 152, 235 } : glm::uvec3{ 255, 0, 0 }, 1.0f);
								renderer->Line({ BonesPos[12].x, BonesPos[12].y }, { BonesPos[7].x, BonesPos[7].y }, player_sleeping ? glm::uvec3{ 52, 152, 235 } : glm::uvec3{ 255, 0, 0 }, 1.0f);
							}
						}
					}

					impl::player_list_mutex.unlock();

				}

				if (showScientists) {


					impl::scientists_list_mutex.lock();

					for (const auto& scientist : impl::scientists_list)
					{
						/* shouldn't be needed, but in case */
						if (!scientist)
							continue;


						glm::vec2 scientist_screen{};
						const glm::vec3 scientist_position = get_player_pos(scientist);


						if (!impl::world_to_screen(scientist_position, &scientist_screen))
							continue;

						const auto scientist_health = impl::get_player_health(scientist);
						const auto scientist_item = impl::get_player_item_name(scientist);
						const int scientist_distance = (int)ceil(distance(scientist_position, get_obj_pos(local_player)));


						renderer->draw_text(
							{ scientist_screen.x - 5.f, scientist_screen.y - 10.f },
							{ 255, 228, 255 },
							11.f,
							L"Scientist \n%lu HP \n%ls%dm", scientist_health, scientist_item.empty() ? L"" : (scientist_item + std::wstring(L"\n")).c_str(), scientist_distance
						);

						//glm::vec2 player_origin_screen{};

						//if (!impl::world_to_screen(scientist_position, &player_origin_screen))
						//	continue;
						//glm::vec2 player_head_screen{};

						//if (!impl::world_to_screen((scientist_position + glm::vec3(-0.4, 1.7, 0)), &player_head_screen))
						//	continue;

						//float height = std::abs(player_origin_screen.y - player_head_screen.y);
						//float width = height / 2;

						//renderer->rectangle(player_head_screen.x, player_head_screen.y, width, height, { 255, 228, 255 });

					}

					impl::scientists_list_mutex.unlock();

				}

				if (showStashes) {


					impl::stash_list_mutex.lock();

					for (const auto& stash : impl::stash_list)
					{
						/* shouldn't be needed, but in case */
						if (!stash)
							continue;


						glm::vec2 stash_screen{};
						const glm::vec3 stash_position = get_obj_pos(stash);


						if (!impl::world_to_screen(stash_position, &stash_screen))
							continue;

						const int stash_distance = (int)ceil(distance(stash_position, get_obj_pos(local_player)));


						renderer->draw_text(
							{ stash_screen.x - 5.f, stash_screen.y - 10.f },
							{ 255, 228, 255 },
							11.f,
							L"Stash \n%dm", stash_distance
						);

					}

					impl::stash_list_mutex.unlock();

				}

				/*		COLL ESP		*/

				impl::collectible_list_mutex.lock();

				for (const auto& collectible : impl::collectible_list)
				{

					/* garbo shiz again */
					const auto unk1 = impl::memory->read(collectible + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = impl::memory->read(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = impl::memory->read(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!collectible)
						continue;

					const auto collectName = memory->read(unk2 + 0x60);
					std::string name = memory->read_ascii(collectName, 64);


					if (name.find("hemp") != std::string::npos) {

						if (showHemp) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Hemp \n%dm", hemp_distance
							);
						}
					}

					if (name.find("mushroom") != std::string::npos) {

						if (showMushrooms) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Mushroom \n%dm", hemp_distance
							);
						}
					}

					if (name.find("pumpkin") != std::string::npos) {

						if (showPumpkins) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Pumpkin \n%dm", hemp_distance
							);
						}
					}

					if (name.find("corn") != std::string::npos) {

						if (showCorn) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Corn \n%dm", hemp_distance
							);
						}
					}

					if (name.find("wood") != std::string::npos) {

						if (showWoodC) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Wood Collectable \n%dm", hemp_distance
							);
						}
					}

					if (name.find("stone-col") != std::string::npos) {

						if (showStoneC) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Stone Collectable \n%dm", hemp_distance
							);
						}
					}

					if (name.find("metal") != std::string::npos) {

						if (showMetalC) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Metal Collectable \n%dm", hemp_distance
							);
						}
					}

					if (name.find("sulfur") != std::string::npos) {

						if (showSulfurC) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Sulfur Collectable \n%dm", hemp_distance
							);
						}
					}




				}


				impl::collectible_list_mutex.unlock();

				/*		ORES ESP		*/

				impl::ore_list_mutex.lock();

				for (const auto& ore : impl::ore_list)
				{

					/* garbo shiz again */
					const auto unk1 = impl::memory->read(ore + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = impl::memory->read(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = impl::memory->read(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!ore)
						continue;




					const auto oreName = memory->read(unk2 + 0x60);
					std::string name = memory->read_ascii(oreName, 64);


					if (name.find("stone-ore") != std::string::npos) {

						if (showStone) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Stone Ore \n%dm", hemp_distance
							);
						}
					}

					if (name.find("metal-ore") != std::string::npos) {

						if (showMetal) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Metal Ore \n%dm", hemp_distance
							);
						}
					}

					if (name.find("sulfur-ore") != std::string::npos) {

						if (showSulfur) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));
							const int dis = glm::distance(hemp_position, get_obj_pos(local_player));

							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Sulfur Ore \n%dm", hemp_distance
							);
						}
					}


				}

				impl::ore_list_mutex.unlock();

				/*		BOXX ESP		*/
				impl::crate_list_mutex.lock();

				for (const auto& crate : impl::crate_list)
				{

					/* garbo shiz again */
					const auto unk1 = impl::memory->read(crate + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = impl::memory->read(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = impl::memory->read(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!crate)
						continue;




					const auto crateName = memory->read(unk2 + 0x60);
					std::string name = memory->read_ascii(crateName, 64);


					if (name.find("crate_tools") != std::string::npos) {

						if (showToolsCrate) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Tools Crate \n%dm", hemp_distance
							);
						}
					}

					if (strstr(name.c_str(), "assets/bundled/prefabs/radtown/crate_normal.prefab")) {

						if (showNormalCrate) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Military Crate \n%dm", hemp_distance
							);
						}
					}

					if (strstr(name.c_str(), "assets/bundled/prefabs/radtown/crate_normal_2.prefab")) {

						if (showMilitaryCrate) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Normal Crate \n%dm", hemp_distance
							);
						}
					}

					if (name.find("crate_elite") != std::string::npos) {

						if (showEliteCrate) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Elite Crate \n%dm", hemp_distance
							);
						}
					}

					if (name.find("barrel") != std::string::npos) {

						if (showBarrelCrate) {
							glm::vec2 hemp_screen{};
							const glm::vec3 hemp_position = get_obj_pos(unk3);


							if (!impl::world_to_screen(hemp_position, &hemp_screen))
								continue;

							const int hemp_distance = glm::distance(hemp_position, get_obj_pos(local_player));


							renderer->draw_text(
								{ hemp_screen.x - 5.f, hemp_screen.y - 10.f },
								{ 255, 228, 255 },
								11.f,
								L"Barrel \n%dm", hemp_distance
							);
						}
					}





				}

				impl::crate_list_mutex.unlock();

				//do_bullet_thickness(local_player);

				/* +++ ADMIN FLAG SET +++ */
				if (local_player) {
					do_admin_flag(local_player, doAdminFlag);

					if (doSpiderman) {
						do_spiderman(local_player);
					}

					disable_deviations(local_player, doAntiRecoil, alltimeEoka, doInstantCharge, doRemoveMovePenalty, forceAutomatic, bulletThickness, extendMeeleRange);
		
				}
	


				renderer->end_scene();

				th::sleep_for(ch::milliseconds(1));
			}

			impl::render_ejected.store(true);
		}


		float distance(glm::vec3 input, glm::vec3 input2)
		{
			return std::sqrt((input.x * input2.x) + (input.y * input2.y) + (input.z * input2.z));

		}


	}

}

