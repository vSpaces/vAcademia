#pragma once
#include "nrm3DObject.h"

class nrmParticles :
	public nrm3DObject,
	public ISceneObjectParticles,
	public moIParticles
{
friend class IParticlesDynamic;
friend class ParticleDynamic_SmokeOfFire;
friend class ParticleDynamic_SingleSmoke;

public:
	template<typename T>
	struct Limit {
		T min;
		T max;
	};

	nrmParticles(mlMedia* apMLMedia);
	virtual ~nrmParticles();

	void Update(DWORD t);	

//реализация ISceneObjectParticles
	virtual void Draw();
	virtual float GetDistSq() { return m_distSq; };
	virtual void UpdateDistToCamera();

public:	
// реализация  moMedia
	virtual moIParticles* GetIParticles(){ return this; }

// реализация  moILoadable
	virtual bool SrcChanged();

// реализация moIParticles
	virtual void SetMaxParticles(int count);
	virtual int GetMaxParticles() const;
	virtual void SetDensity(float density);
	virtual float GetDensity() const;
	virtual void SetMinSpeed(float speed);
	virtual float GetMinSpeed() const;
	virtual void SetMaxSpeed(float speed);
	virtual float GetMaxSpeed() const;
	virtual void SetMinSize(float size);
	virtual float GetMinSize() const;
	virtual void SetMaxSize(float size);
	virtual float GetMaxSize() const;
	virtual void SetMinOpacity(float opacity);
	virtual float GetMinOpacity() const;
	virtual void SetMaxOpacity(float opacity);
	virtual float GetMaxOpacity() const;
	virtual void SetDirChangeFactor(float factor);
	virtual float GetDirChangeFactor() const;
	virtual void SetSpeedChangeFactor(float factor);
	virtual float GetSpeedChangeFactor() const;
	virtual bool SetType(const char* name);
	virtual void SetSortVerts(bool sort);
	virtual void SetDefPointSize(float size);
	virtual float GetDefPointSize() const;

protected:
	struct ParticleDesc {
		CVector3D dir;
		float speed;
		float sizeInitial;
		float opacityInitial;
		float dist;
		float time;
		int state;
		bool isLive;
		bool wasLive;

		ParticleDesc() : 
			dir(CVector3D(0, 0, 0)), 
			speed(0), 
			sizeInitial(0), 
			opacityInitial(0),
			dist(0),
			time(0),
			state(0),
			isLive(false),
			wasLive(false)
		{}
	};

	struct ParticleVertex {
		CVector3D pos;
		float size;
		float opacity;

		ParticleVertex() :
			pos(CVector3D(0, 0, 0)),
			size(0),
			opacity(0)
		{}
	};

	void RecreateVBO();
	void CalcCameraLocalPosition();
	void SortByDist(std::vector<ParticleDesc>::iterator itDesc, std::vector<ParticleVertex>::iterator itVert);
	static CShader* GetShader();
	int calcMustBeParticlesCount() const;
	float calcParticleSizeLimit(float size) const;
	void SetVBO(const ParticleVertex* verts, int count);

	IParticlesDynamic * m_dynamic;
	CBaseBufferObject* m_vbo;
	int m_textureId;
	CShader* m_shader;
	long long m_lastUpdateTime;
	CVector3D m_cameraLocalPos;

	std::vector<ParticleDesc> m_ptDesc;
	std::vector<ParticleVertex> m_ptVert;
		
	int m_countLived;
	int m_maxParticles;
	float m_density;
	Limit<float> m_speed;
	Limit<float> m_size;
	Limit<float> m_sizeCrude;
	Limit<float> m_opacity;
	float m_dirChangeFactor;
	float m_speedChangeFactor;
	float m_distSq;
	bool m_sortVerticles;
	int m_mustBeParticlesCount;
	float m_particleSizeLimit;
	float m_defPointSize;
	bool m_useShader;

	static int m_totalParticles;
};