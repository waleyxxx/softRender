#pragma once
#include <windows.h>
#include "RenderPipeline.h"

struct device_t{
	uint32 **framebuffer;      // 像素缓存：framebuffer[y] 代表第 y行
	float **zbuffer;            // 深度缓存：zbuffer[y] 为第 y行指针
	float **stencilBuffer;		// 模板缓存：stencil[y] 为第 y行指针
	uint32 **texture;          // 纹理：同样是每行索引
	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	float max_u;                // 纹理最大宽度：tex_width - 1
	float max_v;                // 纹理最大高度：tex_height - 1
	int render_state;           // 渲染状态
	uint32 background;         // 背景颜色
	uint32 foreground;         // 线框颜色
};

class WinApp
{
	friend class Renderer;
	friend class Scene;
public:
	WinApp();
	virtual ~WinApp();

	bool init(int w, int h);

	int run();

	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT MessageProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void dispatchMessages();

	bool initSubSystem();

	void device_init(device_t *device, int width, int height, void *fb);
	void device_set_texture(void *bits, long pitch, int w, int h);
	void device_clear();

	void beginScene();
	void endScene();

	Renderer* getRender()
	{
		return m_pRender;
	}

private:
	HWND screen_handle;
	HDC screen_dc;
	HBITMAP screen_hb;
	HBITMAP screen_ob;
	char* screen_fb;
	int screen_w;
	int screen_h;
	int screen_pitch;

	int screen_exit;
	int screen_keys[512];	// 当前键盘按下状态

	class Scene* m_pScene;
	class Renderer* m_pRender;

	device_t device;
};

extern WinApp* g_pApp;

