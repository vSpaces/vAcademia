#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlRMMLPath3D_H_
#define _mlRMMLPath3D_H_

namespace rmml {

/**
 * Класс пути перемещения для 3D-объектов RMML
 */

class mlRMMLPath3D :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public moIPath3D
{
public:
	mlRMMLPath3D(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLPath3D(void);
MLJSCLASS_DECL

	JSFUNC_DECL(getPosition) // (расчет по сплайну (учитывается изгиб ("ускорение")))
	JSFUNC_DECL(getLinearPosition) // (расчет по линии (если вытянуть сплайн в линию))
	JSFUNC_DECL(getNormal) // получить нормаль к определенной позиции (0..1) на пути
	JSFUNC_DECL(getTangent) // получить касательную к определенной позиции (0..1) на пути
	JSFUNC_DECL(getNormalByLinearPos) // получить нормаль к определенной линейной позиции (0..1) на пути
	JSFUNC_DECL(getTangentByLinearPos) // получить касательную к определенной линейной позиции (0..1) на пути

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация moIPath3D
public:
	ml3DPosition getPosition(double adKoef);
	ml3DPosition getLinearPosition(double adKoef);
	ml3DPosition getNormal(double adKoef);
	ml3DPosition getTangent(double adKoef);
	ml3DPosition getNormalByLinearPos(double adKoef);
	ml3DPosition getTangentByLinearPos(double adKoef);
};

}

#endif
