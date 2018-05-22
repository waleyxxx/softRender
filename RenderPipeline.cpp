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
	// 计算宽高比，供透视投影使用
	m_aspectRatio = width / height;
}


void Renderer::drawPrimitive(const  VertexPNT *input1, const VertexPNT *input2, const VertexPNT *input3) 
{
	// 这里可以做一个优化，一个顶点可能被多个三角形处理,但是同一个顶点只做一次流水线变换的操作，把结果cache下来。
	VertexPNT v1 = *input1, v2 = *input2, v3 = *input3;

	// 计算光照
	v1.diffuse = calcLight(v1.pos, v1.normal, v1.diffuse);
	v2.diffuse = calcLight(v2.pos, v2.normal, v2.diffuse);
	v3.diffuse = calcLight(v3.pos, v3.normal, v3.diffuse);

	// 透视投影
	Vector c1 = Matrix4x4::transform(v1.pos, m_worldViewProjectMatrix);
	Vector c2 = Matrix4x4::transform(v2.pos, m_worldViewProjectMatrix);
	Vector c3 = Matrix4x4::transform(v3.pos, m_worldViewProjectMatrix);


	// 裁剪,just in or out,后续可改进.
	if (clip(c1) || clip(c2) || clip(c3))
		return;

	// 归一化之前，保存一下观察空间中顶点的z坐标,供后续插值使用（这个z坐标是保存在透视变化后的w分量中）
	float z1 = c1.w;
	float z2 = c2.w;
	float z3 = c3.w;

	// 归一化,得到NDC(标准化设备坐标)
	normalization(c1);
	normalization(c2);
	normalization(c3);

	// 屏幕映射
	Vector screenP1 = screenMapping(c1);
	Vector screenP2 = screenMapping(c2);
	Vector screenP3 = screenMapping(c3);

	// CCW背面挑拣（用屏幕坐标也可以，只不过顺时针环绕序的三角形 在屏幕映射后 会变成 逆时针）。
	if (ccwCulling(c1, c2, c3))
	{
		return;
	}

	VertexPNT t1 = v1, t2 = v2, t3 = v3;
	t1.pos = screenP1;
	t2.pos = screenP2;
	t3.pos = screenP3;
	t1.pos.w = z1; // pos.w = 摄像机空间中顶点坐标的z值,注意与pos.z（归一化的z值）的区别。
	t2.pos.w = z2;
	t3.pos.w = z3;

	//预处理。预乘1/z，这样顶点属性直接可以做线性插值
	vertexPretreatment(t1);
	vertexPretreatment(t2);
	vertexPretreatment(t3);

	// 拆分三角形为1-2个规则三角形（底边平行于扫描线的方向）
	trapezoid_t traps[2] = {0};
	int n = trapezoid_init_triangle(traps, &t1, &t2, &t3);

	for (int i = 0; i < n; ++i)
	{
		device_render_trap(&traps[i]);
	}
}

// 光照的计算直接在世界坐标系中计算
CColor Renderer::calcLight(const Vector& vPos, const Vector& vNormal, CColor diffuseMaterial)
{
	CColor ret;

	Vector worldPos = Matrix4x4::transform(vPos, m_worldMatrix);

	Vector vLight = m_pointLightPos - worldPos; // 入射点到点光源之间的距离向量

	//float distance_light_vertex = vLight.len(); 
	//float inv_distance_square = 1.f / vLight.len_square(); // 距离平方的倒数
	
	//CColor lightColor = m_lightColor*inv_distance_square; //点光源的强度与它到物体间的距离是平方反比的关系--辐射度理论。
	
	// 变换一下法向量（物体从局部坐标系->世界坐标系，法向量要做相应的变换）
	Matrix4x4 trans = m_worldMatrix.inverse().transpose(); 
	Vector nor = Matrix4x4::transform(vNormal, trans);
	nor.normalize();

	// 计算光线和法向量之间的余弦。(注:光线方向是从入射顶点到光源之间的方向)
	float cos_light_normal = max(vLight.normalize()*nor, 0);

	ret = m_ambient*diffuseMaterial + diffuseMaterial*(m_lightColor*cos_light_normal);

	return ret;
}

// 基本的ccv裁剪，out or in
//  -1<x<1
//  -1<y<1
//   0<z<1
bool Renderer::clip(const Vector& v)
{
	float w = v.w;
	if (fabsf(w)<FLT_EPSILON) // 预防除零异常
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

// NDC 到 屏幕坐标
Vector Renderer::screenMapping(const Vector& v) 
{
	Vector ret;
	
	ret.x = (v.x + 1.0f) * mViewPortW * 0.5f;
	ret.y = (1.0f - v.y) * mViewPortH * 0.5f;
	ret.z = v.z;
	ret.w = 1.0f;

	return ret;
}

// ccw背面裁剪（逆时针环绕序认为是背面）
bool Renderer::ccwCulling(const Vector& a, const Vector& b, const Vector& c)
{
	Vector z_axis(0, 0, 1); 
	float noraml_z_product = Vector::calcTriangleNormal(a, b, c)*z_axis; 
	return noraml_z_product>0;//通过向量点积的符号，判断两个向量的夹角是否大于90度
}

void Renderer::vertexPretreatment(VertexPNT& v)
{	
	float inv_z = 1.0f / v.pos.w; 
	v.pos.w = inv_z;  // 使得pos.w可以线性插值

	// 顶点属性乘以1/z，后续光栅化直接进行线性插值
	v.tc.u *= inv_z;
	v.tc.v *= inv_z;

	v.diffuse *= inv_z;
}

//拆分三角形为1-2个规则三角形（底边平行于扫描线的方向)
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

// 主渲染函数
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

// 按照 Y 坐标计算出左右两条边纵坐标等于 Y 的顶点
void Renderer::trapezoid_edge_interp(trapezoid_t *trap, float y) 
{
	float s1 = trap->left.v2.pos.y - trap->left.v1.pos.y;
	float s2 = trap->right.v2.pos.y - trap->right.v1.pos.y;
	float t1 = (y - trap->left.v1.pos.y) / s1;
	float t2 = (y - trap->right.v1.pos.y) / s2;
	VertexPNT_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1);
	VertexPNT_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);
}

// 顶点属性线性插值
void Renderer::VertexPNT_interp(VertexPNT *y, const VertexPNT *x1, const VertexPNT *x2, float t) 
{
	y->pos = Vector::interp(x1->pos, x2->pos, t);
	y->diffuse = CColor::interp(x1->diffuse, x2->diffuse, t);
	y->tc.u = Util::interp(x1->tc.u, x2->tc.u, t);
	y->tc.v = Util::interp(x1->tc.v, x2->tc.v, t);
}

// 根据左右两边的端点，初始化计算出扫描线的起点和步长
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

//计算步长
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

// 绘制扫描线
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
			if (z_value <= zbuffer[x]) // z-test使用的是归一化的z坐标
			{ 
				zbuffer[x] = z_value; 
				float ccv_z = 1.0f / scanline->curVertexPNT.pos.w; // ccv_z表示观察空间中的z坐标,参见VertexPNTPretreatment()中pos.w的赋值.
				
				float r = scanline->curVertexPNT.diffuse.r * ccv_z; //乘以ccv_z，参考顶点属性的透视投影校正插值.
				float g = scanline->curVertexPNT.diffuse.g * ccv_z;
				float b = scanline->curVertexPNT.diffuse.b * ccv_z;


				float u = scanline->curVertexPNT.tc.u * ccv_z;
				float v = scanline->curVertexPNT.tc.v * ccv_z; //乘以ccv_z，参考顶点属性的透视投影校正插值.
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

// 根据坐标读取纹理
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