#pragma once
#include <windows.h>
#include "RenderPipeline.h"

struct device_t{
	uint32 **framebuffer;      // ���ػ��棺framebuffer[y] ����� y��
	float **zbuffer;            // ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��
	float **stencilBuffer;		// ģ�建�棺stencil[y] Ϊ�� y��ָ��
	uint32 **texture;          // ����ͬ����ÿ������
	int tex_width;              // ������
	int tex_height;             // ����߶�
	float max_u;                // ��������ȣ�tex_width - 1
	float max_v;                // �������߶ȣ�tex_height - 1
	int render_state;           // ��Ⱦ״̬
	uint32 background;         // ������ɫ
	uint32 foreground;         // �߿���ɫ
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
	int screen_keys[512];	// ��ǰ���̰���״̬

	class Scene* m_pScene;
	class Renderer* m_pRender;

	device_t device;
};

extern WinApp* g_pApp;

