//========================================================================
// main.cpp
//========================================================================

#include <windows.h>

#include "MyGameApp.hpp" 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {

	MyGameApp app(hInstance);

	if (!app.Initialize())
		return -1;

	return app.Run();
}