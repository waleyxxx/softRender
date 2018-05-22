#include "App.h"
#include <tchar.h>
#include "Scene.h"

WinApp* g_pApp = NULL;

WinApp::WinApp()
:m_pScene(NULL)
{

}

WinApp::~WinApp()
{
	SAFE_DELETE(m_pScene);
	SAFE_DELETE(m_pRender);
}

bool WinApp::init(int w, int h)
{
	LPCWSTR wndClassName = _T("SoftRender");
	LPCWSTR tileName = _T("SoftRenderDemo");

	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)WindowProcedure, 0, 0, 0,
		NULL, NULL, NULL, NULL, wndClassName };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
					w * h * 4, 0, 0, 0, 0 } };
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc))
		return false;

	screen_handle = CreateWindow(wndClassName, tileName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if (screen_handle == NULL) 
		return false;

	hDC = GetDC(screen_handle);
	screen_dc = CreateCompatibleDC(hDC);
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if (screen_hb == NULL) 
		return false;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
	screen_fb = (char*)ptr;
	screen_w = w;
	screen_h = h;
	screen_pitch = w * 4;

	screen_exit = 0;

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(screen_handle);

	ShowWindow(screen_handle, SW_NORMAL);

	memset(screen_keys, 0, sizeof(int)* 512);
	memset(screen_fb, 0, w * h * 4);

	device_init(&device, w, h, screen_fb);

	return initSubSystem();
}

void WinApp::device_init(device_t *device, int width, int height, void *fb) 
{
	int need = sizeof(void*) * (height * 3 + 1024) + width * height * 4 * 3;
	char *ptr = (char*)malloc(need + 64);
	char *framebuf, *zbuf, *stencilbuf;
	int j;
	assert(ptr);
	// 行索引
	device->framebuffer = (uint32**)ptr;
	device->zbuffer = (float**)(ptr + sizeof(void*) * height);
	device->stencilBuffer = (float**)(ptr + sizeof(void*)* height*2);
	ptr += sizeof(void*) * height * 3;
	device->texture = (uint32**)ptr;
	ptr += sizeof(void*) * 1024;

	framebuf = (char*)ptr;
	zbuf = (char*)ptr + width * height * 4;
	stencilbuf = (char*)ptr + width * height * 4 * 2;
	ptr += width * height * 12;
	if (fb != NULL) framebuf = (char*)fb;
	for (j = 0; j < height; j++) 
	{
		device->framebuffer[j] = (uint32*)(framebuf + width * 4 * j);
		device->zbuffer[j] = (float*)(zbuf + width * 4 * j);
		device->stencilBuffer[j] = (float*)(stencilbuf + width * 4 * j);
	}
	device->texture[0] = (uint32*)ptr;
	device->texture[1] = (uint32*)(ptr + 16);
	memset(device->texture[0], 0, 64);
	device->tex_width = 2;
	device->tex_height = 2;
	device->max_u = 1.0f;
	device->max_v = 1.0f;
	device->background = 0xc0c0c0;
	device->foreground = 0;
}


// 设置当前使用的纹理
void WinApp::device_set_texture(void *bits, long pitch, int w, int h) 
{
	char *ptr = (char*)bits;
	int j;
	assert(w <= 1024 && h <= 1024);
	for (j = 0; j < h; ptr += pitch, j++) 	// 重新计算每行纹理的指针
		device.texture[j] = (uint32*)ptr;
	device.tex_width = w;
	device.tex_height = h;
	device.max_u = (float)(w - 1);
	device.max_v = (float)(h - 1);
}

bool WinApp::initSubSystem()
{
	m_pRender = new Renderer(0, 0, float(screen_w), float(screen_h));
	m_pRender->init();

	m_pScene = new Scene();
	m_pScene->init();

	return true;
}

void WinApp::beginScene()
{
	device_clear();
}

void WinApp::device_clear() 
{
	int y, x, height = screen_h;
	for (y = 0; y < screen_h; y++) 
	{
		uint32 *dst = device.framebuffer[y];
		for (x = screen_w; x > 0; dst++, x--) 
			dst[0] = 0xaaaaaa; // 背景色
	}

	for (y = 0; y < screen_h; y++) 
	{
		float *dst = device.zbuffer[y];
		for (x = screen_w; x > 0; dst++, x--)
			dst[0] = 1.0f;  // z-buff
	}

	for (y = 0; y < screen_h; y++)
	{
		float *dst = device.stencilBuffer[y];
		for (x = screen_w; x > 0; dst++, x--)
			dst[0] = 1.0f;  // s-buff
	}
}

void WinApp::endScene()
{
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
}

int WinApp::run()
{
	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0)
	{
		beginScene();

		m_pScene->update();
		m_pScene->draw();

		endScene();

		dispatchMessages();

		Sleep(1);
	}
	return 0;
}

void WinApp::dispatchMessages()
{
	MSG msgMessage;
	if (!PeekMessage(&msgMessage, 0, 0, 0, PM_REMOVE))
		return;

	TranslateMessage(&msgMessage);
	DispatchMessage(&msgMessage);
}

LRESULT CALLBACK WinApp::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pApp->MessageProcedure(hWnd, uMsg, wParam, lParam);
}

LRESULT WinApp::MessageProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			screen_exit = 1;
			break;
		case WM_KEYDOWN:
			screen_keys[wParam & 511] = 1; 
			break;
		case WM_KEYUP:
			screen_keys[wParam & 511] = 0; 
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

