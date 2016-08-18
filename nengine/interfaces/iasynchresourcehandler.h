#pragma once

/*
Интерфейс получения событий загрузки асинхронного объекта
*/

#include "iasynchresource.h" 

struct IAsynchResourceHandler
{
	// получает прогресс загрузки объекта
	virtual void OnAsynchResourceLoadedPersent(IAsynchResource* asynch, unsigned char percent) = 0;
	// объект загружен
	virtual void OnAsynchResourceLoaded(IAsynchResource* asynch) = 0;
	// ошибка загрузки
	virtual void OnAsynchResourceError(IAsynchResource* asynch) = 0;
};