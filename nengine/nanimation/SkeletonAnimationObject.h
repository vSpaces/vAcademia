
#pragma once

#include "CommonHeader.h"
#include "cal3d\PointController.h"
#include "cal3d\LookController.h"
#include "KinectVideoTexture.h"
#include "IDestroyListener.h"
#include "KinectController.h"
#include "Cal3DPathParser.h"
#include "MimicController.h"
#include "AnimationObject.h"
#include "cal3d\coremodel.h"
#include "IDynamicTexture.h"
#include "ClippingVolume.h"
#include "KinectEmulator.h"
#include "IdlesPackDesc.h"
#include "cal3d\model.h"
#include "Quaternion3D.h"
#include "Vector3D.h"
#include <tinyxml.h>
#include <vector>
#include <string>
#include "IKinect.h"
#include <map>


#define MASK_FILENAME			"mask.bin"
#define MASK_FILENAMEW			L"mask.bin"
#define BUFFER_COUNT			2
#define CLIPPING_VOLUME_COUNT	10

#define MAX_SAO_TEXTURES_COUNT		8

typedef enum _SKELETON_POSITION_INDEX
{
    SKELETON_POSITION_HIP_CENTER = 0,
    SKELETON_POSITION_SPINE,
    SKELETON_POSITION_SHOULDER_CENTER,
    SKELETON_POSITION_HEAD,
    SKELETON_POSITION_SHOULDER_LEFT,
    SKELETON_POSITION_ELBOW_LEFT,
    SKELETON_POSITION_WRIST_LEFT,
    SKELETON_POSITION_HAND_LEFT,
    SKELETON_POSITION_SHOULDER_RIGHT,
    SKELETON_POSITION_ELBOW_RIGHT,
    SKELETON_POSITION_WRIST_RIGHT,
    SKELETON_POSITION_HAND_RIGHT,
    SKELETON_POSITION_HIP_LEFT,
    SKELETON_POSITION_KNEE_LEFT,
    SKELETON_POSITION_ANKLE_LEFT,
    SKELETON_POSITION_FOOT_LEFT,
    SKELETON_POSITION_HIP_RIGHT,
    SKELETON_POSITION_KNEE_RIGHT,
    SKELETON_POSITION_ANKLE_RIGHT,
    SKELETON_POSITION_FOOT_RIGHT,
    SKELETON_POSITION_COUNT
} SKELETON_POSITION_INDEX;


class CUpdateSkeletonAnimationTask;

typedef struct _SBlendOptimizationInfo
{
	int startVertex;
	int endVertex;

	_SBlendOptimizationInfo()
	{
		startVertex = -1;
		endVertex = -1;
	}
} SBlendOptimizationInfo;

typedef struct _SIdleInfo
{
	int motionID;
	int between;
	int freq;
	bool isPlaying;
	int lastTimePassed;
	__int64 startTime;
	CMotion* motion;

	_SIdleInfo()
	{
		motionID = -1;
		between = 0;
		freq = 0;
		isPlaying = false;
		lastTimePassed = -10000;
		startTime = 0;
		motion = NULL;
	}
} SIdleInfo;

typedef struct _SEvent
{
	int eventID;
	void* eventData;

	_SEvent()
	{
		eventID = 0;	// no event with such ID
		eventData = NULL;
	}
} SEvent;

typedef struct _SClipboxs
{
	int type;
	MP_STRING valueID;
	MP_STRING valueOrt;
	MP_STRING valuePlaneStart;
	MP_STRING valuePlaneEnd;
	MP_STRING valueOrt2;
	MP_STRING valuePlaneStart2;
	MP_STRING valuePlaneEnd2;
	MP_STRING valueOrt3;
	MP_STRING valuePlaneStart3;
	MP_STRING valuePlaneEnd3;

	_SClipboxs();
} SClipboxs;

typedef struct _SBlends
{
	MP_STRING src;
	MP_STRING param;
	MP_STRING target;

	CSkeletonAnimationObject* sao;

	_SBlends(std::string _src, std::string _param, std::string _target);
} SBlends;

typedef struct _SMaterialFile
{
	MP_STRING name;
	MP_STRING path;
	void* data;
	bool isNeedToDelete;

	_SMaterialFile();
	_SMaterialFile(std::string _name, std::string& _path, void* _data, bool needToDelete);
} SMaterialFile;

typedef struct _STransitionalInfo
{
	MP_VECTOR<SMaterialFile*> materialFiles;

	MP_VECTOR<MP_STRING> singleMaterials;
	MP_STRING baseMaterial;	

	SClipboxs* clipboxs[CLIPPING_VOLUME_COUNT];
	
	C3DObject* obj;

	CSkeletonAnimationObject* newSAO;

	CSkeletonAnimationObject* sao;

	MP_VECTOR<SBlends*> blends;

	void Clear();

	_STransitionalInfo();		
	~_STransitionalInfo();	
} STransitionalInfo;

class CSkeletonAnimationObjectManager;

class CSkeletonAnimationObject : public CAnimationObject, public IDestroyListener, 
								 public CBaseChangesInformer, public IChangesListener
{
friend class CSkeletonAnimationObjectManager;

public:
	CSkeletonAnimationObject();
	~CSkeletonAnimationObject();

	// Kinect support
public:
	void SetKinectSeatedMode(const bool isSeated);
	void EnableKinect();
	void DisableKinect();
	void DumpKinectInfoToLog();

	bool IsUsualOrder()const;

	void ReloadKinectSettings();

	IKinect* GetKinectObject();
	void SetKinectObject(IKinect* kinect);

	bool IsControlledByKinect();
	//  Возвращает размер данных текущего бинарного состояния кинекта
	unsigned int GetKinectSynchDataSize();
	// Должен заполнить готовый буфер data данными. size - размер буфера. Чтобы его узнать - надо вызвать GetKinectSynchDataSize
	bool  GetKinectSynchData(const unsigned char* data, int& size);
	// Установить кинекту нужное состояние
	bool  SetKinectSynchData(const unsigned char* data, unsigned int size);

	void GetKinectTrackedParts(bool& isHeadTracked, bool& isLeftArmTracked,
								bool& isRightArmTracked, bool& isLeftFootTracked,
								bool& isRightFootTracked, bool& areTwoSkeletonsFound,
								bool& areMarkersNotFound);

	bool IsKinectEnabled();

	void SetMarkerType(const int markerType);
	void EnableFaceTracking();
	void DisableFaceTracking();

private:
	void PatchKinectStateIfNeeded(const unsigned char* data, unsigned int size);
	void TrackingHands();

public:
	void StopLookAtForTime(unsigned int time);

	void SetKinectDeskPoints(CVector3D start, CVector3D end, C3DObject* obj);

	void PointByHandTo(float x, float y, float z);
	void SetLexeme(const int lexemeID, const int time = 0);

	void SetTrackingRightHand(bool isEnabled);
	CVector3D GetRightHandPosition();
	CVector3D GetRightHandDirection();
	CVector3D GetExtendedRightHandDirection();

	void SetTrackingLeftHand(bool isEnabled);
	CVector3D GetLeftHandPosition();
	CVector3D GetLeftHandDirection();
	CVector3D GetExtendedLeftHandDirection();

	int GetType()const;

	void SetFreeze(const bool isFrozen);

	void Update(const int deltaTime, const float distSq);

	void RemoveMotion(const int motionID);
	void OnDestroy(CMotion* const motion);

	void ClearAction(const unsigned int id, const float time);
	void ClearCycle(const unsigned int id, const float time);
	void BlendCycle(const unsigned int id, const float weight, const float delay, const bool isAsynch);
	void ExecuteAction(const unsigned int id, const float delayIn, const float delayOut);
	void SetAction(const unsigned int id, const float time);

	CVector3D GetBonePosition(std::string name);

	CalModel* GetCalModel()const;

	CVector3D GetFaceVector()const;
	CVector3D GetRotatedFaceVector()const ;

	int GetUpdateTime()const;

	bool BindToSkeleton(CSkeletonAnimationObject* const sao);
	void UnbindSkeleton();

	bool IsUpdated();

	std::string GetPath()const;
	std::string GetDir()const;


	CVector3D GetOffset()const;
	void GetOffset(float* const x, float* const y, float* const z);

	CQuaternion GetAnimationRotation()const;
	void GetAnimationRotation(float* const x, float* const y, float* const z, float* const w);

	void OnPosChanged();

	void SetBillboardTextureID(const int textureID);
	int GetBillboardTextureID()const;

	void BeginAsynchUpdate(CUpdateSkeletonAnimationTask* const task);
	void EndAsynchUpdate();

	bool IsContainsExternalTextures()const;

	void CopyMotionsTo(CSkeletonAnimationObject* const sao);

	void SetParent(CSkeletonAnimationObject* parent);
	CSkeletonAnimationObject* GetParent()const;

	void FreeResources();

	int GetFaceCount()const;
	int GetVertexCount()const;

	void GetUsedClipboxIDs(std::vector<int>& clipboxsId);

	void DumpMotionsInfoToLog();

	void  StartCal3DModelAccess();
	void  EndCal3DModelAccess();

	void InitLookControllerIfNeeded();
	void LookAt(CVector3D& pos, int /*time*/, float /*koef*/);
	void LookAt(C3DObject* obj, int time, float koef);

	void DisableKinectDesk();

	CalLookController * GetLookController() { return m_lookController; }

	// Бленды
public:
	void SetBlendWeight(CSkeletonAnimationObject* const sao, const float weight);
	float GetBlendWeight(CSkeletonAnimationObject* const sao)const;

	int GetCompatabilityID();

private:
	void UpdateImpl(int deltaTime, float distSq);
	void SetBlendOptimizationInfo(SBlendOptimizationInfo* const info);
	SBlendOptimizationInfo* GetBlendOptimizationInfo()const;

	void SetMask(unsigned char* const mask, const unsigned int maskSize);
	unsigned char* GetMask();
	unsigned int GetMaskSize()const;

	void UpdateBlending();
	template <int operationType> void StartBlendCycle(SBlendOptimizationInfo* const blendOptimizationInfo, const float blendWeight, CSkeletonAnimationObject* const sao);
	// -- Бленды

	// Отсекающие объемы
public:
	void DisableClippingVolume(const int volumeID);
	void SetClippingVolume(const int type, const int volumeID, const int coordID, const float startValue, 
										const float endValue,
										const int coordID2, const float startValue2, const float endValue2,
										const int coordID3, const float startValue3, const float endValue3);
	void CopyClipboxsTo(CSkeletonAnimationObject* const sao);

	void ToInitialPose();
	void FromInitialPose();

private:
	void ApplyClippingVolumeIfNeeded();

	void SetClipplingAxe(const int volumeID, const int coordID, const float startValue, const float endValue);
	// -- Отсекающие объемы

	// Фоновые движения
public:
	void AddIdle(const int motionID, const int between, const int freq);
	void RemoveIdles();

	void LockIdles();
	void UnlockIdles();

	bool IsSleeped()const;
	void SetSleepingState(const bool isSleeped);	

private:
	void PlayNextIdleMotion();
	bool IsStartApplyIdle();
	__int64 GetStartIdlesTime();

	bool CanIdleStart(const int motionID);

	void StopIdles();
	// -- Фоновые движения

	// Загрузка
public:
	void LoadAsynch(const std::string& path, const bool isBlend = false, CSkeletonAnimationObject* const parent = NULL);
	bool Load(const std::string& path, const bool isBlend = false, CSkeletonAnimationObject* const parent = NULL);
	bool Load(const std::string& path, const bool isBlend = false);

	bool LoadCal3DObjects(const std::string& path, STransitionalInfo* info, const bool isBlend, CSkeletonAnimationObject* const parent, void* data = NULL, unsigned int size = 0);
	void LoadEngineObjects(CSkeletonAnimationObject* const parent, STransitionalInfo* info);

	void LoadIdles(std::string& path);
	SIdlesPackDesc* LoadIdlesFromFile(std::string& path);

	void OnChanged(int eventID);
	void SetLoadedStatus(bool isLoaded);
	bool IsLoaded()const;
	//выставлена поза или нет
	bool IsEndApplyIdle();

	IDynamicTexture* GetVideoDynamicTexture();
	void DeleteDynamicVideoTexture();

	CMimicController* GetMimicController();

private:
	bool LoadSkeleton(TiXmlNode* const object);
	bool LoadMesh();
	bool LoadXML(TiXmlDocument* doc, STransitionalInfo* info);
	void LoadMaterialFiles(MP_VECTOR<MP_STRING>& singleMaterials, const std::string& baseMaterial,
		std::vector<SMaterialFile *>& materialFiles);
	bool LoadMaterials(TiXmlNode* const object, MP_VECTOR<MP_STRING>& singleMaterials,
		std::string& baseMaterial);
	void LoadEngineMaterials(std::vector<SMaterialFile *>& materialFiles);
	void LoadBlends(std::vector<SBlends*>* blends);
	void LoadMask();
	
	void Build3DModel();
	void CalculateTangents();

	bool IsPathCorrect(const std::string& path)const;

	void LinkTextures(const std::string& path);
	int LinkTexture(const std::string& _texturePath, const std::string& path, const bool canBeCompressed);

	bool IsTextureExists(const std::string& path);

	void SetClipBox(int type, std::string& valueID,std::string& valueOrt, std::string& valuePlaneStart, std::string& valuePlaneEnd,std::string& valueOrt2,std::string& valuePlaneStart2, std::string& valuePlaneEnd2, std::string& valueOrt3,std::string& valuePlaneStart3, std::string& valuePlaneEnd3);

	// -- Загрузка

	// Асинхронные события
private:
	void AddIdleByEvent(void* const eventData);
	void RemoveIdlesByEvent(void* const eventData);
	void LockIdlesByEvent(void* const eventData);
	void UnlockIdlesByEvent(void* const eventData);
	void ClearActionByEvent(void* const eventData);
	void ClearCycleByEvent(void* const eventData);
	void BlendCycleByEvent(void* const eventData);
	void ExecuteActionByEvent(void* const eventData);
	void SetActionByEvent(void* const eventData);
	int AssignMotionByEvent(void* const eventData);

	void SaveEvent(const int eventID, void* const eventData);
	void PerformEvents();

	MP_VECTOR<SEvent *> m_events;
	// -- Асинхронные события

private:
	int AssignMotion(const int motionID);

	void AdjustSkeletonTransformation();
	void ExecuteActionImpl(const unsigned int id, const float delayIn, const float delayOut);

	MP_MAP<CSkeletonAnimationObject*, float> m_blends;
	MP_VECTOR<SIdleInfo *> m_idles;
	MP_VECTOR<SIdleInfo *> m_lockedIdles;
	MP_VECTOR<int> m_motions;
	MP_MAP<int, int> m_motionsList;
	MP_VECTOR<int> m_clipboxsId;

	CBaseBufferObject* m_vbo[BUFFER_COUNT];	

	CKinectVideoTexture* m_kinectVideoTexture;

	CalModel* m_calModel;
	CalCoreModel* m_calCoreModel;
	CalLookController* m_lookController;
	C3DObject* m_lookTarget;

	CSkeletonAnimationObject* m_parent;
	C3DObject* m_parentObj;

	CVector3D m_offset;

	float* m_savedVertices; 
	unsigned short* m_savedIndices;
	unsigned int m_savedFaceCount;

	unsigned char* m_mask; 

	MP_STRING m_path;
	MP_STRING m_dir;
	MP_STRING m_baseTexturesPath;

	unsigned int m_maskSize;

	unsigned int m_currentVBONum;

	unsigned int m_compatabilityID;
	unsigned int m_motionCompatabilityID;

	unsigned int m_billboardTextureID;

	unsigned int m_vertexCount;
	unsigned int m_faceCount;

	int m_lastMotionID;
	int m_lastSetAnimationID;

	int m_zOffset;

	SClippingVolume m_volumes[CLIPPING_VOLUME_COUNT];
	
	CMimicController* m_mimicController;

	SBlendOptimizationInfo* m_blendOptimizationInfo;

	bool m_isBlend;
	bool m_isFrozen;
	bool m_isLoaded;
	bool m_isUpdated;
	bool m_isSleeped;
	bool m_isUsualOrder;
	bool m_isPosChanged;
	bool m_isInitialized;
	bool m_isNeedToUpdate;
	bool m_isIdlesPlaying;
	bool m_isStartApplyIdle;
	bool m_isEndApplyIdle;
	bool m_isClippingChanged;
	bool m_isNeedUpdateBuffers;
	bool m_isNeedUpdateBlending;
	bool m_isContainsExternalTextures;
	unsigned int m_kinectVersionTemp;

	int m_endStopLookAtTime;

	__int64 m_startIdlesTime;

	void* m_xml;

	CVector3D m_lookAtPos;

	CQuaternion m_quat;
	CQuaternion m_oldQuat;

	CRITICAL_SECTION m_assignMotionCS;
	CRITICAL_SECTION m_calModelAccessCS;

	CUpdateSkeletonAnimationTask* m_updatingTask;

	CalPointController* m_pointController;	

	MP_MAP<MP_STRING, MP_STRING> m_texturesPath;//список отдельных текстур одежды из папки другой одежды	

	int m_leftEyePoseID;
	int m_rightEyePoseID;

	IKinect* m_kinectObj;
	CKinectController* m_kinectController;
	CKinectEmulator* m_kinectEmulator;
	bool m_isKinectActive;

	bool m_isTrackingRightHand;
	bool m_isTrackingLeftHand;

	CVector3D m_rightHandPosition;
	CVector3D m_leftHandPosition;

	CVector3D m_rightHandDirection;
	CVector3D m_leftHandDirection;

	CVector3D m_rightHandDirectionExtended;
	CVector3D m_leftHandDirectionExtended;

	CCal3DPathParser m_pathObject;
};

	