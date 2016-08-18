#pragma once

/*
Интерфейс получения событий выполнения анимаций 3Д объекта
*/

struct IAnimationEventsHandler
{
	// анимация выполнена
	virtual void OnAnimationComplete() = 0;
	// анимация установлена
	virtual void OnAnimationSet() = 0;
};