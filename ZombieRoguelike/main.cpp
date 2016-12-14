#include "stdafx.h"
#define CATCH_CONFIG_RUNNER

#include "main.h"
#include <Windows.h>
#include <iostream>
#include <chrono>

#define MS_PER_UPDATE 16

using namespace std::chrono;

Engine engine(100, 50);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
	engine.load();
	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	engine.save();
	return 0;
}

int main(int argc, char* const argv[])
{
	// global setup...
	engine.load();

	int result = Catch::Session().run(argc, argv);

	time_point<std::chrono::system_clock> previous = system_clock::now();
	double lag = 0.0;
	while (true)
	{
		time_point<std::chrono::system_clock> current = system_clock::now();
		std::chrono::duration<double, std::milli> elapsed = current - previous;
		previous = current;
		lag += elapsed.count();

		std::cout << lag << std::endl;

		while (lag >= MS_PER_UPDATE)
		{
			engine.update();
			lag -= MS_PER_UPDATE;
		}

		engine.render();
		TCODConsole::flush();
	}

	//while (!TCODConsole::isWindowClosed()) {
	//	engine.update();
	//	engine.render();
	//	TCODConsole::flush();
	//}

	// global clean-up...
	engine.save();

	return result;
}