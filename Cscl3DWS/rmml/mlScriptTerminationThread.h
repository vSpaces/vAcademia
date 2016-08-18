#pragma once

namespace rmml {

/*
  Класс потока для прерывания зависших скриптов.
*/
class mlScriptTerminationThread{
	class mlSTThreadInternal;
	std::auto_ptr<mlSTThreadInternal> mpThread;

public: 
	mlScriptTerminationThread(JSContext* acx);
	~mlScriptTerminationThread();

	void ScriptExecutionStarted(void* aCurrentScript);

	void ScriptExecuted(void* aCurrentScript);

	int GetExceededTimeout();

};

}