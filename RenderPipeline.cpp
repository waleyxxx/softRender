#include "RenderPipeline.h"
#include "Vertex.h"
#include "App.h"

Renderer::Renderer(float x, float y, float w, float h)
:m_aspectRatio(0)
{
	setViewPort(x, y, w, h);

	mViewPortW = w;
	mViewPortH = h;

	m_worldMatrix.identity();
	m_viewMatix.identity();
	m_projectionMatrix.identity();
	m_worldViewProjectMatrix.identity();
}

Renderer::~Renderer()
{
}

void Renderer::setViewPort(float xPos, float yPos, float width, float height)
{
	// �����߱ȣ���͸��ͶӰʹ��
	m_aspectRatio = width / height;
}


void Renderer::drawPrimitive(const  VertexPNT *input1, const VertexPNT *input2, const VertexPNT *input3) 
{
	// ���������һ���Ż���һ��������ܱ���������δ���,����ͬһ������ֻ��һ����ˮ�߱任�Ĳ������ѽ��cache������
	VertexPNT v1 = *input1, v2 = *input2, v3 = *input3;

	// �������
	v1.diffuse = calcLight(v1.pos, v1.normal, v1.diffuse);
	v2.diffuse = calcLight(v2.pos, v2.normal, v2.diffuse);
	v3.diffuse = calcLight(v3.pos, v3.normal, v3.diffuse);

	// ͸��ͶӰ
	Vector c1 = Matrix4x4::transform(v1.pos, m_worldViewProjectMatrix);
	Vector c2 = Matrix4x4::transform(v2.pos, m_worldViewProjectMatrix);
	Vector c3 = Matrix4x4::transform(v3.pos, m_worldViewProjectMatrix);


	// �ü�,just in or out,�����ɸĽ�.
	if (clip(c1) || clip(c2) || clip(c3))
		return;

	// ��һ��֮ǰ������һ�¹۲�ռ��ж����z����,��������ֵʹ�ã����z�����Ǳ�����͸�ӱ仯���w�����У�
	float z1 = c1.w;
	float z2 = c2.w;
	float z3 = c3.w;

	// ��һ��,�õ�NDC(��׼���豸����)
	normalization(c1);
	normalization(c2);
	normalization(c3);

	// ��Ļӳ��
	Vector screenP1 = screenMapping(c1);
	Vector screenP2 = screenMapping(c2);
	Vector screenP3 = screenMapping(c3);

	// CCW������������Ļ����Ҳ���ԣ�ֻ����˳ʱ�뻷����������� ����Ļӳ��� ���� ��ʱ�룩��
	if (ccwCulling(c1, c2, c3))
	{
		return;
	}

	VertexPNT t1 = v1, t2 = v2, t3 = v3;
	t1.pos = screenP1;
	t2.pos = screenP2;
	t3.pos = screenP3;
	t1.pos.w = z1; // pos.w = ������ռ��ж��������zֵ,ע����pos.z����һ����zֵ��������
	t2.pos.w = z2;
	t3.pos.w = z3;

	//Ԥ����Ԥ��1/z��������������ֱ�ӿ��������Բ�ֵ
	vertexPretreatment(t1);
	vertexPretreatment(t2);
	vertexPretreatment(t3);

	// ���������Ϊ1-2�����������Σ��ױ�ƽ����ɨ���ߵķ���
	trapezoid_t traps[2] = {0};
	int n = trapezoid_init_triangle(traps, &t1, &t2, &t3);

	for (int i = 0; i < n; ++i)
	{
		device_render_trap(&traps[i]);
	}
}

// ���յļ���ֱ������������ϵ�м���
CColor Renderer::calcLight(const Vector& vPos, const Vector& vNormal, CColor diffuseMaterial)
{
	CColor ret;

	Vector worldPos = Matrix4x4::transform(vPos, m_worldMatrix);

	Vector vLight = m_pointLightPos - worldPos; // ����㵽���Դ֮��ľ�������

	//float distance_light_vertex = vLight.len(); 
	//float inv_distance_square = 1.f / vLight.len_square(); // ����ƽ���ĵ���
	
	//CColor lightColor = m_lightColor*inv_distance_square; //���Դ��ǿ�������������ľ�����ƽ�����ȵĹ�ϵ--��������ۡ�
	
	// �任һ�·�����������Ӿֲ�����ϵ->��������ϵ��������Ҫ����Ӧ�ı任��
	Matrix4x4 trans = m_worldMatrix.inverse().transpose(); 
	Vector nor = Matrix4x4::transform(vNormal, trans);
	nor.normalize();

	// ������ߺͷ�����֮������ҡ�(ע:���߷����Ǵ����䶥�㵽��Դ֮��ķ���)
	float cos_light_normal = max(vLight.normalize()*nor, 0);

	ret = m_ambient*diffuseMaterial + diffuseMaterial*(m_lightColor*cos_light_normal);

	return ret;
}

// ������ccv�ü���out or in
//  -1<x<1
//  -1<y<1
//   0<z<1
bool Renderer::clip(const Vector& v)
{
	float w = v.w;
	if (fabsf(w)<FLT_EPSILON) // Ԥ�������쳣
	{
		return true;
	}

	float inv_w = 1.f / w;

	if (v.x*inv_w<-1.f || v.x*inv_w>1.f)
	{
		return true;
	}

	if (v.y*inv_w<-1.f || v.x*inv_w>1.f)
	{
		return true;
	}

	if (v.z*inv_w<0 || v.z*inv_w>1.f)
	{
		return true;
	}

	return false;
}

void Renderer::normalization(Vector& v)
{
	float inv_z = 1.f / v.w;
	v.x *= inv_z;
	v.y *= inv_z;
	v.z *= inv_z;
	v.w = 1.f;
}

// NDC �� ��Ļ����
Vector Renderer::screenMapping(const Vector& v) 
{
	Vector ret;
	
	ret.x = (v.x + 1.0f) * mViewPortW * 0.5f;
	ret.y = (1.0f - v.y) * mViewPortH * 0.5f;
	ret.z = v.z;
	ret.w = 1.0f;

	return ret;
}

// ccw����ü�����ʱ�뻷������Ϊ�Ǳ��棩
bool Renderer::ccwCulling(const Vector& a, const Vector& b, const Vector& c)
{
	Vector z_axis(0, 0, 1); 
	float noraml_z_product = Vector::calcTriangleNormal(a, b, c)*z_axis; 
	return noraml_z_product>0;//ͨ����������ķ��ţ��ж����������ļн��Ƿ����90��
}

void Renderer::vertexPretreatment(VertexPNT& v)
{	
	float inv_z = 1.0f / v.pos.w; 
	v.pos.w = inv_z;  // ʹ��pos.w�������Բ�ֵ

	// �������Գ���1/z��������դ��ֱ�ӽ������Բ�ֵ
	v.tc.u *= inv_z;
	v.tc.v *= inv_z;

	v.diffuse *= inv_z;
}

//���������Ϊ1-2�����������Σ��ױ�ƽ����ɨ���ߵķ���)
int Renderer::trapezoid_init_triangle(trapezoid_t *trap, const VertexPNT *p1,
									  const VertexPNT *p2, const VertexPNT *p3) 
{
	const VertexPNT *p;
	float k, x;

	if (p1->pos.y > p2->pos.y) p = p1, p1 = p2, p2 = p;
	if (p1->pos.y > p3->pos.y) p = p1, p1 = p3, p3 = p;
	if (p2->pos.y > p3->pos.y) p = p2, p2 = p3, p3 = p;
	if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y) return 0;
	if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x) return 0;

	if (p1->pos.y == p2->pos.y) 
	{	
		// triangle down
		if (p1->pos.x > p2->pos.x) p = p1, p1 = p2, p2 = p;
		trap[0].top = p1->pos.y;
		trap[0].bottom = p3->pos.y;
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p3;
		trap[0].right.v1 = *p2;
		trap[0].right.v2 = *p3;
		return (trap[0].top < trap[0].bottom)? 1 : 0;
	}

	if (p2->pos.y == p3->pos.y) 
	{
		// triangle up
		if (p2->pos.x > p3->pos.x) p = p2, p2 = p3, p3 = p;
		trap[0].top = p1->pos.y;
		trap[0].bottom = p3->pos.y;
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p2;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p3;
		return (trap[0].top < trap[0].bottom)? 1 : 0;
	}

	trap[0].top = p1->pos.y;
	trap[0].bottom = p2->pos.y;
	trap[1].top = p2->pos.y;
	trap[1].bottom = p3->pos.y;

	k = (p3->pos.y - p1->pos.y) / (p2->pos.y - p1->pos.y);
	x = p1->pos.x + (p2->pos.x - p1->pos.x) * k;

	if (x <= p3->pos.x)
	{		
		// triangle left
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p2;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p3;
		trap[1].left.v1 = *p2;
		trap[1].left.v2 = *p3;
		trap[1].right.v1 = *p1;
		trap[1].right.v2 = *p3;
	}
	else 
	{					
		// triangle right
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p3;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p2;
		trap[1].left.v1 = *p1;
		trap[1].left.v2 = *p3;
		trap[1].right.v1 = *p2;
		trap[1].right.v2 = *p3;
	}

	return 2;
}

// ����Ⱦ����
void Renderer::device_render_trap(trapezoid_t *trap) 
{
	scanline_t scanline;
	int j, top, bottom;
	top = (int)(trap->top + 0.5f);
	bottom = (int)(trap->bottom + 0.5f);
	for (j = top; j < bottom; j++) 
	{
		if (j >= 0 && j < mViewPortH) 
		{
			trapezoid_edge_interp(trap, (float)j + 0.5f);
			trapezoid_init_scan_line(trap, &scanline, j);
			device_draw_scanline(&scanline);
		}

		if (j >= mViewPortH) 
			break;
	}
}

// ���� Y ��������������������������� Y �Ķ���
void Renderer::trapezoid_edge_interp(trapezoid_t *trap, float y) 
{
	float s1 = trap->left.v2.pos.y - trap->left.v1.pos.y;
	float s2 = trap->right.v2.pos.y - trap->right.v1.pos.y;
	float t1 = (y - trap->left.v1.pos.y) / s1;
	float t2 = (y - trap->right.v1.pos.y) / s2;
	VertexPNT_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1);
	VertexPNT_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);
}

// �����������Բ�ֵ
void Renderer::VertexPNT_interp(VertexPNT *y, const VertexPNT *x1, const VertexPNT *x2, float t) 
{
	y->pos = Vector::interp(x1->pos, x2->pos, t);
	y->diffuse = CColor::interp(x1->diffuse, x2->diffuse, t);
	y->tc.u = Util::interp(x1->tc.u, x2->tc.u, t);
	y->tc.v = Util::interp(x1->tc.v, x2->tc.v, t);
}

// �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ���
void Renderer::trapezoid_init_scan_line(const trapezoid_t *trap, scanline_t *scanline, int yOffset) 
{
	float width = trap->right.v.pos.x - trap->left.v.pos.x;
	scanline->xOffset = (int)(trap->left.v.pos.x + 0.5f);
	scanline->width = (int)(trap->right.v.pos.x + 0.5f) - scanline->xOffset;
	scanline->yOffset = yOffset;
	scanline->curVertexPNT = trap->left.v;
	if (trap->left.v.pos.x >= trap->right.v.pos.x) scanline->width = 0;
	VertexPNT_division(&scanline->step, &trap->left.v, &trap->right.v, width);
}

//���㲽��
void Renderer::VertexPNT_division(VertexPNT *y, const VertexPNT *x1, const VertexPNT *x2, float width) 
{
	float inv_width = 1.0f / width;
	y->pos.x = (x2->pos.x - x1->pos.x) * inv_width;
	y->pos.y = (x2->pos.y - x1->pos.y) * inv_width;
	y->pos.z = (x2->pos.z - x1->pos.z) * inv_width;
	y->pos.w = (x2->pos.w - x1->pos.w) * inv_width;

	y->diffuse.r = (x2->diffuse.r - x1->diffuse.r) * inv_width;
	y->diffuse.g = (x2->diffuse.g - x1->diffuse.g) * inv_width;
	y->diffuse.b = (x2->diffuse.b - x1->diffuse.b) * inv_width;

	y->tc.u = (x2->tc.u - x1->tc.u) * inv_width;
	y->tc.v = (x2->tc.v - x1->tc.v) * inv_width;
}

// ����ɨ����
void Renderer::device_draw_scanline(scanline_t *scanline) 
{
	uint32 *framebuffer = g_pApp->device.framebuffer[scanline->yOffset];
	float *zbuffer = g_pApp->device.zbuffer[scanline->yOffset];
	int x = scanline->xOffset;
	int w = scanline->width;
	int width = (int)mViewPortW;

	for (; w > 0; x++, w--) 
	{
		if (x >= 0 && x < width) 
		{
			float z_value = scanline->curVertexPNT.pos.z;
			if (z_value <= zbuffer[x]) // z-testʹ�õ��ǹ�һ����z����
			{ 
				zbuffer[x] = z_value; 
				float ccv_z = 1.0f / scanline->curVertexPNT.pos.w; // ccv_z��ʾ�۲�ռ��е�z����,�μ�VertexPNTPretreatment()��pos.w�ĸ�ֵ.
				
				float r = scanline->curVertexPNT.diffuse.r * ccv_z; //����ccv_z���ο��������Ե�͸��ͶӰУ����ֵ.
				float g = scanline->curVertexPNT.diffuse.g * ccv_z;
				float b = scanline->curVertexPNT.diffuse.b * ccv_z;


				float u = scanline->curVertexPNT.tc.u * ccv_z;
				float v = scanline->curVertexPNT.tc.v * ccv_z; //����ccv_z���ο��������Ե�͸��ͶӰУ����ֵ.
				CColor tex =  device_texture_read(u, v);

				//diffuse*tex
				r *= tex.r;
				g *= tex.g;
				b *= tex.b;

				int R = (int)(r * 255.0f);
				int G = (int)(g * 255.0f);
				int B = (int)(b * 255.0f);
				R = Util::clamp(R, 0, 255);
				G = Util::clamp(G, 0, 255);
				B = Util::clamp(B, 0, 255);
				framebuffer[x] = (R << 16) | (G << 8) | (B);
			}
		}
		VertexPNT_add(&scanline->curVertexPNT, &scanline->step);
		if (x >= width) break;
	}
}

void Renderer::VertexPNT_add(VertexPNT *y, const VertexPNT *x) 
{
	y->pos.x += x->pos.x;
	y->pos.y += x->pos.y;
	y->pos.z += x->pos.z;
	y->pos.w += x->pos.w;

	y->diffuse.r += x->diffuse.r;
	y->diffuse.g += x->diffuse.g;
	y->diffuse.b += x->diffuse.b;

	y->tc.u += x->tc.u;
	y->tc.v += x->tc.v;
}

// ���������ȡ����
uint32 Renderer::device_texture_read(float u, float v) 
{
	int x, y;
	u = u * g_pApp->device.max_u;
	v = v * g_pApp->device.max_v;
	x = (int)(u + 0.5f);
	y = (int)(v + 0.5f);
	x = Util::clamp(x, 0, g_pApp->device.tex_width - 1);
	y = Util::clamp(y, 0, g_pApp->device.tex_height - 1);
	return g_pApp->device.texture[y][x];
}