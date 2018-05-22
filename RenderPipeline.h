#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Material.h"
#include "VertexShader.h"
#include "Vertex.h"


struct edge_t
{
	VertexPNT v, v1, v2; 
};

struct trapezoid_t
{
	float top, bottom;
	edge_t left, right; 
} ;


struct scanline_t
{
	VertexPNT curVertexPNT, step;
	int xOffset, yOffset, width; 
};

class Renderer
{
	Matrix4x4  m_worldMatrix;
	Matrix4x4  m_viewMatix;
	Matrix4x4  m_projectionMatrix;
	Matrix4x4  m_worldViewProjectMatrix;

	float mViewPortW;
	float mViewPortH;

	uint32* texture;
	uint32* framebuffer;

	Vector m_pointLightPos;
	CColor m_lightColor;

	CColor m_ambient;

public:
	Renderer(float x, float y, float w, float h);

	~Renderer();

	bool init()
	{
		return true;
	}

	///////////////////////////////Transform///////////////////////////////////

	void setViewPort(float xPos, float yPos, float width, float height);

	void setWorldTransform(Matrix4x4& m)
	{
		m_worldMatrix = m;
		updateTransform();
	}

	void setViewer(Vector eye, Vector lookAt, Vector up)
	{
		m_viewMatix =  Matrix4x4::view(eye, lookAt, up);
		updateTransform();
	}

	void setPerspectiveProjection(float fov, float nearPlane, float farPlane)
	{
		m_projectionMatrix = Matrix4x4::project(m_aspectRatio, fov, nearPlane, farPlane);
		updateTransform();
	}


	///////////////////////////////Light//////////////////////////////////////
	void setAmbientLight(CColor color)
	{
		m_ambient = color;
	}

	void setPointLight(Vector pos, CColor color)
	{
		m_pointLightPos = pos;
		m_lightColor = color;
	}

	CColor calcLight(const Vector& vPos, const Vector& normal, CColor diffuseMaterial);

	//////////////////////////////Render//////////////////////////////////////
	void clear(bool colorBuff = true, bool zBuff = true, bool stencilBuff = true, CColor color = CColor::WHITE, float z = 1.f, float stencil = 1.f);

	void drawPrimitive(const VertexPNT *v1,  const VertexPNT *v2, const VertexPNT *v3);

protected:

	void updateTransform()
	{
		m_worldViewProjectMatrix = m_worldMatrix*m_viewMatix*m_projectionMatrix;
		return;
	}

	float getViewPortAspectRatio();

	///////////////RenderPipeLine////////////////////
	void VertexPNTProcess();
	void clip();
	void screenMapping();
	void Rasterization();
	void shadeFragment();
	void updateFrameBuff();

	bool clip(const Vector& v);

	bool ccwCulling(const Vector& a, const Vector& b, const Vector& c);

	void normalization(Vector& v);

	Vector screenMapping(const Vector& x);

	void vertexPretreatment(VertexPNT& v);

	int trapezoid_init_triangle(trapezoid_t *trap, const VertexPNT *p1,
		const VertexPNT *p2, const VertexPNT *p3);
	
	void device_render_trap(trapezoid_t *trap);
	
	void trapezoid_edge_interp(trapezoid_t *trap, float y);
	void VertexPNT_interp(VertexPNT *y, const VertexPNT *x1, const VertexPNT *x2, float t);
	void trapezoid_init_scan_line(const trapezoid_t *trap, scanline_t *scanline, int y);
	void VertexPNT_division(VertexPNT *y, const VertexPNT *x1, const VertexPNT *x2, float w);
	void device_draw_scanline(scanline_t *scanline);
	void VertexPNT_add(VertexPNT *y, const VertexPNT *x);
	uint32 device_texture_read(float u, float v);

private:
	float m_aspectRatio;
};

