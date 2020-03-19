#pragma once
#include "iteration.hpp"
#include "other_globals.hpp"
#include "renderer.hpp"
#include "Windows.h" 
#include "d3d11.h"

	int main()
	{
		/* initialize the memory class, use any registry key, then the process name. */
		impl::memory = std::make_unique<memory_mgr>("Uninstall", "RustClient.exe");



		/* create individual threads, then let them be independent */
		std::thread iteration(&impl::iter::thread);
		iteration.detach();

		std::thread renderer(&impl::render::thread);
		renderer.detach();

		impl::render::men.push_sections(impl::MenuSections);



		while (!GetAsyncKeyState(VK_END))
			th::sleep_for(ch::milliseconds(150));

		/* this has to be the way to eject from detached independent threads, since we can't close them from this anymore */
		impl::thread_ejection.store(true);

		while (!impl::iter_ejected.load() && !impl::render_ejected.load())
			th::sleep_for(ch::milliseconds(50));

		return 1;
	}

