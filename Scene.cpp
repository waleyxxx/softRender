#include "Scene.h"
#include "Vertex.h"
#include "App.h"

Scene::Scene()
{
}


Scene::~Scene()
{
}

bool Scene::init()
{	
	//  front face 
	m_vertices[0] = VertexPNT(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f);
	m_vertices[1] = VertexPNT(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f);
	m_vertices[2] = VertexPNT(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f);
	m_vertices[3] = VertexPNT(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);

	//  back face 
	m_vertices[4] = VertexPNT(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);
	m_vertices[5] = VertexPNT(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f);
	m_vertices[6] = VertexPNT(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f);
	m_vertices[7] = VertexPNT(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f);

	// top face 
	m_vertices[8] = VertexPNT(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f);
	m_vertices[9] = VertexPNT(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f);
	m_vertices[10] = VertexPNT(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f);
	m_vertices[11] = VertexPNT(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);

	// bottom face
	m_vertices[12] = VertexPNT(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);
	m_vertices[13] = VertexPNT(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f);
	m_vertices[14] = VertexPNT(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f);
	m_vertices[15] = VertexPNT(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f);

	// left face
	m_vertices[16] = VertexPNT(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f);
	m_vertices[17] = VertexPNT(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f);
	m_vertices[18] = VertexPNT(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f);
	m_vertices[19] = VertexPNT(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);

	// right face 
	m_vertices[20] = VertexPNT(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 1.0f);
	m_vertices[21] = VertexPNT(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 0.0f, 0.0f);
	m_vertices[22] = VertexPNT(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 0.0f);
	m_vertices[23] = VertexPNT(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.7f, 0.7f, 0.7f, 1.0f, 1.0f);

	init_texture();

	// vertex normal average
	init_normal();

	g_pApp->getRender()->setPointLight(Vector(10, 10, 10), CColor::WHITE);

	g_pApp->getRender()->setAmbientLight(CColor(0.3f, 0.3f, 0.3f));

	g_pApp->getRender()->setViewer(Vector(0,0,0), Vector(0,0,1), Vector(0,1,0));

	g_pApp->getRender()->setPerspectiveProjection(PI/2, 1.f, 500.f);

	return true;
}


void Scene::update()
{
	Matrix4x4 r = Matrix4x4::rotation(Vector(-1, -0.5, 1), 1);
	Matrix4x4 t = Matrix4x4::translate(0,0,5);
	Matrix4x4 m = r*t;
	g_pApp->getRender()->setWorldTransform(m);
}

void Scene::draw()
{
	draw_cube();
}

void Scene::draw_cube() 
{
	draw_plane(0, 1, 2, 3);
	draw_plane(4, 5, 6, 7);
	draw_plane(8, 9, 10, 11);
	draw_plane(12, 13, 14, 15);
	draw_plane(16, 17, 18, 19);
	draw_plane(20, 21, 22, 23);
}

void Scene::draw_plane(int a, int b, int c, int d)
{
	VertexPNT p1 = m_vertices[a], p2 = m_vertices[b], p3 = m_vertices[c], p4 = m_vertices[d];
	g_pApp->getRender()->drawPrimitive(&p1, &p2, &p3);
	g_pApp->getRender()->drawPrimitive(&p3, &p4, &p1);
}

// 手动创建一个类似棋盘的纹理
void Scene::init_texture() {
	static uint32 texture[256][256];
	int i, j;
	for (j = 0; j < 256; j++) {
		for (i = 0; i < 256; i++) {
			int x = i / 32, y = j / 32;
			texture[j][i] = ((x + y) & 1) ? 0xffffff : 0x4abcde;
		}
	}

	g_pApp->device_set_texture(texture, 256 * 4, 256, 256);
}

void Scene::init_normal()
{
	for (int i = 0; i < 8; ++i)
	{
		VertexPNT* arr[3];
		arr[0] = &m_vertices[i];
		int index = 1;

		for (int j = 8; j < 24; ++j)
		{
			if (m_vertices[i].pos == m_vertices[j].pos)
			{
				arr[index++] = &m_vertices[j];
				if (index==3)
				{
					break;
				}
			}
		}

		assert(index == 3);

		Vector v[3] = { arr[0]->normal, arr[1]->normal, arr[2]->normal };
		Vector avg = Vector::average(v, 3);
		arr[0]->normal = arr[1]->normal = arr[2]->normal = avg;
	}
}