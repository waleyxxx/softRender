#pragma once

#include "Vertex.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	bool init();

	void update();
	void draw();

	void draw_cube();
	void draw_plane(int a, int b, int c, int d);

	void init_texture();

	void init_normal();

private:
	VertexPNT m_vertices[24];

	VertexPNT m_floor[4];
};

