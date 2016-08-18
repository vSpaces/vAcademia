
#ifndef rmml_h___
#define rmml_h___

#include <memory>
#include <string>
#include <vector>
#include <list>
#include "BSSCCode.h"

#include "..\..\nmemoryprofiler\IMemoryProfiler.h"
#include "..\..\nmemoryprofiler\memoryprofilerFuncs.h"
#include "..\..\nmemoryprofiler\memoryprofilerDefines.h"
#include <XInput.h>

#ifndef CSCL
#include "oms/oms_context.h"

namespace mme{
	class Updatable;
}
using namespace mme;
#endif

using oms::omsresult;
using oms::omsContext;

struct ZONE_OBJECT_INFO;
                                             
namespace rmml{

struct syncObjectState;

#ifndef WIN32
#define RMML_NO_VTABLE
#else
#define RMML_NO_VTABLE __declspec(novtable)
//#define RMML_NO_VTABLE
#endif

// Для импорта/экспорта классов и процедур
#if defined (RMML_BUILD)
#  define RMML_DECL  __declspec(dllexport)
#else
# define RMML_DECL   __declspec(dllimport)
#endif

#include "mlSynchData.h"

#define MLWORD unsigned short
#define MLBYTE unsigned char
#define MLDWORD unsigned long
#define MLUSHORT unsigned short
#define MLULONG unsigned long
#define MLLONG long

/**
 * Структурные типы данных
 */
struct mlPoint{
	int x,y;
};
#define ML_INIT_POINT(pnt) pnt.x=0; pnt.y=0;
struct mlSize{
	int width,height;
};
#define ML_INIT_SIZE(sz) sz.width=0; sz.height=0;
struct mlRect{
	int left,top,right,bottom;
};
#define ML_INIT_RECT(rc) rc.left=0; rc.top=0; rc.right=0; rc.bottom=0;
struct mlBounds{
	int x, y, width, height;
};
#define ML_INIT_BOUNDS(bnd) bnd.x = 0; bnd.y = 0; bnd.width = 0; bnd.height = 0;
struct ml3DPosition{
	double x,y,z;
};
#define ML_INIT_3DPOSITION(pos) {(pos).x=0.0; (pos).y=0.0; (pos).z=0.0;}
#define ML_COPY_3DPOSITION(a, b) {a.x = b.x; a.y = b.y; a.z = b.z;}
struct ml3DScale{
	double x,y,z;
};
#define ML_INIT_3DSCALE(scl) {(scl).x=0.0; (scl).y=0.0; (scl).z=0.0;}
struct ml3DRotation{
	double x,y,z,a; // кватернион 
};
#define ML_INIT_3DROTATION(rot) {(rot).x=0.0; (rot).y=0.0; (rot).z=1.0; (rot).a=0.0;}
struct ml3DPlane{
	double x,y,z,d;
};
#define ML_INIT_3DPLANE(plane) {(plane).x=0.0; (plane).y=0.0; (plane).z=1.0; (plane).d=0.0;}
struct ml3DBounds{
	double xMin, xMax, yMin, yMax, zMin, zMax;
};
#define ML_INIT_3DBOUNDS(bnd) {(bnd).xMin=0.0; (bnd).xMax=0.0; (bnd).yMin=0.0; (bnd).yMax=0.0; (bnd).zMin=0.0; (bnd).zMax=0.0;}
struct mlColor{
	unsigned char r,g,b,a;
};
#define ML_INIT_COLOR(col) {(col).r=0; (col).g=0; (col).b=0; (col).a=0xFF;}

typedef std::wstring mlString;

/**
 * Медиа-типы
 */
#define MLMT_UNDEFINED 0
#define MLMPT_LOADABLE (1<<(8+0))
#define MLMPT_VISIBLE (1<<(8+1))
#define MLMPT_AUDIBLE (1<<(8+2))
#define MLMPT_CONTINUOUS (1<<(8+3))
#define MLMPT_BUTTON (1<<(8+4))
#define MLMPT_3DOBJECT (1<<(8+5))
#define MLMT_VISIBLE (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
	                MLMPT_BUTTON | \
					)
// Изображение
#define MLMT_IMAGE (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
	                MLMPT_BUTTON | \
					1 \
					)
// Анимация
#define MLMT_ANIMATION (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
	                MLMPT_CONTINUOUS | \
	                MLMPT_BUTTON | \
					2 \
					)
// Видео
#define MLMT_VIDEO (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
	                MLMPT_CONTINUOUS | \
	                MLMPT_BUTTON | \
					3 \
					)
// Текст
#define MLMT_TEXT (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
	                MLMPT_BUTTON | \
					4 \
					)
//#define MLMT_HTML (MLMPT_LOADABLE | \
//	                MLMPT_VISIBLE | \
//	                MLMPT_BUTTON | \
//					5 \
//					)
#define MLMT_BUTTON (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
	                MLMPT_BUTTON | \
					6 \
					)
#define MLMT_ACTIVEX (MLMPT_LOADABLE | \
	                MLMPT_VISIBLE | \
					7 \
					)
#define MLMT_PLUGIN (MLMPT_LOADABLE | \
					8 \
					)
#define MLMT_AUDIO (MLMPT_LOADABLE | \
	                MLMPT_AUDIBLE | \
	                MLMPT_CONTINUOUS | \
					1 \
					)
// XML-документ
#define MLMT_XML   (MLMPT_LOADABLE | \
					1 \
					)
// Демонстрационное окно
#define MLMT_VIEWPORT ( MLMPT_VISIBLE | \
					20 \
					) 
// 3D сцена
#define MLMT_SCENE3D ( MLMPT_LOADABLE | \
					21 \
					) 
// Камера
#define MLMT_CAMERA ( MLMPT_LOADABLE | \
					MLMPT_3DOBJECT | \
					22 \
					) 
// Освещение
#define MLMT_LIGHT ( MLMPT_LOADABLE | \
					MLMPT_3DOBJECT | \
					23 \
					) 
// 3D-объект
#define MLMT_OBJECT ( MLMPT_LOADABLE | \
					MLMPT_3DOBJECT | \
					24 \
					) 
// Движение 3D-объекта
#define MLMT_MOTION ( MLMPT_LOADABLE | \
					25 \
					) 
// 3D-персонаж
#define MLMT_CHARACTER ( MLMPT_LOADABLE | \
					MLMPT_3DOBJECT | \
					26 \
					) 
// Речь 3D-персонажа
#define MLMT_SPEECH ( MLMPT_LOADABLE | \
					27 \
					) 
// Группа 3D-объектов
#define MLMT_GROUP  ( MLMPT_LOADABLE | \
					MLMPT_3DOBJECT | \
					28 \
					)
// "Облако" 3D-персонажа 
#define MLMT_CLOUD  ( MLMPT_LOADABLE | \
					29 \
					) 
// Виземы
#define MLMT_VISEMES ( MLMPT_LOADABLE | \
					30 \
					) 
// Подсказка
#define MLMT_HINT   ( MLMPT_LOADABLE | \
					31 \
					) 
// Путь для 3D-объектов
#define MLMT_PATH3D ( MLMPT_LOADABLE | \
					32 \
					) 
// Карта для 3D-объектов
#define MLMT_MAP	( \
					33 \
					) 
// Материал для 3D-объектов
#define MLMT_MATERIAL ( MLMPT_LOADABLE | \
					34 \
					) 
// Материалы для 3D-объектов
#define MLMT_MATERIALS ( MLMPT_LOADABLE | \
					35 \
					) 

// Флеш объект
#define MLMT_FLASH ( MLMPT_LOADABLE |   \
					MLMPT_VISIBLE | \
					MLMPT_CONTINUOUS | \
					36 \
					)

// Browser объект
#define MLMT_BROWSER ( MLMPT_LOADABLE | \
					MLMPT_VISIBLE | \
					37 \
					)
// QuickTime
#define MLMT_QT		(MLMPT_LOADABLE | \
					MLMPT_VISIBLE | \
					MLMPT_CONTINUOUS | \
					38 \
					)

// Input-элемент для ввода текста
#define MLMT_INPUT ( MLMPT_VISIBLE | \
	                MLMPT_BUTTON | \
					39 \
					)

// Рамка из изображения
#define MLMT_IMAGE_FRAME (MLMPT_LOADABLE | \
					MLMPT_VISIBLE | \
					MLMPT_BUTTON | \
					40 \
					)
// 3D-объект зона общения
#define MLMT_COMMINICATION_AREA ( MLMPT_LOADABLE | \
				MLMPT_3DOBJECT | \
					41 \
					)
// Изображение
#define MLMT_LINE ( \
					MLMPT_VISIBLE | \
					MLMPT_BUTTON | \
					42 \
					)
// Частицы
#define MLMT_PARTICLES (MLMPT_LOADABLE | \
					MLMPT_3DOBJECT | \
					43 \
					)

// Композиция
#define MLMT_COMPOSITION	(MLMPT_VISIBLE | \
					MLMPT_BUTTON | \
					250 \
					)

#if (defined(RMML_BUILD) || defined(RMML_DEBUG))
// Внутренние типы RMML-элементов 
// (используются только SceneManager-ом и Debugger-ом)

#define MLMPT_ELEMENT	(1<<(8+7))
#define MLMPT_CUSTOM	(MLMPT_ELEMENT-1)

// Ресурс
#define MLMT_RESOURCE		( MLMPT_LOADABLE | \
							242 \
							) 

// Тень 3D-объекта
#define MLMT_SHADOW3D		( \
							243 \
							) 
// Тени 3D-объектов
#define MLMT_SHADOWS3D		( \
							244 \
							) 
// Параметры idle-ового движения
#define MLMT_IDLE			( \
							245 \
							) 
// Набор idle-овых движений
#define MLMT_IDLES			( \
							246 \
							) 
// Параметры ходьбы 3D-персонажа
#define MLMT_MOVEMENT		( \
							247 \
							) 
// Стили текста
#define MLMT_STYLES			(MLMPT_LOADABLE | \
							248 \
							)
// Установка последовательности 
#define MLMT_SEQUENCER		(MLMPT_CONTINUOUS | \
							249 \
							)
// Сцена
#define MLMT_SCENE			(MLMT_COMPOSITION+1)
#define MLMT_SCENECLASS		(MLMT_COMPOSITION+2)
// Скрипт
#define MLMT_SCRIPT			(255)

// дополнительные типы скрипта (event, watch, event expression)
#define MLST_EVENT  (MLMT_SCRIPT | (1<<(8+0)))
#define MLST_WATCH  (MLMT_SCRIPT | (1<<(8+1)))
#define MLST_SYNCHMS  (MLMT_SCRIPT | (1<<(8+2)))
#define MLST_EVEXPR  (MLMT_SCRIPT | (1<<(8+3)))
#define MLST_SYNCH  (MLMT_SCRIPT | (1<<(8+4)))
#define MLST_SYNCHPROP  (MLMT_SCRIPT | (1<<(8+5)))
#define MLST_SYNCHEVENT	(MLMT_SCRIPT | (1<<(8+6)))
#define MLSTF_NOTSET  ((1<<(8+7)))
#endif

enum {
	MLPT_UNKNOWN,
	MLPT_NULL,
	MLPT_INT,
	MLPT_BOOLEAN,
	MLPT_STRING,
	MLPT_DOUBLE,
	MLPT_POINT,
	MLPT_SIZE,
	MLPT_RECT,
	MLPT_3DPOS,
	MLPT_3DSCL,
	MLPT_3DROT,
	MLPT_FUNCTION,
	MLPT_ELEMREF,
	MLPT_JSOREF,
	MLPT_LIMIT,
	MLPT_DWORDARGB
};

#define MLCF_COLLISION_BEGIN 1
#define MLCF_COLLISION_END   2

class mlMedia;
class moMedia;

/**
 * Класс используется в методах интерфейсов, когда надо вернуть строку.
 * (т.к. если просто подсунуть ссылку на std::wstring, то выделиться память в 
 * куче другой dll-ки, а будет пытаться удалить в куче этой dll-ки и упадет)
 */
#pragma warning( disable : 4251 )
class RMML_DECL mlOutString{
public:
	mlOutString():
		MP_WSTRING_INIT(mstr)
	{
	}

	mlOutString(const mlOutString& other):
		MP_WSTRING_INIT(mstr)
	{
		mstr = other.mstr;
	}

	MP_WSTRING mstr;
	mlOutString& operator=(const wchar_t* apwc);
	mlOutString& operator=(const char* apsz);
	mlOutString& operator=(mlString as);
};

/**
 * Интерфейс всех RMML-элементов
 */
struct RMML_NO_VTABLE mlIElement{

};

/**
 * Интерфейс всех загружаемых RMML-элементов
 */
#define	RMML_LOAD_STATE_SRC_LOADED				0
#define	RMML_LOAD_STATE_SRC_LOADING				2
#define	RMML_LOAD_STATE_ALL_RESOURCES_LOADED	1
#define	RMML_LOAD_STATE_LOADERROR				-1

enum ELoadingPriority
{
	ELP_UI = 0,	// максимальный приоритет
	ELP_ShowNow = 1,
	ELP_ShowLater = 2,
	ELP_Garbage = 3,
	ELP_UI_Visible = 4,
	ELP_XML = 5,
	ELP_BACKGROUND_SOUND = 6,
	ELP_Last
};

struct RMML_NO_VTABLE mlILoadable{
	// Свойства
	virtual const wchar_t* GetRawSrc() const= 0;
	virtual const wchar_t* GetSrc() = 0;
	virtual ELoadingPriority GetLoadingPriority() const = 0;
	virtual bool IsStreaming() = 0;
	virtual bool IsLoading() = 0;
	// События
	/* aiState: 0 - загружен файл, указанный в src,
				2 - начался процесс загружки файла, указанный в src,
				1 - загружены все ресурсы объекта (например загрузились все объекты внутри группы),
				-1 - возникла ошибка 
	*/
	virtual void onLoad(int aiState) = 0;
	virtual void onUnload() = 0;
	virtual void onLoading(int amiLoadedBytes, int amiTotalBytes) = 0;
	//
};

/**
 * Интерфейс всех загружаемых медиа-объектов
 */
struct RMML_NO_VTABLE moILoadable{
	// Свойства
	virtual bool SrcChanged()=0; // загрузка нового ресурса
	virtual bool Reload()=0; // перегрузка ресурса по текущему src
	virtual void LoadingPriorityChanged( ELoadingPriority aPriority)=0; // смена приоритета загрузки ресурса
};

struct RMML_NO_VTABLE mlIVisibleNotifyListener{
	// изменилось визуальное представление видимого элемента
	virtual void VisualRepresentationChanged(mlMedia* map)=0;
	// видимый элемент уничтожился и больше от него уведомлений не будет
	virtual void VisibleDestroyed(mlMedia* map)=0;
};

/**
 * Интерфейс всех видимых RMML-элементов
 */
struct RMML_NO_VTABLE mlIVisible{
	// Свойства
	virtual mlPoint GetAbsXY()=0; // получить абсолютное положение (с учетом положения родителей)
	virtual void SetSize(mlSize asz)=0; 
	virtual mlSize GetSize()=0; 
	virtual int GetAbsDepth()=0; // получить абсолютную глубину (с учетом глубины родителей)
	virtual bool GetAbsVisible()=0; // получить абсолютное значение видимости (с учетом видимости родителей)
	virtual float GetAbsOpacity()=0; //получить непрозрачность
	// События
	//
	virtual void Show()=0;
	virtual void Hide()=0;
	//
	//virtual bool IsVisualRepresentationChanged()=0;
	// установить слушателя уведомлений видимых элементов
	virtual void SetVListener(mlIVisibleNotifyListener* apListener)=0;
	// удалить слушателя уведомлений видимых элементов
	virtual void RemoveVListener(mlIVisibleNotifyListener* apListener)=0;

	virtual mlMedia* GetMLMedia()=0;
};

/**
 * Интерфейс всех видимых медиа-объектов
 */
struct RMML_NO_VTABLE moIVisible{
	// Свойства
	virtual void AbsXYChanged()=0; // изменилось абсолютное положение 
	virtual void SizeChanged()=0;
	virtual void AbsDepthChanged()=0; // изменилась абсолютная глубина
	virtual void AbsVisibleChanged()=0; // изменилась абсолютная видимость
	virtual void AbsOpacityChanged()=0; // изменилась абсолютная видимость
	// Прочие
	virtual mlSize GetMediaSize()=0; 
	virtual void Crop(short awX, short awY, MLWORD awWidth, MLWORD awHeight)=0; 
	virtual mlRect GetCropRect()=0; 
	virtual void SetMask(mlMedia* aMask, int aiX=0, int aiY=0)=0;
	virtual void SetMask(int aiX, int aiY, int aiW, int aiH)=0;
	virtual int IsPointIn(const mlPoint aPnt)=0; // -1 - not handled, 0 - point is out, 1 - point is in
};

/**
 * Интерфейс всех длящихся RMML-элементов
 */
struct RMML_NO_VTABLE mlIContinuous{
	// Свойства
	virtual bool GetPlaying()=0; // получить флаг проигрывания
	virtual void SetFPS(const short aFPS)=0;	// устанавливает значение FPS
	virtual int GetFPS()=0;	// получить значение FPS для картиночной анимации
	virtual void SetCurFrame(const int aFrame)=0;	// устанавливает значение currentFrame
	virtual int	GetCurFrame()=0;	// получить номер кадра на который нужно перейти
	virtual void SetLength(const int alLength)=0;	// устанавливает значение _length (общее кол-во кадров)
	virtual int GetLength()=0;	// получить кол-во кадров картиночной анимации
	virtual void SetCurPos(unsigned long alPos)=0;	// установить currentPos
	virtual unsigned long GetCurPos()=0;	// получить currentPos (в ms), на которое нужно перемотать
//	virtual void SetDuration(const unsigned long alDur)=0;	// устанавливает значение _duration (продолжительность в ms)
	virtual long GetIADuration()=0; // получить продолжительность картиночной анимации
	virtual bool GetLoop()=0;	// получить флаг цикличности
	// События
	virtual void onEnterFrame()=0;
	virtual void onPlayed()=0;
	//
	virtual void Rewind()=0;
	virtual void Play()=0;
	virtual void Stop()=0;
};

/**
 * Интерфейс всех длящихся медиа-объектов
 */
struct RMML_NO_VTABLE moIContinuous{
	// Свойства
	virtual void PlayingChanged()=0; // изменился флаг _playing
	virtual void FrameChanged()=0; // изменился номер текущего кадра
	virtual void PosChanged()=0; // изменился currentPos
	virtual void LengthChanged(){} // изменилось количество кадров в картиночной анимации
	virtual void DurationChanged(){} // изменилась продолжительность картиночной анимации (в ms)
	virtual void FPSChanged(){} // изменился FPS картиночной анимации
	virtual unsigned long GetCurPos()=0;	// запрашивает текущее значение (продолжительность в ms)
	virtual long GetDuration()=0;	// запрашивает значение _duration (продолжительность в ms)
	// События
	//
	virtual void setCheckPoints(long* apfCheckPoints, int aiLength)=0;
	virtual long getPassedCheckPoint()=0;
};

/**
 * Интерфейс всех RMML-элементов, могущих быть кнопками
 */
struct RMML_NO_VTABLE mlIButton{
	virtual unsigned int GetKeyCode()=0;
	virtual unsigned int GetKeyStates()=0;
	virtual unsigned int GetKeyScanCode()=0;
	virtual unsigned int GetKeyUnicode()=0;
	// Свойства
	virtual bool GetAbsEnabled()=0;
	// События
	//virtual void OnPress()=0;
	//virtual void OnRelease()=0;
	//virtual void OnReleaseOutside()=0;
	//virtual void OnRollOver()=0;
	//virtual void OnRollOut()=0;

};

/**
 * Интерфейс всех медиа-объектов, могущих быть кнопками
 */
struct RMML_NO_VTABLE moIButton{
	// Свойства
	virtual void AbsEnabledChanged()=0;
	// События
	virtual void onSetFocus()=0;
	virtual void onKillFocus()=0;
	virtual void onKeyDown()=0;
	virtual void onKeyUp()=0;
};

/**
 * Интерфейс всех RMML-элементов, представляющих 3D-сцены
 */
struct RMML_NO_VTABLE mlIScene3D{
	virtual bool GetUseMapManager()=0;
};

/**
 * Интерфейс 3D-сцен
 */
struct RMML_NO_VTABLE moIScene3D{
	virtual void UseMapManagerChanged()=0;
};

struct mlIMaterial;

/**
 * Интерфейс всех RMML-элементов, представляющих 3D-объекты
 */
struct RMML_NO_VTABLE mlI3DObject{
	virtual mlMedia* GetParentGroup()=0;
	virtual mlMedia* GetScene3D()=0;
	// Свойства
	virtual bool GetVisible()=0;
	virtual bool GetInCameraSpace()=0;
	virtual bool GetCheckCollisions()=0;
	virtual int GetBoundingBox()=0;
	virtual void IntersectionChanged()=0;
	virtual mlMedia* GetShadows()=0;
	virtual mlIMaterial* GetMaterial(unsigned char auID)=0;
	virtual mlMedia* GetMotion()=0;
	virtual bool GetGravitation()=0;
	virtual bool GetIntangible()=0;
	virtual bool GetBillboard()=0;
	virtual bool GetUnpickable()=0;
	virtual mlColor GetBoundingBoxColor()=0;
	virtual mlColor GetColor()=0;	// примешиваемый ко всем материалам объекта цвет
	virtual unsigned char GetOpacity()=0; // 0..0xFF  степень непрозрачности
	virtual void SyncronizeCoords()=0; // нужно синхронизировать координаты
	// События
	virtual void onPassed()=0;
	virtual void onCollision(mlMedia** aapObjects, int aFlags, ml3DPosition endPoint, int pathPointCount, bool isStatic)=0;
	virtual void onLODChanged(double adVisionDistance)=0;
	// прочие
	virtual bool GetAbsVisible()=0;
	virtual wchar_t* GetClassName()=0;
};

/**
 * Карты материалов 3D-объектов
 */
struct RMML_NO_VTABLE mlIMap{
	// Свойства
	virtual mlMedia* GetSource()=0;
};

/**
 * Материалы 3D-объектов
 */
struct RMML_NO_VTABLE moIMaterial{
	// Свойства
	virtual mlColor GetDiffuse()=0;
//	virtual void SetDiffuse(mlColor &aColor)=0;
	virtual mlColor GetAmbient()=0;
//	virtual void SetAmbient(mlColor &aColor)=0;
	virtual mlColor GetSpecular()=0;
//	virtual void SetSpecular(mlColor &aColor)=0;
	virtual double GetPower()=0; // 0..255
//	virtual void SetPower(float auVal)=0; // >= 0
	virtual mlColor GetEmissive()=0;
//	virtual void SetEmissive(mlColor &aColor)=0;
	virtual unsigned char GetOpacity()=0; // 0 - прозрачный, 255 - непрозрачный
//	virtual void SetOpacity(unsigned char auVal)=0;
	virtual mlMedia* GetMap(const wchar_t* apwcIndex)=0;
//	virtual void SetMap(const wchar_t* apwcIndex, mlIVisible* apVisible)=0;
	//
};

struct RMML_NO_VTABLE mlIMaterial : public moIMaterial{
};

/**
 * Интерфейс всех 3D-объектов
 */
struct RMML_NO_VTABLE moI3DObject{
	// Свойства
	virtual ml3DPosition GetPos()=0; 
	virtual void PosChanged(ml3DPosition &pos)=0; // изменилось положение 
	virtual ml3DScale GetScale()=0;
	virtual void ScaleChanged(ml3DScale &scl)=0;
	virtual ml3DRotation GetRotation()=0;
	virtual void RotationChanged(ml3DRotation &rot)=0;
	virtual void VisibleChanged()=0; // изменилась видимость
	virtual void InCameraSpaceChanged()=0;
	virtual void CheckCollisionsChanged()=0; // изменился флаг проверки на пересечения
	virtual void BoundingBoxChanged()=0;
	virtual void ShadowChanged()=0;
	virtual moIMaterial* GetMaterial(unsigned auID)=0;
	virtual void MaterialsChanged()=0;
	virtual void MaterialChanged(unsigned auID)=0;
	virtual void MaterialMapChanged(unsigned auID, const wchar_t* apwcIndex)=0;
	virtual void MotionChanged()=0;
	virtual void GravitationChanged()=0;
	virtual void IntangibleChanged()=0;
	virtual void BillboardChanged()=0;
	virtual void UnpickableChanged()=0;
	virtual void BoundingBoxColorChanged()=0;
	virtual void ColorChanged()=0;
	virtual void OpacityChanged()=0;
	// Методы
	virtual void doMotion(mlMedia* apMotion)=0;
	virtual void setMotion(mlMedia* apMotion,int aiDurMS)=0;
	virtual void removeMotion(mlMedia* apMotion)=0;
		// avObjects - ссылка на указатель на массив указателей на объекты, 
		// с которыми пересекается объект (0-й элемент - указатель на объект,
		// с которым произошло последнее столкновение)
		// (массив создается и уничтожается RenderManager-ом)
		// aiLen - длина массива
		// aPos3D - положение объекта, при котором столкновения с 0-м объектом еще не было
	virtual bool getCollisions(mlMedia** &avObjects, int &aiLength, ml3DPosition &aPos3D)=0;
	virtual bool getIntersections(mlMedia** &avObjects, int &aiLength)=0;
	virtual bool getIntersectionsIn(mlMedia** &avObjects, int &aiLength)=0;
	virtual bool getIntersectionsOut(mlMedia** &avObjects, int &aiLength)=0;
	virtual void goPath(mlMedia* apPath, int aiFlags)=0;
	virtual void goPath(ml3DPosition* apPath, int aiPointCount, unsigned int auShiftTime)=0;
	virtual mlMedia* getPath()=0;
		// найти путь до указанного места
	virtual bool findPathTo(ml3DPosition &aFrom, ml3DPosition &aTo, ml3DPosition* &avPath, int &aiLength)=0;
		// установка скорости движения объекта вдоль пути (%/сек)
	virtual void setPathVelocity(float afVelocity)=0;
		// установка котрольных точек (в %-ах пути), на которых будет срабатывать
		// onPassed. getPassedCheckPoint() вернет значение контрольной точки
	virtual void setPathCheckPoints(float* apfCheckPoints, int aiLength)=0;
	virtual float getPassedCheckPoint()=0;
	virtual void setPathPercent(float afPercent)=0;
	virtual float getPathPercent()=0;
	virtual bool attachTo(mlMedia* ap3DODest,const char* apszBoneName)=0;
	virtual ml3DBounds getBounds(mlMedia* ap3DOCoordSysSrc)=0;
		// получение bounds-ов подобъекта, на который натянут материал с идентификатором aiMatID
	virtual ml3DBounds getSubobjBounds(int aiMatID)=0;
	virtual int moveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions)=0;
	virtual ml3DPosition getAbsolutePosition()=0;
	virtual ml3DRotation getAbsoluteRotation()=0;
	virtual bool attachSound(mlMedia* apAudio, ml3DPosition& pos3D)=0;
	virtual bool detachSound(mlMedia* apAudio)=0;
	virtual bool addPotentialTexture(mlMedia* apVisibleObject)=0;
	virtual bool addPotentialTexture(const wchar_t* apwcSrc)=0;
	// Authoring
	virtual bool Get3DPropertiesInfo(mlSynchData &aData)=0;
	virtual bool Get3DProperty(char* apszName,char* apszSubName, mlOutString &sVal)=0;
	virtual bool Set3DProperty(char* apszName,char* apszSubName,char* apszVal)=0;
	// другие
		// установка материала по его описанию (обычно XML)
	virtual void SetMaterialDescription(int aiMatIdx, const wchar_t* apwcDescr)=0;
	// приаттачен ли объект к чему либо
	virtual bool HasAttachParent()=0;
	// список текстур объекта
	virtual std::string getAllTextures()=0;
	// материал под мышкой
	virtual int detectObjectMaterial(int x, int y)=0;
	// установить тайлинг материала
	virtual void SetMaterialTiling(int textureSlotID, float tilingKoef) = 0;
	// показать/скрыть координатные оси
	virtual void showAxis();
	virtual void hideAxis();
	// вкл/выкл тени
	virtual void EnableShadows(bool isEnabled) = 0;
	// изменить тип коллизий
	virtual void ChangeCollisionLevel(int level) = 0;
	virtual bool IsFullyLoaded() = 0;
	virtual bool IsIntersectedWithOtherObjects() = 0;
	// Получить путь к файлу карты текстуры, если он доступен через ResManager
	virtual bool GetMaterialTextureSrc(int aiMatIdx, const wchar_t* apwcMapName, mlOutString &asSrc)=0;
	virtual void LevelChanged(int level) = 0;
	virtual int GetCurrentLevel() = 0;
};

/**
 * Интерфейс 3D-группы
 */
struct RMML_NO_VTABLE moI3DGroup{
	// Свойства
	virtual void ScriptedChanged()=0;
	// Authoring
	virtual bool ChildAdded(mlMedia* apChild)=0;
	virtual bool ChildRemoving(mlMedia* apChild)=0;
};

/**
 * Интерфейс RMML-элементов 3D-групп
 */
struct RMML_NO_VTABLE mlI3DGroup{
	// Свойства
	virtual bool GetScripted()=0;
};

struct RMML_NO_VTABLE moIParticles{
	virtual void SetMaxParticles(int count)=0;
	virtual int GetMaxParticles() const=0;
	virtual void SetDensity(float density)=0;
	virtual float GetDensity() const=0;
	virtual void SetMinSpeed(float speed)=0;
	virtual float GetMinSpeed() const=0;
	virtual void SetMaxSpeed(float speed)=0;
	virtual float GetMaxSpeed() const=0;
	virtual void SetMinSize(float size)=0;
	virtual float GetMinSize() const=0;
	virtual void SetMaxSize(float size)=0;
	virtual float GetMaxSize() const=0;
	virtual void SetMinOpacity(float opacity)=0;
	virtual float GetMinOpacity() const=0;
	virtual void SetMaxOpacity(float opacity)=0;
	virtual float GetMaxOpacity() const=0;
	virtual void SetDirChangeFactor(float factor)=0;
	virtual float GetDirChangeFactor() const=0;
	virtual void SetSpeedChangeFactor(float factor)=0;
	virtual float GetSpeedChangeFactor() const=0;
	virtual bool SetType(const char* name)=0;
	virtual void SetSortVerts(bool sort)=0;
	virtual void SetDefPointSize(float size)=0;
	virtual float GetDefPointSize() const=0;
};

/**
 * Интерфейс звуковых RMML-элементов
 */
struct mlIAudible{
	//
	virtual unsigned long GetRMCurPos() = 0;
	virtual const wchar_t* GetEmitterType() = 0;
	virtual const wchar_t* GetSplineSrc() = 0;
	virtual const int GetVolume() = 0;
	virtual const wchar_t* GetLocation() = 0;
};

/**
 * Интерфейс звуковых объектов
 */
struct moIAudible{
	virtual bool EmitterTypeChanged() = 0;
	virtual bool SplineSrcChanged() = 0;
	virtual bool VolumeChanged() = 0;
	virtual bool LocationChanged() = 0;
};

/**
 * Интерфейс RMML-элемента камеры
 */
struct RMML_NO_VTABLE mlICamera{
	// Свойства
	virtual int GetFOV()=0;
	virtual bool GetFixedPosition()=0;
	virtual void SetFOV(int aiFOV)=0;
	virtual ml3DPosition GetDest()=0;
	virtual ml3DPosition GetDestOculus()=0;
	virtual ml3DPosition GetUp()=0;
	virtual void SetDest(ml3DPosition aiDest)=0;
	virtual mlMedia* GetLinkTo()=0;
	virtual double GetMaxLinkDistance()=0;
	// События
	virtual void onTrackFilePlayed()=0;
};

/**
 * Интерфейс камеры
 */
struct RMML_NO_VTABLE moICamera{
	// Свойства
	virtual void FOVChanged()=0;
	virtual void FixedPositionChanged()=0;
	virtual void DestChanged()=0;
	virtual void DestOculusChanged()=0;
	virtual void UpChanged()=0;
	virtual void ResetDestOculus()=0;
	virtual void LinkToChanged()=0;
	virtual ml3DPosition GetDest()=0; 
	virtual ml3DPosition GetDestOculus()=0;
	virtual ml3DPosition GetUp()=0;
	virtual void MaxLinkDistanceChanged()=0;
	virtual void SetNearPlane(float nearPlane)=0;
	virtual float GetZLevel()=0;
	// Методы
	virtual int destMoveTo(ml3DPosition aPos3D, int aiDuaration, bool abCheckCollisions)=0;
	virtual void Rotate(int aiDuaration, int aiTime)=0;
	virtual void Move(int aiDuaration, int aiTime)=0;
	virtual void ClearOffset() = 0;
	virtual bool HasOffset() = 0;
	virtual void CorrectPosition() = 0;
	// воспроизводит движение камеры по треку из файла
	virtual void PlayTrackFile(const wchar_t* apwcFilePath) = 0;
	// События
};

/**
 * Интерфейс RMML-элемента viewport'а
 */
struct RMML_NO_VTABLE mlIViewport{
	// Дочерние элементы
	virtual mlMedia* GetHint()=0;
	// Свойства
	virtual mlMedia* GetCamera()=0;
	virtual mlMedia** GetInclude()=0;
	virtual mlMedia** GetExclude()=0;
	virtual double GetSoundVolume()=0;
	virtual bool GetTraceObjectOnMouseMove()=0;
	// События
	virtual void onShowHint(mlMedia* apObject, mlPoint aScreenXY, const wchar_t* apwcText)=0;
	virtual void onHintShown(mlMedia* apObject)=0;
	virtual void onHideHint()=0;
};

// Параметры 3D-локации
struct ml3DLocation
{
	mlOutString ID;
	mlOutString params;
};

#define NO_OBJECT		0xFFFFFFFF

/**
 * Интерфейс viewport'а
 */
struct RMML_NO_VTABLE moIViewport{
		// abGroundOnly - игнорировать все объекты кроме земли
		// apaCheckObjects - массив 3D-объектов, которые обязательно нужно проверить
		// на пересечение с лучом, даже если они имеют прозрачную текстуру
		// appTextureMedia - возвращаемая ссылка на rmml-2D-объект, натянутый на текстуру, в которую попали
		// apXYTextureMedia - 2D-координаты положения мыши относительно этого 2D-объекта
	virtual mlMedia* GetObjectUnderMouse(mlPoint &aMouseXY, bool abGroundOnly = false,
		ml3DPosition *apXYZ=NULL, ml3DLocation *apLocation=NULL, int *apiType=NULL, 
		ml3DPosition *apXYTexture=NULL, mlMedia** apaCheckObjects=NULL, 
		mlMedia** appTextureMedia=NULL, mlPoint* apXYTextureMedia=NULL, bool* abpGround=NULL)=0;
	virtual bool GetProjection(ml3DPosition &aXYZ, mlPoint &aXY)=0;
	virtual bool GetProjection(mlMedia* ap3DO, mlPoint &aXY, mlRect &aBounds)=0;
	virtual void GetRay(mlPoint &aXY, ml3DPosition &aXYZ0, ml3DPosition &aXYZ)=0;
	virtual void SetLODScale(float lodScale)=0;
	// Свойства
	virtual void CameraChanged()=0;
	virtual void InclExclChanged()=0;
	virtual void SoundVolumeChanged()=0;
	virtual void TraceObjectOnMouseMoveChanged()=0;
	// События
	virtual void onPress(mlPoint apntXY)=0; // нажали где-то (apntXY) на viewport-е
	virtual void onRelease()=0;				// отпустили кнопку мыши
	// Методы
	virtual mlRect GetBounds(mlMedia* apObject)=0;
		// получить точку пересечения луча с экрана с плоскостью, заданной точкой и нормалью
	virtual ml3DPosition GetIntersectionWithPlane(mlPoint aXY, ml3DPosition aPlane, ml3DPosition aNormal, bool& abBackSide)=0;
		// получить точку проекции 3D-точки на землю с учетом гравитации
	virtual ml3DPosition GetProjectionToLand(ml3DPosition a3DPoint)=0;
	// получить точку проекции 3D-точки на землю с учетом объектов
	virtual ml3DPosition GetProjectionToObjects(ml3DPosition a3DPoint)=0;
		// получить строковый идентификатор объекта по 2D-координатам
	virtual bool GetObjectStringIDAt(mlPoint aXY, mlOutString& asStringID)=0;
};

struct mlStyle{
	enum Units{
		U_undef,
		U_pt,		// пункты
		U_px,		// пикселы
		U_percent	// проценты
	};
	struct IntWithUnits{
		short val; // -1 - default
		Units units;
	};
	enum{
		FS_undef,
		FS_normal,
		FS_italic
	} fontStyle;
	enum{
		FW_undef,
		FW_normal,
		FW_bold
	} fontWeight;
	IntWithUnits fontSize;
	IntWithUnits lineHeight;
	char* fontFamily;
	enum{
		TA_undef,
		TA_left,
		TA_right,
		TA_center,
		TA_justify
	} textAlign;
	IntWithUnits textIndent;
	enum{
		TD_undef,
		TD_none,
		TD_underline
	} textDecoration;
	struct Color{
		bool def; // defined
		unsigned char r,g,b;
	} color;
	enum{
		AA_undef,
		AA_none,
		AA_smooth
	} antialiasing;

	mlStyle(){
		fontStyle=FS_undef;
		fontWeight=FW_undef;
		fontSize.val=-1;
		fontSize.units=U_undef;
		lineHeight.val=-1;
		lineHeight.units=U_undef;
		fontFamily=NULL;
		textAlign=TA_left;
		textIndent.val=-1;
		textIndent.units=U_undef;
		textDecoration=TD_undef;
		color.def=false;
		antialiasing=AA_undef;
	}

	// !!! the strucure must be destroying in the DLL it was created in !!!
	~mlStyle();	
	mlStyle& operator=(mlStyle& aStyle);
};

/**
 * Интерфейс RMML-элементов изображении
 */
struct RMML_NO_VTABLE mlIImage{
	enum Filter{
		AA_NONE,
		AA_LINEAR,
		AA_ANISOTROPIC
	};
	virtual Filter GetAntialiasing()=0;
	virtual bool GetEditable()=0;
	virtual bool GetEditableNow()=0;
	virtual bool GetRemoteControlNow()=0;
	virtual mlColor GetColor()=0;
	virtual bool GetTile()=0;
	virtual bool GetSmooth()=0;
	virtual mlBounds GetCrop()=0;
	virtual bool GetCompressed()=0;
	virtual int GetAngle()=0;
	virtual unsigned char GetMaxDecreaseKoef()=0;
	virtual bool GetSysmem()=0;
};

/**
 * Интерфейс изображения
 */
struct RMML_NO_VTABLE moIImage{
	virtual void EditableChanged()=0;
	virtual void EditableNowChanged()=0;
	virtual void ColorChanged()=0;
	virtual void TileChanged()=0;
	virtual void SmoothChanged()=0;
	virtual void CropChanged()=0;
	virtual void CompressedChanged()=0;
	virtual void AngleChanged()=0;
	virtual void MaxDecreaseKoefChanged()=0;
	virtual void SysmemChanged()=0;
	virtual bool MakeSnapshot(mlMedia** appVisibles, const mlRect aCropRect)=0;
	virtual void DrawLine(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color, int aiWeight)=0;
	virtual void FillRectangle(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color)=0;
	virtual void SetCanvasSize(int aiWidth, int aiHeight)=0;
	virtual void DrawEllipse(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor aColor, int aiWeight, const mlColor aFillColor)=0;
	virtual void DrawRectangle(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor aColor, int aiWeight, const mlColor aFillColor)=0;
	virtual void SetActiveTool(int aToolNumber)=0;
	virtual void UndoAction()=0;
	virtual void PasteText(bool isPasteOnIconClick = false)=0;
	virtual void SetLineWidth(int aLineWidth)=0;
	virtual void SetLineColor(const mlColor aColor)=0;
	virtual void SetBgColor(const mlColor aColor)=0;
	virtual void SetToolOpacity(int aOpacity)=0;
	virtual void SetTextFontName(wchar_t* aFontName)=0;
	virtual void SetTextBold(bool isBold)=0;
	virtual void SetTextItalic(bool isItalic)=0;
	virtual void SetTextUnderline(bool isUnderline)=0;
	virtual void SetTextAntialiasing(bool isAntialiasing)=0;
	virtual void SetToolMode(int aMode)=0;
	virtual void SetLineDirection(int aDirection)=0;
	virtual void Resample(int aiWidth, int aiHeight)=0;
	virtual void ClearOldScreen()=0;
	virtual mlColor GetPixel(int aiX, int aiY)=0;
	// сделать себе копию изображения
	virtual void DuplicateFrom(mlMedia* apSourceImage)=0;
	virtual bool isPaintToolUsed()=0;
	virtual bool isFirstSharingFrameDrawed()=0;
	virtual void CopyToClipboard() = 0;
	virtual bool ChangeState( unsigned char type, unsigned char *aData, unsigned int aDataSize) = 0;
	virtual bool GetBinState( BYTE*& aBinState, int& aBinStateSize) = 0;
	virtual bool SetIPadDrawing( bool abIPadDrawing) = 0;
	virtual bool ApplyIPadState() = 0;
	virtual bool IsBinaryStateReceived() = 0;
	virtual void Save() = 0;
};

/**
 * Интерфейс текстовых RMML-элементов
 */
struct RMML_NO_VTABLE mlIText{
	// Свойства
	virtual const wchar_t* GetValue()=0; // получить текст
	virtual mlStyle* GetStyle(const char* apszTag=NULL)=0;
	virtual bool IsHtml()=0;
	virtual const wchar_t* GetSliceTail()=0;
	virtual bool IsAdjustHeight()=0;
	virtual const mlColor GetBkgColor()=0;
	virtual mlSize GetFormatSize()=0; 
	// установить размер текстового поля
	// (может не совпадать с размером элемента)
	virtual void SetTextSize(unsigned int auWidth, unsigned int auHeight)=0;
	// установить размер текста в количестве строк
	virtual void SetScrollSizeInLines(unsigned int auPageSize, unsigned int auTextSize)=0;
	virtual int GetSymbolIDByXY(int x, int y) = 0;
	// События
};

/**
 * Интерфейс текста
 */
struct RMML_NO_VTABLE moIText{
	// Свойства
	virtual void ValueChanged()=0;
	virtual void StylesChanged()=0;
	virtual void HtmlChanged()=0;
	virtual void AdjustChanged()=0;
	virtual void BkgColorChanged()=0;
	virtual unsigned int GetScrollTop()=0;
	virtual void SetScrollTop(unsigned int auValue)=0;
	virtual unsigned int GetScrollLeft()=0;
	virtual void SetScrollLeft(unsigned int auValue)=0;
	// События
	// Методы
	virtual void ScrollToLine(unsigned int auLine)=0;
	virtual void AddText(wchar_t* str)=0;
	//added nikitin
	virtual void DecreaseCursor(unsigned int delta)=0;
	virtual void IncreaseCursor(unsigned int delta)=0;
	virtual void GetVisibleBounds(int& b, int& e)=0;
	virtual void SetCursorPos(int pos)=0;
	virtual void ToClipboard(wchar_t* str)=0;
	virtual std::wstring FromClipboard()=0;
	virtual int GetCursorX()=0;
	virtual int GetCursorY()=0;
	virtual int GetCursorPos()=0;
	virtual int GetCursorHeight()=0;
	virtual void SetSelection(int from, int to)=0;
	virtual bool GetSliced()=0;
	virtual int GetSymbolIDByXY(int x, int y) = 0;
	////added nikitin
	// Прочие
	virtual bool GetTextFormattingInfo(mlSynchData &aData)=0; // для Authoring-а
	virtual bool AreWordBreaksExists() = 0;
};

/**
 * Интерфейс RMML-элементов ввода
 */
struct RMML_NO_VTABLE mlIInput{
	virtual const wchar_t* GetValue()=0; // получить текст
	virtual mlMedia* GetParent()=0;
	virtual mlStyle* GetStyle(const char* apszTag=NULL)=0;
	virtual const wchar_t* GetSliceTail()=0;
	virtual bool IsAdjustHeight()=0;
	virtual void SetInputSize(unsigned int auWidth, unsigned int auHeight) = 0;
	virtual bool GetPassword()=0;
	virtual bool GetMultiline()=0;
	virtual int GetTabGroup()=0;
	virtual int GetTabIndex()=0;
	virtual void OnKeyDown(int aiCode)=0;
	virtual void OnSetFocus()=0;
	virtual void OnKillFocus()=0;
};

/**
 * Интерфейс объектов ввода
 */
struct RMML_NO_VTABLE moIInput{
	virtual void ValueChanged()=0;
	virtual void StylesChanged()=0;
	virtual void PasswordChanged()=0;
	virtual void MultilineChanged()=0;
	virtual void TabGroupChanged()=0;
	virtual void TabIndexChanged()=0;

	virtual const wchar_t* GetValue() const = 0;

	virtual bool InsertText(const wchar_t* apwcText, int aiPos=-1)=0;
};

/**
 * Интерфейс видео RMML-элементов
 */
struct RMML_NO_VTABLE mlIVideo{
	// Свойства
	virtual bool IsWindowed()=0;
	// События
};

/**
 * Интерфейс видео
 */
struct RMML_NO_VTABLE moIVideo{
	// Свойства
	// События
	virtual void WindowedChanged()=0;
};

/**
 * Интерфейс RMML-элементов 2D-линии
 */
struct RMML_NO_VTABLE mlILine{
	virtual mlPoint GetAbsXY2()=0;
	virtual mlColor GetColor()=0;
	virtual int GetThickness()=0;
};

/**
 * Интерфейс 2D-линии
 */
struct RMML_NO_VTABLE moILine{
	virtual void ColorChanged()=0;
	virtual void ThicknessChanged()=0;
	virtual void SmoothChanged()=0;
};

#define ML_UNDEF_MARGIN_VALUE -1

/**
* Интерфейс RMML-элементов рамки из изображения
*/
struct RMML_NO_VTABLE mlIImageFrame{
	// 
	virtual int GetMargins(int& aiMarginTop, int& aiMarginBottom, int& aiMarginLeft, int& aiMarginRight)=0;
	virtual bool GetTile()=0;
};

struct RMML_NO_VTABLE moIImageFrame{
	// 
	virtual void MarginsChanged()=0;
	virtual void TileChanged()=0;
};

/**
 * Интерфейс RMML-элементов походок 3D-персонажей
 */
struct RMML_NO_VTABLE mlIMovement{
	virtual mlMedia* GetMotion(const wchar_t* apwcName)=0;
	// Свойства
	virtual double	GetStepLength()=0;
	virtual double	GetRotSpeed()=0;
	// События
};

/**
 * Интерфейс набора idle-ов для 3D-персонажей
 */
struct RMML_NO_VTABLE mlIIdles{
	virtual mlMedia* GetIdle(int aiNum)=0;
};

/**
 * Интерфейс idle-а для 3D-персонажей
 */
struct RMML_NO_VTABLE mlIIdle{
	virtual mlMedia* GetMotion()=0;
	virtual int GetBetween()=0;
	virtual double GetFreq()=0;
};

enum mlE3DShadowType{
	MLSHT_NONE,
	MLSHT_PROJECTIVE,
	MLSHT_STENCIL
};

/**
 * Интерфейс тени для 3D-объектов
 */
struct RMML_NO_VTABLE mlIShadow{
	virtual mlE3DShadowType GetType()=0;
	virtual ml3DPlane GetPlane()=0;
	virtual mlColor GetColor()=0;
	virtual mlMedia* GetLight()=0;
};

/**
 * Интерфейс набора теней для 3D-объектов
 */
struct RMML_NO_VTABLE mlIShadows: public mlIShadow{
	virtual mlMedia* GetShadow(int aiNum)=0;
};

/**
 * Интерфейс RMML-элементов 3D-персонажей
 */
struct RMML_NO_VTABLE mlICharacter{
	// Дочерние элементы
	virtual mlMedia* GetCloud()=0;
	// Свойства
	virtual mlIMovement* GetMovement()=0;
	virtual mlMedia* GetVisemes()=0;
	virtual mlMedia* GetIdles()=0;
	virtual mlIIdles* GetFaceIdles()=0;
	virtual bool GetUnpickable();
	// События
	virtual void onTurned()=0;
	virtual void onSaid()=0;
	virtual void onFalling()=0;
	virtual void onFallAndStop()=0;
	virtual void onWreck()=0;
	virtual void onNeedToChangePosition()=0;
	virtual void onRightHandPosChanged(ml3DPosition aRightHandPos, ml3DPosition aRightHandDir, 
		ml3DPosition aRightHandDirExt)=0;
	virtual void onLeftHandPosChanged(ml3DPosition aLeftHandPos, ml3DPosition aLeftHandDir, 
		ml3DPosition aLeftHandDirExt)=0;
	// Методы
	virtual void SynchronizeZPosition()=0;
};

/**
 * Интерфейс 3D-персонажей
 */
struct RMML_NO_VTABLE moICharacter{
	// Свойства
	virtual void UnpickableChanged()=0;
	virtual void MovementChanged()=0;
	virtual void VisemesChanged()=0;
	virtual void IdlesChanged()=0;
	virtual void FaceIdlesChanged()=0;
	virtual mlMedia* GetSpeech()=0; // speech/audio, который сейчас произносится
	virtual mlICharacter** GetInterlocutors()=0;
	virtual void IdlesSrcChanged(const wchar_t* apwcIdlesName, const wchar_t* apwcSrc)=0;
	// Методы
	virtual bool say(mlMedia* apSpeech=NULL, int* apDuration=NULL)=0;
	virtual bool goTo(mlMedia* apObject, ml3DPosition* apTurnToPos=NULL, mlMedia* apTurnToObject=NULL, unsigned int shiftTime = 0)=0;
	virtual bool goTo(ml3DPosition &aPos, ml3DPosition* apTurnToPos=NULL, mlMedia* apTurnToObject=NULL, unsigned int shiftTime = 0)=0;
	virtual bool turnTo(mlMedia* apObject)=0;
	virtual bool turnTo(ml3DPosition &aPos)=0;
	virtual bool turnTo(int aiFlags)=0;
	virtual bool lookAt(mlMedia* apObject, int aiMs, double adKoef)=0;
	virtual bool lookAt(ml3DPosition &aPos, int aiMs, double adKoef)=0;
	virtual bool lookAtByEyes(mlMedia* apObject, int aiMs, double adKoef)=0;
	virtual bool lookAtByEyes(ml3DPosition &aPos, int aiMs, double adKoef)=0;
	virtual bool lookAtByEyes(const wchar_t* apDest, int aiMs, double adKoef)=0;
	virtual bool addInterlocutor(mlICharacter* apCharacter)=0;
	virtual bool removeInterlocutor(mlICharacter* apCharacter)=0;
		// установить выражение лица с индексом auiIndex, взятое из apVisemes.
		// aiTime - период времени установки/сброса, 
		// aiKeepupTime - период времени, сколько "держать" это выражение на лице
	virtual bool setViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime=-1, int aiKeepupTime=-1)=0;
		// убрать с лица выражение с индексом auiIndex, взятое из apVisemes.
		// aiTime - период времени "убирания"
	virtual bool unsetViseme(mlMedia* apVisemes, unsigned int auiIndex, int aiTime=-1)=0;
		// последовательно сменить все выражения лица, описанные в apVisemes, за период времени aiTime
	virtual bool doVisemes(mlMedia* apVisemes, int aiTime=-1)=0;
		// установить персонажа в свободное место в области, заданной аргументами:
		// apStartPos - начальная позиция, adRadius - радиус области
		// abInRoom - поместить в пределах комнаты
	virtual bool setToRandomFreePlace(ml3DPosition* apStartPos, double adRadius, bool abInRoom)=0;
		// иди вперед
	virtual bool goForward(unsigned int shiftTime)=0;
		// пяться
	virtual bool goBackward(unsigned int shiftTime)=0;
		// иди боком влево
	virtual bool strafeLeft(unsigned int shiftTime)=0;
		// иди боком вправо
	virtual bool strafeRight(unsigned int shiftTime)=0;
		// перестань идти (как goTo(NULL))
	virtual bool stopGo()=0;
		// поворачивайся влево
	virtual bool turnLeft(unsigned int shiftTime)=0;
		// поворачивайся вправо
	virtual bool turnRight(unsigned int shiftTime)=0;
		// перестань поворачиваться
	virtual bool stopTurn()=0;
		// воспроизвести записанный звук
	virtual bool sayRecordedVoice( const char* alpcUserName, const char* alpcFileName, LPCSTR alpcCommunicationAreaName, unsigned int auPosition, unsigned int auSyncVersion) = 0;
		// установить положение губ за указанное время
	virtual bool setLexeme( unsigned int auiIndex, int aiTime=0) = 0;
		// иди вперед и налево
	virtual bool goLeft(unsigned int shiftTime)=0; 
		// иди вперед и направо
	virtual bool goRight(unsigned int shiftTime)=0; 
		// иди назад и налево
	virtual bool goBackLeft(unsigned int shiftTime)=0; 
		// иди назад и направо
	virtual bool goBackRight(unsigned int shiftTime)=0; 

	virtual bool getFaceVector(float& x, float& y, float& z) = 0;

    virtual void toInitialPose() = 0;
	virtual void fromInitialPose() = 0;
		// показать на заданную точку или объект
	virtual bool pointByHandTo(const ml3DPosition& aPos, mlMedia* apObject = NULL) = 0;

	virtual bool setSleepingState(bool abSleep) = 0;
	virtual void SetTrackingRightHand(bool isEnabled) = 0;
	virtual void SetTrackingLeftHand(bool isEnabled) = 0;
	virtual bool setKinectDeskPoints(const ml3DPosition& aPos, const ml3DPosition& aPos2, const moMedia* moObj) = 0;
	virtual void disableKinectDesk() = 0;
	// События
};

/**
 * Интерфейс RMML-элементов движений 3D-объектов
 */
struct RMML_NO_VTABLE mlIMotion{
	// Свойства
	virtual void SetDuration(unsigned int aiDuration)=0;
	virtual unsigned int GetDuration()=0;
	virtual bool GetInverse()=0;
	virtual bool GetPrimary()=0;
	// События
	virtual void onSet()=0;
	virtual void onDone()=0;
};

/**
 * Интерфейс движений 3D-объектов
 */
struct RMML_NO_VTABLE moIMotion{
	// Свойства
	virtual void DurationChanged()=0;
	virtual void InverseChanged()=0;
	virtual void PrimaryChanged()=0;
	virtual mlMedia* GetObject()=0; // какой объект выполняет или выполнил это действие
	// События
	// Прочие
	virtual ml3DPosition GetFramePos(const wchar_t* apwcTrack, unsigned int auFrame)=0; 
	virtual void SetFramePos(const wchar_t* apwcTrack, unsigned int auFrame, ml3DPosition &pos)=0;
	virtual ml3DScale GetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame)=0;
	virtual void SetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame, ml3DScale &scl)=0;
	virtual ml3DRotation GetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame)=0;
	virtual void SetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame, ml3DRotation &rot)=0;
	//
	virtual ml3DPosition GetFramePos(int aiTrack, unsigned int auFrame)=0; 
	virtual void SetFramePos(int aiTrack, unsigned int auFrame, ml3DPosition &pos)=0;
	virtual ml3DScale GetFrameScale(int aiTrack, unsigned int auFrame)=0;
	virtual void SetFrameScale(int aiTrack, unsigned int auFrame, ml3DScale &scl)=0;
	virtual ml3DRotation GetFrameRotation(int aiTrack, unsigned int auFrame)=0;
	virtual void SetFrameRotation(int aiTrack, unsigned int auFrame, ml3DRotation &rot)=0;
	virtual bool moveTo(ml3DPosition& aPos3D)=0;
	virtual bool rotateTo(ml3DRotation& aRot3D)=0;
};

/**
 * Интерфейс RMML-элементов 'облачков' 3D-персонажей
 */
struct RMML_NO_VTABLE mlICloud{
	// Дочерние элементы
	virtual mlMedia* GetText()=0;
	virtual mlMedia* GetNicknameText()=0;
	// Свойства
	virtual bool GetVisibility()=0;
	virtual mlMedia* GetContent()=0;
	virtual mlMedia* GetFootnote()=0;
	virtual const wchar_t* GetAlign()=0;
	virtual bool GetMoveOutOfScreen()=0;
	virtual bool GetAllowOverlap()=0;
	virtual bool GetSortOnDepth()=0;
	virtual int GetMarginX()=0;
	virtual int GetMarginY()=0;
	virtual double GetMaxVisibleDistance()=0;
	virtual mlMedia* GetAttachedTo()=0;
};

/**
* Интерфейс 'облачков' 3D-объектов
*/
struct RMML_NO_VTABLE moICloud{
	virtual void VisibleChanged()=0;
	virtual void ContentChanged()=0;
	virtual ml3DPosition GetPos()=0;
	virtual void SetPos(ml3DPosition &aPos)=0;
	virtual void PosChanged(ml3DPosition &pos)=0;
	virtual void FootnoteChanged()=0;
	virtual void AlignChanged()=0;
	virtual void MoveOutOfScreenChanged()=0;
	virtual void AllowOverlapChanged()=0;
	virtual void SortOnDepthChanged()=0;
	virtual void MaxVisibleDistanceChanged()=0;
	virtual void MarginXChanged()=0;
	virtual void MarginYChanged()=0;
	virtual void AttachedToChanged(mlMedia* pmlMedia)=0;
	virtual void SetSpecParameters(int adeltaNoChangePos, int ashiftAtAvatarPosZ)=0;
	virtual void ViewVecChanged(ml3DPosition &viewVec);
};

/**
 * Интерфейс hint-а для 3D-объектов
 */
struct RMML_NO_VTABLE mlIHint{
	// Свойства
	virtual mlStyle* GetStyle(const char* apszTag=NULL)=0;
	// События
};

/**
 * Интерфейс Path3D-объекта
 */
struct RMML_NO_VTABLE moIPath3D{
	// Свойства
	// События
	// прочие
	virtual ml3DPosition getPosition(double adKoef)=0;
	virtual ml3DPosition getLinearPosition(double adKoef)=0;
	virtual ml3DPosition getNormal(double adKoef)=0;
	virtual ml3DPosition getTangent(double adKoef)=0;
	virtual ml3DPosition getNormalByLinearPos(double adKoef)=0;
	virtual ml3DPosition getTangentByLinearPos(double adKoef)=0;
};

enum ML_VARENUM{
    MLVT_EMPTY           = 0,
    MLVT_NULL            = 1,
    MLVT_I2              = 2,
    MLVT_I4              = 3,
    MLVT_R4              = 4,
    MLVT_R8              = 5,
    MLVT_CY              = 6,
    MLVT_DATE            = 7,
    MLVT_BSTR            = 8,
    MLVT_DISPATCH        = 9,
    MLVT_ERROR           = 10,
    MLVT_BOOL            = 11,
    MLVT_VARIANT         = 12,
    MLVT_UNKNOWN         = 13,
    MLVT_DECIMAL         = 14,
// VBA reserves 15 for future use
    MLVT_I1              = 16,
    MLVT_UI1             = 17,
    MLVT_UI2             = 18,
    MLVT_UI4             = 19,
    MLVT_I8              = 20,
    MLVT_UI8             = 21,
    MLVT_INT             = 22,
    MLVT_UINT            = 23,
    MLVT_VOID            = 24,
    MLVT_HRESULT         = 25,
    MLVT_PTR             = 26,
    MLVT_SAFEARRAY       = 27,
    MLVT_CARRAY          = 28,
    MLVT_USERDEFINED     = 29,
    MLVT_LPSTR           = 30,
    MLVT_LPWSTR          = 31,
// VBA reserves 32-35 for future use
    MLVT_RECORD          = 36,

    MLVT_FILETIME        = 64,
    MLVT_BLOB            = 65,
    MLVT_STREAM          = 66,
    MLVT_STORAGE         = 67,
    MLVT_STREAMED_OBJECT = 68,
    MLVT_STORED_OBJECT   = 69,
    MLVT_BLOB_OBJECT     = 70,
    MLVT_CF              = 71,
    MLVT_CLSID           = 72,

    MLVT_BSTR_BLOB       = 0x0fff,

    MLVT_VECTOR          = 0x1000,
    MLVT_ARRAY           = 0x2000,
    MLVT_BYREF           = 0x4000,
    MLVT_RESERVED        = 0x8000,

    MLVT_ILLEGAL         = 0xffff,
    MLVT_ILLEGALMASKED   = 0x0fff,
    MLVT_TYPEMASK        = 0x0fff
};
struct ML_VARIANT{
	unsigned short vt;
	MLWORD wReserved1;
	MLWORD wReserved2;
	MLWORD wReserved3;
	union{
		MLLONG lVal;
		MLBYTE bVal;
		short iVal;
		float fltVal;
		double dblVal;
		short boolVal;
		long scode;
		union tagCY {
			struct {
#ifdef _MAC
				long      Hi;
				long Lo;
#else
				unsigned long Lo;
				long      Hi;
#endif
			};
			MLDWORD int64;
		} cyVal;
		double date;
		wchar_t* bstrVal;
		void *punkVal;
		void *pdispVal;
		void *parray;
		MLBYTE *pbVal;
		short *piVal;
		long *plVal;
		float *pfltVal;
		double *pdblVal;
		short *pboolVal;
		short *pbool;
		long *pscode;
		tagCY *pcyVal;
		double *pdate;
		wchar_t* *pbstrVal;
		void* *ppunkVal;
		void* *ppdispVal;
		void* *pparray;
		ML_VARIANT *pvarVal;
		void* byref;
		char cVal;
		unsigned short uiVal;
		unsigned long ulVal;
		int intVal;
		unsigned int uintVal;
		struct tagDEC {
			unsigned short wReserved;
			union {
				struct {
					MLBYTE scale;
					MLBYTE sign;
				};
				unsigned short signscale;
			};
			unsigned long Hi32;
			union {
				struct {
#ifdef _MAC
					MLULONG Mid32;
					MLULONG Lo32;
#else
					MLULONG Lo32;
					MLULONG Mid32;
#endif
				};
				MLDWORD Lo64;
			};
		};
		tagDEC *pdecVal;
		char* *pcVal;
		MLUSHORT *puiVal;
		MLULONG *pulVal;
		int *pintVal;
		unsigned int *puintVal;
		struct  __tagBRECORD{
			void* pvRecord;
			void *pRecInfo;
		};
	};
};

/**
 * Интерфейс ActiveX-элементов RMML
 */
struct RMML_NO_VTABLE mlIActiveX{
	// Свойства
	// События
	virtual omsresult CallExternal(const wchar_t* apProcName, int aiArgC, 
						   ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent)=0;
};

/**
 * Интерфейс ActiveX-объектов
 */
struct RMML_NO_VTABLE moIActiveX{
	// Свойства
	// События
	virtual omsresult Call(wchar_t* apProcName, int aiArgC, 
						   ML_VARIANT* pvarArgs, ML_VARIANT* varRes)=0;
	virtual omsresult Set(wchar_t* apPropName,ML_VARIANT* vVar)=0;
	virtual omsresult Get(wchar_t* apPropName,ML_VARIANT* varRes)=0;
};

/**
* Интерфейс Browser-элементов RMML
*/
struct RMML_NO_VTABLE mlIBrowser{
	// Свойства
	virtual mlColor GetBkgColor()=0;
	virtual const wchar_t* GetDefaultCharset()=0;
	virtual bool GetAXFrame()=0;
	virtual const wchar_t* GetValue()=0;
	// Методы
	// События
	virtual void onScrolled()=0; // документ проскроллировали html-ку (колесом мыши, выделением...)
		// Fired when a new hyperlink is being navigated to.
	virtual void onBeforeNavigate(const wchar_t* URL, int flags, const wchar_t* targetFrameName, 
				const wchar_t* postData, const wchar_t* headers, bool &cancel)=0;
		// Fired when the document being navigated to becomes visible and enters the navigation stack
	virtual void onNavigateComplete(const wchar_t* URL)=0;
	virtual void onDocumentComplete(const wchar_t* URL)=0;
		// Fired when download progress is updated
	virtual void onProgressChange(int progress, int progressMax)=0;
	virtual void onDownloadBegin()=0; // Download of a page started
	virtual void onDownloadComplete()=0; // Download of page complete
	virtual void onCommandStateChange(int command, bool enable)=0; // The enabled state of a command changed 
		// Fires when an error occurs during navigation
	virtual void onNavigateError(const wchar_t* URL, const wchar_t* frame, int StatusCode, bool &cancel)=0;
};

/**
* Интерфейс Browser-объектов
*/
struct RMML_NO_VTABLE moIBrowser{
	// Свойства
	virtual const wchar_t* GetDocumentType()=0;
	virtual const wchar_t* GetLocationName()=0;
	virtual const wchar_t* GetLocationURL()=0;
	virtual bool GetBusy()=0;
	virtual int GetReadyState()=0;
	virtual bool GetOffline()=0;
	virtual void SetOffline(bool abValue)=0;
	virtual bool GetSilent()=0;
	virtual void SetSilent(bool abValue)=0;
	virtual int GetScrollTop()=0;
	virtual void SetScrollTop(int aiValue)=0;
	virtual int GetScrollLeft()=0;
	virtual void SetScrollLeft(int aiValue)=0;
	virtual void BkgColorChanged()=0;
	virtual void ValueChanged()=0;
	// Методы
		// flags: 2 - NoHistory, 4 - NoHistory, 8 - NoWriteToCache
	virtual omsresult Navigate(const wchar_t* URL, int flags, const wchar_t* postData, const wchar_t* headers)=0;
	virtual void GoBack()=0; // Navigates to the previous item in the history list
	virtual void GoForward()=0; // Navigates to the next item in the history list
	virtual void GoHome()=0; // Go home/start page (src)
	virtual void Refresh(int level)=0; // Refresh the currently viewed page (NORMAL = 0, IFEXPIRED = 1, CONTINUE = 2, COMPLETELY = 3)
	virtual void Stop()=0; // Stops opening a file
	virtual bool ScrollTo(int x, int y)=0; // абсолютный
	virtual bool ScrollBy(int x, int y)=0; // относительный 
	virtual void ScrollToAnchor(const wchar_t* anchor)=0;
	virtual omsresult ExecScript(const wchar_t* code, const wchar_t* language, ML_VARIANT* result)=0;
	virtual bool ExecCommand(const wchar_t* cmdID, bool showUI, ML_VARIANT value)=0;
	virtual bool QueryCommandEnabled(const wchar_t* cmdID)=0;
	virtual bool FireMouseDown(int x, int y)=0; // x,y относительно левого верхнего угла окна браузера
	virtual bool FireMouseMove(int x, int y)=0;
	virtual bool FireMouseUp(int x, int y)=0;
	virtual bool FireMouseWheel(int x, int y, int distance)=0;
	virtual bool KeyDown(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode)=0;
	virtual bool KeyUp(unsigned int keyCode, unsigned int keyStates, unsigned int scanCode)=0;
	virtual bool KeyPress(unsigned int charCode, unsigned long scanCode)=0;
//	virtual bool SetFocus()=0;
//	virtual bool KillFocus()=0;
	// События
};

struct mlIFlash
{
    virtual void onFSCommand(const wchar_t *command, const wchar_t *args) = 0;

	virtual int GetWModeOnCreate()=0;
};

struct moIFlash
{
    virtual void  setVariable(const wchar_t *var, const wchar_t *value) = 0;

	virtual int getScaleMode()=0;
	virtual void setScaleMode(int)=0;

	virtual int getWMode()=0;
	virtual void setWMode(int)=0;

// возвращает длину данных
    virtual long getVariable(const wchar_t *var, mlOutString &sRetVal) = 0;
// возвращает длину результата
    virtual long callFunction(const wchar_t *request, mlOutString &sRetVal) = 0;
};

/**
 * Интерфейс ActiveX-элементов RMML
 */
struct RMML_NO_VTABLE mlIPlugin{
	// Свойства
	virtual const wchar_t*	GetUID()=0;
	// События
	virtual omsresult Call(char* apProcName, char* aArgTypes, va_list aArgsV)=0;
};

/**
 * Интерфейс ActiveX-объектов
 */
struct RMML_NO_VTABLE moIPlugin{
	// Свойства
	// События
	//
	virtual omsresult Call(char* apProcName, int aiArgC, va_list aArgsV, MLDWORD &aRes)=0;
};

/**
 * Интерфейс RMML-элементов реплик персонажей
 */
struct RMML_NO_VTABLE mlISpeech{
	// События
	virtual void onSaid()=0;
};

/**
 * Интерфейс реплик персонажей
 */
struct RMML_NO_VTABLE moISpeech{
	virtual long GetCurrentPos()=0;
};

/**
 * Интерфейс RMML-синхронизатора
 */
struct RMML_NO_VTABLE mlISinchronizer{
	virtual omsresult CallEvent( unsigned int aiID, MLBYTE* apData, MLWORD auDataLength)=0;
	virtual omsresult GetObjectID( const wchar_t* apwcStringID, unsigned int &auID)=0;
	virtual omsresult GetObjectStringID( unsigned int aiID, mlOutString &asStringID)=0;
	virtual void NotifyGetState(MLBYTE* apData, MLDWORD alSize)=0;
	// отправить на сервер изменения в состояниях (synch-объектов) синхронизируемых объектов
	virtual omsresult SendStateChanges(MLBYTE* apData, int aiDataLength)=0;
};

// Интерфейс, реализуемый RMML-объектом, поддерживающим бинарную синхронизацию состояния
// (например, картинка-редактор)
// (все состояние держит RM-объект)
struct RMML_NO_VTABLE mlIBinSynch{
	// уведомление о том, что состояние изменилось 
	// с момента последнего запроса изменений
	// (возвращает false, если изменения отсылать нельзя: сделать откат сосояния)
	// (aulOffset и aulBlockSize имеют смысл только для BSSCC_REPLACE)
	// (auStateVersion - версия состояния, к которой применимо это изменение)
	virtual bool StateChanged(unsigned int auStateVersion, BSSCCode aCode, void* apData, unsigned long aulSize, 
			unsigned long aulOffset=0, unsigned long aulBlockSize=0)=0;
	virtual bool GetBinState( BYTE*& aBinState, int& aBinStateSize)=0;
};

// Интерфейс, реализуемый RM-объектом, поддерживающим бинарную синхронизацию состояния
// (например, картинка-редактор)
struct RMML_NO_VTABLE moIBinSynch{
	// с сервера пришло полное состояние: установить его
	virtual bool SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize)=0;
	// с сервера пришли изменения состояния: обновить состояние
	virtual bool UpdateState(unsigned int auStateVersion, BSSCCode aCode, const unsigned char* apState, int aiSize, 
		unsigned long aulOffset=0, unsigned long aulBlockSize=0)=0;
	// нужно отправить на сервер полное состояние: запросить его
	// (возвращает размер данных, apState может быть null)
	virtual unsigned long GetFullState(unsigned int& auStateVersion, const unsigned char* apState, int aiSize)=0;
	// Сбросить состояние объекта, обнулить его версию
	virtual void Reset()=0;
};

/**
 * Структуры для Debugger-а
 */
// структуры для передачи свойств
//struct mldPropertyInfo{
//	int type;
//}; 
struct mldPropertiesInfo{
	short numGroup;
};
struct mldPropGroupInfo{
	short numPropeties;
	int type;
};
// структуры для передачи данных о событиях
struct mldEventsInfo{
	short numGroup;
};
struct mldEventsGroupInfo{
	short numEvents;
	int type;
};

/**
 * Базовый класс всех RMML-элементов, работающих с медиа-объектами
 */
class moMedia;

#define ML_MO_IS_NULL(PMO) ((((unsigned)PMO)&~3)==0)

class RMML_NO_VTABLE mlMedia{
//friend class moMedia;
public:
	unsigned short mType; // тип медиа-объекта (загружаемый, видимый, длящийся, картинка, звук, ...)
                          // этот тип может меняться в зависимости от типа загруженного ресурса
//	shared_ptr<moMedia> mMOMedia; // указатель на медиа-объект
protected:
	moMedia* mpMO; // указатель на медиа-объект
public:
//	void SetMO(shared_ptr<moMedia> aMOMedia){ mMOMedia=aMOMedia; }
	mlMedia(){ mpMO=NULL; }
	void SetMO(moMedia* apMO){ mpMO = apMO; }
	moMedia* GetMO(){ return mpMO; }
	moMedia* GetSafeMO(){ if (ML_MO_IS_NULL(mpMO)) return NULL; return mpMO; }
	virtual mlMedia* GetParentElement()=0;
	virtual mlMedia* GetScene()=0;
	virtual bool IsAllParentComposLoaded()=0;
	virtual bool GetFrozen()=0;
	virtual int EnumPlugins(std::vector<mlMedia*>& avPlugRefs)=0;
	virtual unsigned int GetSynchID(unsigned int &auiBornReality, bool abGetIfUndefined = true)=0;
	virtual mlMedia** GetChildren()=0;
	// возвращает все видимые подобъекты композиции (осовбождать через FreeChildrenArray же)
	virtual mlMedia** GetAllVisibleChildren()=0;
	virtual void FreeChildrenArray(mlMedia** apaChildren)=0;
	// проверка типа медиа-объекта
	bool IsLoadable(){ return (mType & MLMPT_LOADABLE)?true:false; }
	bool IsVisible(){ return (mType & MLMPT_VISIBLE)?true:false; }
	bool IsAudible(){ return (mType & MLMPT_AUDIBLE)?true:false; }
	bool IsContinuous(){ return (mType & MLMPT_CONTINUOUS)?true:false; }
	bool IsButton(){ return (mType & MLMPT_BUTTON)?true:false; }
	// 
	void SetType(int aiType){ mType=(unsigned short)aiType; }
		// уточнение типа медиа-объекта
		// (например видимый медиа-объект при загрузке оказывается видео; 
		//  тогда он вызывает SetType(MLMT_VIDEO))
	int GetType(){ return mType; }
	// произвольный доступ к свойствам 
	virtual int GetPropType(const char*)=0;
	virtual int GetIntProp(const char*)=0;
	virtual bool GetBooleanProp(const char*)=0;
	virtual double GetDoubleProp(const char*)=0;
	virtual wchar_t* GetStringProp(const char*)=0;
	virtual mlMedia* GetRefProp(const char*)=0;
	virtual void SetProp(const char*, int)=0;
	virtual void SetProp(const char*, double)=0;
	virtual void SetProp(const char*, const char*)=0;
	virtual void SetProp(const char*, const wchar_t*)=0;
	// доступ к интерфейсам прототипов
	virtual mlILoadable* GetILoadable(){ return NULL; }
	virtual mlIVisible* GetIVisible(){ return NULL; }
	virtual mlI3DGroup* GetI3DGroup(){ return NULL; }
	virtual mlIAudible* GetIAudible(){ return NULL; }
	virtual mlIContinuous* GetIContinuous(){ return NULL; }
	virtual mlIButton* GetIButton(){ return NULL; }
	virtual mlI3DObject* GetI3DObject(){ return NULL; }
	// доступ к интерфейсам отдельных типов
	virtual mlIImage* GetIImage() { return NULL; }
	virtual mlIText* GetIText(){ return NULL; }
	virtual mlIInput* GetIInput(){ return NULL; }
	virtual mlICamera* GetICamera(){ return NULL; }
	virtual mlIViewport* GetIViewport(){ return NULL; }
	virtual mlICharacter* GetICharacter(){ return NULL; }
	virtual mlICloud* GetICloud(){ return NULL; }
	virtual mlIMotion* GetIMotion(){ return NULL; }
	virtual mlIActiveX* GetIActiveX(){ return NULL; }
	virtual mlIBrowser* GetIBrowser(){ return NULL; }
	virtual mlIFlash* GetIFlash(){ return NULL; }
	virtual mlIPlugin* GetIPlugin(){ return NULL; }
	virtual mlISpeech* GetISpeech(){ return NULL; }
	virtual mlIIdle* GetIIdle(){ return NULL; }
	virtual mlIIdles* GetIIdles(){ return NULL; }
	virtual mlIVideo* GetIVideo(){ return NULL; }
	virtual mlIShadows* GetIShadows(){ return NULL; }
	virtual mlIShadow* GetIShadow(){ return NULL; }
	virtual mlIMap* GetIMap(){ return NULL; }
	virtual mlIMaterial* GetIMaterial(){ return NULL; }
	virtual mlIHint* GetIHint(){ return NULL; }
	virtual mlIScene3D* GetIScene3D(){ return NULL; }
	virtual mlIImageFrame* GetIImageFrame(){ return NULL; }
	virtual mlILine* GetILine(){ return NULL; }
	// доступ к дополнительным интерфейсам
	virtual mlIBinSynch* GetIBinSynch(){ return NULL; }
	// доступ к неизвестным интерфейсам других типов
	virtual void* GetInterface(long /*iid*/){ return NULL; }
	//// поддержка синхронизации
	// сиинхронизировать состояние объекта на всех клиентах
	virtual bool synchronize(mlSynchData* apData=NULL)=0;
	// объект переместился из одной зоны в другую (вызывается MapManager-ом)
	virtual void ZoneChanged(int aiZone)=0;
	// объект перешел в другую реальность (вызывается MapManager-ом)
	virtual void RealityChanged(unsigned int auReality)=0;
	// изменилось визуальное представление объекта
	// (используется как сигнал для перерисовки объектов-текстур 3D-объектов)
	virtual void VisualRepresentationChanged()=0;
	// Объект является синхрониронизируемым
	virtual bool IsSynchronized() = 0;
	// Инициализация объекта закончена, Объект готов к работе
	virtual bool IsSynchInitialized() = 0;
	// 
	virtual void ReadyForBinState()=0;
	// 
	virtual void SetLocationSID(const wchar_t* pszLocationID)=0;
	virtual void SetCommunicationAreaSID(const wchar_t* pszCommunicationAreaID)=0;	
	//
	virtual bool IsForcedUpdateBinState()=0;
};

/**
 * Базовый класс всех медиа-объектов, поддерживаемых RMML
 */
class moMedia{
protected:
	mlMedia* mpMLMedia; // указатель на RMML-элемент
public:
	unsigned short mType; // тип медиа-объекта. Копируется из mpMLMedia->mType только когда медиа-объект отвязывается от RMML-объекта для последующего удаления
	moMedia(){ mpMLMedia = NULL; mType = 0; }
	moMedia(mlMedia* apMLMedia){ SetMLMedia(apMLMedia); mType = 0; }
	virtual ~moMedia(){}
public:
	int GetType(){ return mType; }
	__forceinline void SetMLMedia(mlMedia* apMLMedia){ mpMLMedia=apMLMedia; }
	__forceinline mlMedia* GetMLMedia(){ return mpMLMedia; }
	virtual moILoadable* GetILoadable(){ return NULL; }
	virtual moIVisible* GetIVisible(){ return NULL; }
	virtual moIAudible* GetIAudible(){ return NULL; }
	virtual moIContinuous* GetIContinuous(){ return NULL; }
	virtual moIButton* GetIButton(){ return NULL; }
	virtual moI3DObject* GetI3DObject(){ return NULL; }
	//
	virtual moICamera* GetICamera(){ return NULL; }
	virtual moIViewport* GetIViewport(){ return NULL; }
	virtual moIText* GetIText(){ return NULL; }
	virtual moIInput* GetIInput(){ return NULL; }
	virtual moICharacter* GetICharacter(){ return NULL; }
	virtual moIMotion* GetIMotion(){ return NULL; }
	virtual moIActiveX* GetIActiveX(){ return NULL; }
	virtual moIBrowser* GetIBrowser(){ return NULL; }
	virtual moIFlash* GetIFlash(){ return NULL; }
	virtual moIPlugin* GetIPlugin(){ return NULL; }
	virtual moISpeech* GetISpeech(){ return NULL; }
	virtual moIVideo* GetIVideo(){ return NULL; }
	virtual moIImage* GetIImage(){ return NULL; }
	virtual moIPath3D* GetIPath3D(){ return NULL; }
	virtual moI3DGroup* GetIGroup3D(){ return NULL; }
	virtual moIScene3D* GetIScene3D(){ return NULL; }
	virtual moICloud* GetICloud(){ return NULL; }
	virtual moIBinSynch* GetIBinSynch(){ return NULL; }
	virtual moIImageFrame* GetIImageFrame(){ return NULL; }
	virtual moILine* GetILine(){ return NULL; }
	virtual moIParticles* GetIParticles(){ return NULL; }
	virtual void* GetInterface(long /*iid*/){ return NULL; }
	// если возвращает false, значит метод ничего не делает и можно его в дальнейшем не вызывать
	virtual bool Update(const __int64 /*alTime*/){ return false; }
	virtual void GetSynchData(mlSynchData& /*aSynchData*/){ return; }
	virtual void SetSynchData(mlSynchData& /*aSynchData*/){ return; }
	virtual void FrozenChanged()= 0;
	virtual void PropIsSet(){ return; }
	virtual void AllParentComposLoaded(){ return; }
	virtual void ParentChanged(){ return; }
	//
};


enum mlEModes{
	smmInitilization,
	smmLoading,
	smmNormal,
	smmPlay,
	smmPause,
	smmAuthoring,
	smmDisconnection
};

enum mlETestModes{
	smNoTest,
	smNormalAutoTests,
	smAutoClassTest
};

typedef void (*mlCallbackFunc)(void* apObj, void* apParams);

struct mlCallback{
	mlCallbackFunc mFunc;
	void* mpParams;		// параметр для mCallbackFunc
};

// значение для muID, которое показывает, что элемент не является объектом мира
#define OBJ_ID_UNDEF 0xFFFFFFFF
// значение для muID, которое показывает, что элемент является классом
#define OBJ_ID_CLASS 0xFFFFFFFE
// значение для muID, которое больше или равно данному показывает, что элемент является аватаром
#define OBJ_ID_AVATAR 4000000000
// значение для muID, которое больше или равно данному показывает, что элемент является редактируемым (раставляемым) пользователем объектом
#define OBJ_ID_EDITABLE 1000000000

#define REALITY_ID_UNDEF 0xFFFFFFFF

struct mlObjectInfo4Creation
{
	// Это мой аватар?
	bool isMyAvatar;
	// идентификатор объекта
	unsigned int muObjectID;			
	// идентификатор реаальности рождения
	unsigned int muBornRealityID;		
	// идентификатор реаальности местонахождения
	unsigned int muRealityID;
	// RMML-тип объекта (обычно MLMT_OBJECT или MLMT_GROUP)
	unsigned short musType;	
	// имя объекта
	const char* mszName;	
	// имя класса объекта
	const char* mszClassName;	
	// имя файла класса объекта (для проверки)
	const char* mszClassFileName;	
	// Путь к ресурсам
	const wchar_t* mwcSrc;
	// родительская 3D-сцена или 3D-группа
	const mlMedia* mpParent;
	// скрипт инициализации (userProps)
	const wchar_t* mwcParams;
	mlCallback mCallback;
	// для callback-а
	void* mpMapManager;
	// для callback-а
	const ZONE_OBJECT_INFO* mpZoneObjectInfo;
	//время создания
	long long creatingTime;
	// уровень объекта
	unsigned int muLevel;
	// размеры объекта (из БД)
	ml3DPosition mBounds;
	// позиция аватара (получена либо параметром CreateAvatar, либо с сервера синхронизации)
	ml3DPosition mPos;
	// Полное состояние синхронизируемого объекта
	syncObjectState* objectFullState;
	// ID объекта, у которого надо вызвать onCreate или onError после создания объекта
	int miCreationHandlerID;
	// нужно ли грузить меш с высоким приоритетом
	bool isHighPriority;

	mlObjectInfo4Creation()
	{
		muObjectID = OBJ_ID_UNDEF;
		muBornRealityID = REALITY_ID_UNDEF;
		muRealityID = REALITY_ID_UNDEF;
		musType = 0;
		mszName = NULL;
		mszClassName = NULL;
		mszClassFileName = NULL;
		mwcSrc = NULL;
		mpParent = NULL;
		mwcParams = NULL;
		mCallback.mFunc = NULL;
		mCallback.mpParams = NULL;
		mpMapManager = NULL;
		mpZoneObjectInfo = NULL;
		muLevel = 0;
		creatingTime = 0;
		ML_INIT_3DPOSITION(mBounds);
		ML_INIT_3DPOSITION(mPos);
		objectFullState = NULL;
		miCreationHandlerID = 0;
		isHighPriority = false;
	}
};

struct mlObjectInfo4OnServerCreation: public mlObjectInfo4Creation
{
	ml3DPosition mPos;	// позиция объекта
	ml3DRotation mRot;	// поворот объекта
	ml3DScale mScl;		// масштаб объекта
	ml3DScale mBounds;	// размеры объекта по осям
	int miLevel;		// "уровень" объекта
	const char* mszLocation;	// идентификатор локации
	mlObjectInfo4OnServerCreation(){
		ML_INIT_3DPOSITION(mPos);
		ML_INIT_3DROTATION(mRot);
		ML_INIT_3DSCALE(mScl);
		mScl.x = mScl.y = mScl.z = 1.0;
		ML_INIT_3DSCALE(mBounds);
		miLevel = 0;
		mszLocation = "";
	}
};

/*
this.LOGIN_SERVER_STATUS_BIT = 1;
this.RES_SERVER_STATUS_BIT = 2;
this.LOG_SERVER_STATUS_BIT = 4;
this.MAP_SERVER_STATUS_BIT = 8;
this.SYNC_SERVER_STATUS_BIT = 16;
this.SERVICE_SERVER_STATUS_BIT = 32;
#define	VOIP_RECORD_SERVER_STATUS_BIT	0x40
#define	VOIP_PLAY_SERVER_STATUS_BIT		0x80
*/
enum mlServerMask{
	ML_SERVER_MASK_WORLD_SERVICE = (1 << 0),	// сервер авторизации
	ML_SERVER_MASK_RES = (1 << 1),		// ресурсный сервер
	ML_SERVER_MASK_LOG = (1 << 2),		// (не используется)
	ML_SERVER_MASK_MAP = (1 << 3),		// сервер карты
	ML_SERVER_MASK_SYNC = (1 << 4),		// сервер синхронизации
	ML_SERVER_MASK_SERVICE = (1 << 5),	// сервер сервисов
	ML_SERVER_MASK_VOIP_RECORD = (1 << 6),	// сервер записи речи
	ML_SERVER_MASK_VOIP_PLAY = (1 << 7),	// сервер воспроизведения речи
	ML_SERVER_MASK_SHARING = (1 << 8),		//сервер передачи видео
};

struct RMML_NO_VTABLE mlObjectInfo{
	unsigned int id;
	mlString name;
	mlString src;
	mlString parent;
};

/**
 * Интерфейс, возвращаемый при создании менеджера сцен
 */
struct RMML_NO_VTABLE mlISceneManager{
	// загрузить композицию пользовательского интерфейса
	virtual omsresult LoadUI(const char* asSrc)=0;
	virtual omsresult LoadUI(const wchar_t* asSrc)=0;
	// загрузить модуль
	virtual omsresult LoadModuleMeta()=0;
	// загрузить модуль и сцену
	virtual omsresult LoadModuleMetaAndLoadScene( unsigned int aID, wchar_t *asModuleID, wchar_t* asScene)=0;
	// загрузить сцену модуля по номеру
	virtual omsresult LoadScene(int aiNum)=0;
	// загрузить сцену
	virtual omsresult LoadScene(const char* asSrc)=0;
	virtual omsresult LoadScene(const wchar_t* asSrc)=0;
	// Обновить сцену
	virtual omsresult Update(const __int64 alTime, int &aiDeltaTime, const bool isNormalMode)=0;
	// выдать указатель на композицию пользовательского интерфейса
	virtual mlMedia* GetUI()=0;
	// выдать указатель на активную сцену
	virtual mlMedia* GetActiveScene()=0;
	// выполнить JavaScript
	virtual omsresult ExecScript(const wchar_t* apwcScript, int aiLength=-1, wchar_t* *appwcRetVal=NULL)=0;
	// установить режим работы менеджера сцен
	virtual omsresult SetMode(mlEModes aeMode)=0;
	// получить режим работы менеджера сцен
	virtual mlEModes GetMode()=0;
	// установить тестовый режим работы менеджера сцен
	virtual void SetTestMode(mlETestModes aeTesmMode)=0;
	// установить директорию тестовых ресурсов
	virtual void SetTestResDir(const wchar_t* apwcTestResDir)=0;	
	// взять состояние сцены
	virtual omsresult GetState(void* &apData, unsigned long &aSize)=0;
	// восстановить состояние сцены
	virtual omsresult SetState(void* aData, unsigned long aDataSize)=0;
	// выдать указатель на композицию авторинга
	virtual mlMedia* GetAuthScene()=0;
	// выдать указатель на элемент по его полному имени
	virtual mlMedia* GetObjectByTarget(const wchar_t* apwcTarget)=0;
	//выдать RealityID элементa по его полному имени
	virtual unsigned int GetObjectRealityIDByTarget(const wchar_t* apwcTarget) = 0;
	// выдать указатель на сцену мира
	virtual mlMedia* GetWorldScene() const=0;

	//// Поддержка третьей модели синхронизации
	// 
	// аватар сменил локацию
	virtual void LocationChanged(const wchar_t* apwcLocationName,  unsigned int auiObjectID, unsigned int auiBornRealityID)=0;
	virtual void CommunicationAreaChanged(const wchar_t* apwcCommunicationAreaName,  unsigned int auiObjectID, unsigned int auiBornRealityID)=0;
	// У аватара значительно поменялись координаты xy
	// создать моего аватара (синхронно)
	virtual mlMedia* CreateAvatar(mlObjectInfo4Creation &aAvatarInfo)=0;
	// создать объект (синхронно)
	virtual mlMedia* CreateObject(mlObjectInfo4Creation &aObjectInfo)=0;
	// данные, приходящий с сервака, если там объект не найден
	virtual omsresult HandleObjectNotFound( unsigned int aObjectID, unsigned int aBornReality, unsigned int aRealityID)=0;
	// восстанавливает синхронизацию после потери связи с сервером
	virtual omsresult RestoreSynchronization( const wchar_t* apwcRequestedUrl) = 0;
	// определяет существование объекта в рммл
	virtual bool IsObjectExist( unsigned int auObjectID, unsigned int auBornRealityID)=0;
	// удаляет объект
	virtual omsresult DeleteObject( unsigned int auObjectID, unsigned int auBornRealityID)=0;
		// appaObjects - указатель на переменную, в которую надо положить ссылку на массив медиа-объектов для последующего удаления
	virtual omsresult DeleteObject( mlMedia* apObject, moMedia** *appaObjects = NULL)=0;

	virtual omsresult DeletePreviousRealityObjects(const std::vector<unsigned int>& aExludeOnjects)=0;
	// обрабатывает сообщение от другого клиента
	virtual omsresult HandleSyncMessage(unsigned int auObjectID, unsigned int auBornRealityID, const wchar_t* apwcSender, unsigned char* apData, unsigned short aiDataSize)=0;
	// возвращает список инклудов
	virtual int ExtractIncludes( const wchar_t* apcFileSrc, const wchar_t** appIncludes, int count)=0;

	/*
	// Vladimir 2011/04/14
	*/
	// установить логин для автоматического входа
	virtual void SetLogin(const wchar_t* apwcLogin) = 0;
	// установить пароль для автоматического входа
	virtual void SetPassword(const wchar_t* apwcPassword) = 0;
	// установить тип пароля: закодированный - 1 или не закодированный - 0 - для автоматического входа
	virtual void SetPasswordEncoding(int iPasswordEncodingMode) = 0;
	// установить ключ для автоматического входа
	virtual void SetLoginKey(const wchar_t* apwcLoginKey) = 0;
	// установить ключ для автоматического входа после обновления через loginKey
	virtual void SetUpdaterKey(const wchar_t* apwcUpdaterKey) = 0;
	// установить ключ для автоматического входа после обновления через loginKey, функция для вызова из потока
	virtual void SetUpdaterKeyByThread(const wchar_t* apwcUpdaterKey) = 0;
	/*
	// end
	*/

	virtual void OnWindowMoved() = 0;
	virtual void OnWindowSizeChanged( const int& auClientWidth, const int& auClientHeight) = 0;
	virtual void OnKinectFound() = 0;
	virtual void OnOculusRiftFound() = 0;
	virtual bool OnControllerFound() = 0;
	virtual void OnControllerLost() = 0;
	
	// установить переход на URL
	virtual void SetLocationUrl(const wchar_t* apwcURL) = 0;
	virtual void SetLocationUrlAndTeleport(const wchar_t* apwcURL) = 0;
	virtual void SetLocationUrl(const wchar_t* apwcLocation, const wchar_t* apwcPlace
		, float x, float y, float z, unsigned long zoneID
		, const wchar_t* apwcAction, const wchar_t* apwcActionParam)=0;
	virtual const wchar_t* GetLocationURL()=0;

	//virtual void ChangeObjectBornReality( mlMedia* apMedia, unsigned int auBornRealityID) = 0;
	virtual void NotifyMapOnMyAvatarCreatedListeners() = 0;

	virtual const wchar_t* GetClientStringID() = 0;

//#ifndef CSCL
//	virtual void add_updatable (shared_ptr<Updatable> smp_new_updbl) = 0;
//#endif
	// существует ли определение для класса rmml-объектов
	virtual bool IsJSObjClassDefined(const wchar_t* apwcClassName)=0;
	// выводит инфу о ссылках на заданные объекты в trace
	// abLost - выводить инфу только по потерянным RMML-элементам (parent==null)
	// aiType - 0: все, 1: 3D-объекты, 2: 3D-персонажи, 3: 3D-группы, 4: композиции
	// apwcObjName - имя объекта
	virtual bool TraceObjectsRefs(bool abLost=true, int aiType=0, const wchar_t* apwcObjName=NULL)=0;
	// вывести в tracer инфу об объекте, который под мышкой
	virtual void TraceObjectUnderMouse()=0;
	// выдает видимый 2D-RMML-элемент, который находится под мышкой (для 3D-объектов, соответственно, viewport)
	virtual mlMedia* GetVisibleUnderNouse()=0;
	// установлена (или восстановлена) связь с одним из серверов
	virtual void OnServerConnected(int aServerMask, bool abRestored)=0; // mlServerMask
	// установка (или восстановление) связи с одним из серверов закончилось неудачно
	virtual void OnServerConnectionError(int aServerMask, int aiError)=0; // mlServerMask
	// потеряна связь с одним из серверов
	virtual void OnServerDisconnected(int aServerMask, int aiCause)=0; // mlServerMask

	virtual void SetSyncCallbacks();

	virtual void SetClientStringID(const wchar_t* apwcStringID)=0;
	virtual void SetServiceCallbacks() = 0;
	// вывести в поток определенное поддерево или 
	// все дерево RMML-объектов в целом в зависимости от режима (aiMode)
	//  aiMode: 0 - все дерево
	virtual void OutRMMLObjectTree(std::ostream* aOStream, int aiMode)=0;

	// вызвать процедуру добавления объекта под мышкой в галерею объектов
	virtual void CopyObjectUnderMouseToGallery()=0;

	// получить поддерево или 
	// все дерево RMML-объектов в целом в зависимости от режима (aiMode)
	//  aiMode: 0 - все дерево
	virtual void GetRMMLObjectTree( std::vector<mlObjectInfo> &avElements, int aiMode) = 0;

	// получить свойство синхронизированного объекта
	virtual std::wstring GetJsonFormatedProps(const wchar_t* apwcTarget, int aIndent = 0) = 0;
	//virtual bool HasBinaryState(const wchar_t* apwcTarget) = 0;
	// получить ID текущего сеанса
	virtual unsigned int GetCurSeanceID() = 0;
	// получить состояние текущего сеанса
	virtual bool isCurSeancePlaying() = 0;
	// установить состояние текущего сеанса
	virtual void SetSeancePlaying(bool playing) = 0;
	// приложение стало активным
	virtual void OnAppActivated() = 0;
	// приложение перестало быть активным
	virtual void OnAppDeactivated() = 0;
	// Приложение пытаются закрыть
	// Если вернет false, то приложение не надо закрывать.
	virtual bool OnClose() = 0;

	// вывести в tracer и лог глубины 2D-объекта под мышкой и его родителей
	virtual void TraceUnderMouseObjectDepth() = 0;

	//virtual void SetSynchObjectCount(int count) = 0;	

	// завершение телепортации
	virtual void OnPreciseURL(const wchar_t* aURL) = 0;

	virtual void OnSeancesInfo( const wchar_t* aSeancesInfo, bool bFull) = 0;

	//  Ошибка авторизации прокси сервера
	virtual void OnProxyAuthError( const wchar_t* aSettingName, int aProxyType, const wchar_t* aProxyServer) = 0;

	//  Ошибка соединения с сервером
	virtual void OnConnectToServerError(int errorCode) = 0;

	// Уведомляет, что с сервера пришли описания синхронизируемых объектов
	virtual void OnMapObjectsLoaded() = 0;

	// генерация события о показе зон синхронизации
	virtual void ShowZoneEvent( bool abVal) = 0;

	// код информации о загрузке мира
	virtual void SetInfoState( int iInfoState) = 0;

	// код ошибки при отмены авторизации
	virtual void OnLogout( int iCode) = 0;

	// вызывается, когда на окно плеера перетащили файл
	virtual void OnWindowFileDrop(const wchar_t* apwcFilePath, int x, int y) = 0;

	// вызывается, когда от пользователя не приходили больше 5 минут события мыши и клавиатуры или он свернул окно  (спящий режим)
	virtual void OnUserSleep() = 0;

	virtual void OnVideoCaptureError(const unsigned int errorCode) = 0;

	virtual void OnCodeEditorClose(const std::wstring& text) = 0;

	// вызывается, когда пользователь уходил в спящий режим и вернулся
	virtual void OnUserWakeUp() = 0;

	virtual void OnNotifyServerTOInfo( int minutes) = 0;

	// Пересчитывает бинарное состояние объекта. Вызывается только из Ctrl+I
	virtual void TransposeBinState(const wchar_t* apwcTarget) = 0;

	// вызвать сборщик мусора
	// если abAsynch == true, то по возможности в асинхроннном режиме
	virtual void CallJS_GC(bool abAsynch = true, bool abForceCall = false) = 0;

	virtual void OnEventsInfoIntoLocation( const wchar_t* aSeancesInfo) = 0;

	virtual void OnIPadSessionChangeSlide( int changeSlide) = 0;

	virtual void OnVisibilityDistanceKoefChanged(int koef) = 0;
	
	// результат выполнения операции Cut Редактора записей
	virtual void OnCutOperationComplete( int aErrorCode, unsigned int aRecordDuration) = 0;
	// результат выполнения операции Copy Редактора записей
	virtual void OnCopyOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции Paste Редактора записей
	virtual void OnPasteOperationComplete( int aErrorCode, unsigned int aRecordDuration) = 0;
	// результат выполнения операции Delete Редактора записей
	virtual void OnDeleteOperationComplete( int aErrorCode, unsigned int aRecordDuration) = 0;
	// результат выполнения операции Save Редактора записей
	virtual void OnSaveOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции SaveAs Редактора записей
	virtual void OnSaveAsOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции Play Редактора записей
	virtual void OnPlayOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции Pause Редактора записей
	virtual void OnPauseOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции Rewind Редактора записей
	virtual void OnRewindOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции Seek Редактора записей
	virtual void OnSeekOperationComplete( int aErrorCode) = 0;
	// результат выполнения операции Undo Редактора записей
	virtual void OnUndoOperationComplete( int aErrorCode, unsigned int aRecordDuration) = 0;
	// результат выполнения операции Redo Редактора записей
	virtual void OnRedoOperationComplete( int aErrorCode, unsigned int aRecordDuration) = 0;
	// результат выполнения операции Open Редактора записей
	virtual void OnOpenOperationComplete( int aErrorCode, std::wstring aRecordName, std::wstring aRecordDesciption, unsigned int aRecordDuration, std::wstring aAuthor, std::wstring aCreationDate, std::wstring aLocation, std::wstring aLanguage, std::wstring aURL) = 0;
	// установить версию 
	virtual void SetVersionInfo(wchar_t* version) = 0;
};

RMML_DECL omsresult CreateSceneManager(omsContext* apmcx);
RMML_DECL void DestroySceneManager(omsContext* amcx);

inline void AppendPointer(std::string& aStr, void* aPtr)
{
	char strPtr[ 10];
	aStr += "0x";
	_itoa_s((int)aPtr, strPtr, 10, 16);
	aStr += strPtr;
}

}

#endif