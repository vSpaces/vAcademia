#include "stdafx.h"
#include "nrmParticles.h"
#include "ParticlesDynamic\SmokeOfFire.h"
#include "ParticlesDynamic\SingleSmoke.h"

int nrmParticles::m_totalParticles = 0;

nrmParticles::nrmParticles(mlMedia* apMLMedia) :
	nrm3DObject(apMLMedia),
	m_textureId(g->tm.GetObjectNumber("SMOKE.PNG")),
	m_shader(GetShader()),
	m_maxParticles(0),
	m_countLived(0),
	m_density(1.f),
	m_vbo(NULL),
	m_distSq(0.f),
	m_dirChangeFactor(0.03f),
	m_speedChangeFactor(0.3f),	
	m_dynamic(NULL),
	m_sortVerticles(false),
	m_defPointSize(100.0f),
	m_useShader(g->rs.GetBool(SHADERS_USING))
{
	m_dynamic = new ParticleDynamic_SmokeOfFire(*this);

	m_speed.min = 10.0f;
	m_speed.max = 100.0f;
	m_size.min = 100.0f;
	m_size.max = 200.0f;
	m_opacity.min = 0.3f;
	m_opacity.max = 0.6f;
	
	m_obj3d = new C3DObject();

	SetMaxParticles(10);
	m_lastUpdateTime = g->ne.time;
	g->scm.GetActiveScene()->AddParticles(this);
}

nrmParticles::~nrmParticles()
{	
	g->scm.GetActiveScene()->RemoveParticles(this);
	if (m_obj3d) {
		delete m_obj3d;
		m_obj3d = NULL;
	}
	if (m_vbo)
		delete m_vbo;
	if (m_dynamic)
		delete m_dynamic;
	m_totalParticles -= m_countLived;
}

CShader* nrmParticles::GetShader()
{	
	int shaderID = g->sm.GetObjectNumber("shaders/particles");	
	CShader* shader = g->sm.GetObjectPointer(shaderID);

	static bool uniformsSetted = false;
	if (!uniformsSetted) {
		shader->Bind(0);
		shader->SetUniformTexture("mainMap", 0);
		shader->Unbind();
		uniformsSetted = true;
	}
	return shader;
}

void nrmParticles::Draw()
{
	if (m_useShader != g->rs.GetBool(SHADERS_USING)) {
		m_useShader = g->rs.GetBool(SHADERS_USING);
		RecreateVBO();
	}

	g->stp.PushMatrix();
	float x, y, z;
		
	m_obj3d->GetCoords(&x, &y, &z);
	GLFUNC(glTranslatef)(x, y, z);

	m_obj3d->GetRotation()->ApplyRotation();

	m_obj3d->GetScale(&x, &y, &z);
	GLFUNC(glScalef)(x, y, z);
	
	g->stp.SetActiveTexture(0);
	g->tm.BindTexture(m_textureId);

	if (m_useShader) {
		m_shader->Bind(0);
		m_shader->SetUniformFloat("vpHeight", g->stp.GetCurrentHeight());
	} else {
		GLFUNC(glPointSize)(m_defPointSize);
	}

	m_vbo->PreRender();
	m_vbo->RenderRange(0, m_countLived);
	m_vbo->PostRender();

	if (m_useShader) {
		m_shader->Unbind();
	}

	g->stp.PopMatrix();
}

void nrmParticles::RecreateVBO() {
	if (m_vbo)
		delete m_vbo;	

	m_vbo = new CBaseBufferObject();
	m_vbo->SetPrimitiveType(GL_POINTS);
	m_vbo->SetUpdateType(UPDATE_TYPE_DYNAMIC_VERTEX_NORMAL);

	if (m_useShader) {
		IBaseBufferObject::VertexAttribDesc attribDesc[] = {
			{m_shader->GetAttribLocation("aPosition"), 3, GL_FLOAT, offsetof(nrmParticles::ParticleVertex, pos)},
			{m_shader->GetAttribLocation("aSize"), 1, GL_FLOAT, offsetof(nrmParticles::ParticleVertex, size)},
			{m_shader->GetAttribLocation("aOpacity"), 1, GL_FLOAT, offsetof(nrmParticles::ParticleVertex, opacity)}
		};

		m_vbo->SetVertexDescSize(sizeof(nrmParticles::ParticleVertex));	
		m_vbo->SetAttribDesc(attribDesc, sizeof(attribDesc) / sizeof(attribDesc[0]));
	} else {
		m_vbo->SetVertexDimension(3);
	}
	SetVBO(&m_ptVert.front(), m_maxParticles);
}

void nrmParticles::SetVBO(const ParticleVertex* verts, int count) {
	if (m_useShader) {
		m_vbo->SetVertexArray(verts, count);
	} else {
		CVector3D* v = new CVector3D[count];
		for (int i = 0; i < count; ++i) {
			v[i] = verts[i].pos;
		}
		m_vbo->SetVertexArray(v, count);
		delete [] v;
	}
}

void nrmParticles::UpdateDistToCamera()
{
	CCamera3D* camera = g->cm.GetActiveCamera();
	if (!camera) {
		m_distSq = 0.0f;
		return;
	}

	CVector3D p;
	m_obj3d->GetCoords(&p.x, &p.y, &p.z);
	m_distSq = (p - camera->GetEyePosition()).GetLengthSq();
}

void nrmParticles::CalcCameraLocalPosition()
{
	CCamera3D* camera = g->cm.GetActiveCamera();
	if (!camera) {
		m_cameraLocalPos = CVector3D(0, 0, 0);
		return;
	}

	m_cameraLocalPos = camera->GetEyePosition();
	CVector3D v;
	m_obj3d->GetScale(&v.x, &v.y, &v.z);
	m_cameraLocalPos.x /= v.x;
	m_cameraLocalPos.y /= v.y;
	m_cameraLocalPos.z /= v.z;

	CQuaternion q = m_obj3d->GetRotation()->GetAsDXQuaternion();
	q.Conjugate();
	m_cameraLocalPos *= q;	

	m_obj3d->GetCoords(&v.x, &v.y, &v.z);
	m_cameraLocalPos -= v;
}

//сортировка по удаленности от камеры
void nrmParticles::SortByDist(std::vector<nrmParticles::ParticleDesc>::iterator itDesc,
	std::vector<nrmParticles::ParticleVertex>::iterator itVert)
{
	itDesc->dist = (m_cameraLocalPos - itVert->pos).GetLengthSq();
	std::vector<ParticleDesc>::iterator itDesc2 = m_ptDesc.begin();
	std::vector<ParticleVertex>::iterator itVert2 = m_ptVert.begin();
	for (; itDesc2 != itDesc; ++itDesc2) 
	{
		if (itDesc2->dist > itDesc->dist) {
			std::swap(*itDesc, *itDesc2);
			std::swap(*itVert, *itVert2);
		}
		++itVert2;
	}
}

void nrmParticles::Update(DWORD t)
{
	float dt = (float)(t - m_lastUpdateTime)/1000;
	m_lastUpdateTime = t;
	

	m_mustBeParticlesCount = calcMustBeParticlesCount();
	m_size.min = calcParticleSizeLimit(m_sizeCrude.min);
	m_size.max = calcParticleSizeLimit(m_sizeCrude.max);

	int mustBorn = m_mustBeParticlesCount - m_countLived;
	if (m_sortVerticles)
		CalcCameraLocalPosition();

	IParticlesDynamic & dynamic = *m_dynamic;
	std::vector<ParticleDesc>::iterator itDesc = m_ptDesc.begin();
	std::vector<ParticleVertex>::iterator itVert = m_ptVert.begin();
	for (; itDesc != m_ptDesc.end(); ++itDesc) 
	{
		ParticleDesc & desc = *itDesc;
		ParticleVertex & vert = *itVert;
		if (desc.isLive) {
			dynamic.dynamic(desc, vert, dt);
			dynamic.dead(desc, vert, dt);
			if (!desc.isLive) {
				--m_countLived;
				--m_totalParticles;
				++mustBorn;				
				if (itDesc - m_ptDesc.begin() < m_countLived) { //сдвигаем умершие частицы в конец
					std::swap(desc, m_ptDesc[m_countLived]);
					std::swap(vert, m_ptVert[m_countLived]);
				}				
			} else {
				if (m_sortVerticles)
					SortByDist(itDesc, itVert);
			}
		} else if (mustBorn > 0) {
			dynamic.alive(desc, vert, dt);
			if (desc.isLive) {
				dynamic.emit(desc, vert, dt);
				if (itDesc - m_ptDesc.begin() > m_countLived) { //сдвигаем ожившие частицы в начало
					std::swap(desc, m_ptDesc[m_countLived]);
					std::swap(vert, m_ptVert[m_countLived]);
				}
				if (m_sortVerticles)
					SortByDist(itDesc, itVert);
				++m_countLived;
				++m_totalParticles;
				--mustBorn;
			}
		}
		++itVert;
	}

	SetVBO(&m_ptVert.front(), m_countLived);
}

int nrmParticles::calcMustBeParticlesCount() const 
{
	float k = 1.0f;
	float max = g->rs.GetInt(MAX_PARTICLES_COUNT);
	if (max < m_totalParticles)
		k = max / m_totalParticles;
	int count = m_density * k * m_maxParticles;
	return count;
}

float nrmParticles::calcParticleSizeLimit(float size) const 
{
	float sizeLimit = g->rs.GetInt(PARTICLE_LIMIT_SIZE);
	
	if (size > sizeLimit) {
		float countParts1 = g->rs.GetInt(MAX_PARTICLES_TO_ENABLE_LIMIT_SIZE);
		float countParts2 = g->rs.GetInt(MAX_PARTICLES_COUNT);
		float k = (m_totalParticles - countParts1) / (countParts2 - countParts1);
		k = max(0, min(1, k));
		return size + (sizeLimit - size) * k;
	} else {
		return size;
	}
}

// реализация  moILoadable
bool nrmParticles::SrcChanged() 
{
	if (!mpMLMedia)
	{
		return false;
	}

	const wchar_t* src = mpMLMedia->GetILoadable()->GetRawSrc();
	if (!src) 
	{
		return false;	
	}

	int id = g->tm.GetObjectNumber(src);
	if (id == ERR_OBJECT_NOT_FOUND) {
		return false;
	}
	
	m_textureId = id;
	GetMLMedia()->GetILoadable()->onLoad( RMML_LOAD_STATE_SRC_LOADED);
	return true;
}

// реализация moIParticles
void nrmParticles::SetMaxParticles(int count) 
{
	assert(count >= 0);	

	int lastMaxParticles = m_maxParticles;
	m_maxParticles = count;
	m_ptDesc.resize(count);
	m_ptVert.resize(count);
	if (m_maxParticles > lastMaxParticles)
	{		
		RecreateVBO();
	} else if (m_countLived > m_maxParticles) 
	{
		m_countLived = m_maxParticles;
	}
}

int nrmParticles::GetMaxParticles() const {
	return m_maxParticles;
}

void nrmParticles::SetDensity(float density) {
	assert(density >= 0.0f && density <= 1.0f);

	m_density = density;
}

float nrmParticles::GetDensity() const {
	return m_density;
}

void nrmParticles::SetMinSpeed(float speed) {
	m_speed.min = speed;
}

float nrmParticles::GetMinSpeed() const {
	return m_speed.min;
}

void nrmParticles::SetMaxSpeed(float speed) {
	m_speed.max = speed;
}

float nrmParticles::GetMaxSpeed() const {
	return m_speed.max;
}

void nrmParticles::SetMinSize(float size) {
	m_sizeCrude.min = size;
}

float nrmParticles::GetMinSize() const {
	return m_sizeCrude.min;
}

void nrmParticles::SetMaxSize(float size) {
	m_sizeCrude.max = size;
}

float nrmParticles::GetMaxSize() const {
	return m_sizeCrude.max;
}

void nrmParticles::SetMinOpacity(float opacity) {
	m_opacity.min = opacity;
}

float nrmParticles::GetMinOpacity() const {
	return m_opacity.min;
}

void nrmParticles::SetMaxOpacity(float opacity)	{
	m_opacity.max = opacity;
}

float nrmParticles::GetMaxOpacity() const	{
	return m_opacity.max;
}

bool nrmParticles::SetType(const char* name){
	IParticlesDynamic * newDynamic;
	if (!strcmp(name, "SmokeOfFire")) {
		newDynamic = new ParticleDynamic_SmokeOfFire(*this);
	} else 
	if (!strcmp(name, "SingleSmoke")) {
		newDynamic = new ParticleDynamic_SingleSmoke(*this);
	} else {
		return false;
	}

	if (m_dynamic)
		delete m_dynamic;
	m_dynamic = newDynamic;
	return true;
}

void nrmParticles::SetSortVerts(bool sort) {
	m_sortVerticles = sort;
}

void nrmParticles::SetDirChangeFactor(float factor) {
	m_dirChangeFactor = factor;
}

float nrmParticles::GetDirChangeFactor() const {
	return m_dirChangeFactor;
}

void nrmParticles::SetSpeedChangeFactor(float factor) {
	m_speedChangeFactor = factor;
}

float nrmParticles::GetSpeedChangeFactor() const {
	return m_speedChangeFactor;
}

void nrmParticles::SetDefPointSize(float size) {
	m_defPointSize = size;
}

float nrmParticles::GetDefPointSize() const {
	return m_defPointSize;
}