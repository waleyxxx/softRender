#include "App.h"
#include "Plane.h"

int main()
{
	WinApp app;
	g_pApp = &app;
	if (g_pApp->init(640, 480))
	{
		return g_pApp->run();
	}

	return 0;
}