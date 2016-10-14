#include "stdafx.h"
#define CATCH_CONFIG_RUNNER

#include "main.h"
#include <Windows.h>
#include <iostream>

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

	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}

	// global clean-up...
	engine.save();

	return result;
}

//int main() {
//	//FreeConsole();
//
//	engine.load();
//	while (!TCODConsole::isWindowClosed()) {
//		engine.update();
//		engine.render();
//		TCODConsole::flush();
//	}
//	engine.save();
//	return 0;
//}