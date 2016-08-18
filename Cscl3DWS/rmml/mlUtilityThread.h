#pragma once

namespace rmml {

/*
  Класс вспомогательного потока.
  Используется для асинхронной работы GC.
  Очередной тик потока вызывыется по окончании Update-а SceneManager-а, 
  если предыдущий тик закончил свою работу.
*/
class mlUtilityThread{
	friend class mlGuardElemsToDelete;
	class mlUThreadInternal;
	std::auto_ptr<mlUThreadInternal> mpThread;

private:
	void LockElemsToDelete();
	void UnlockElemsToDelete();

public: 
	mlUtilityThread(JSRuntime* art);
	~mlUtilityThread();

	bool StartGC();
};

// защитить список элементов на удаление от изменений другим потоком
class mlGuardElemsToDelete
{
	mlUtilityThread* mpUtilityThread;
public:
	mlGuardElemsToDelete(mlUtilityThread* apUtilityThread){
		mpUtilityThread = apUtilityThread;
		mpUtilityThread->LockElemsToDelete();
	}
	~mlGuardElemsToDelete(){
		mpUtilityThread->UnlockElemsToDelete();
	}
};

}