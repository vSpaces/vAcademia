#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlUtilityThread.h"
#include "config/prolog.h"

#include "Windows.h"
#include "crtdbg.h"
#include "ViskoeThread.h"

namespace rmml {

	class mlUtilityThread::mlUThreadInternal:
		public ViskoeThread::CThreadImpl<mlUtilityThread::mlUThreadInternal>
	{
		JSContext* mcxOwn; // собственный контекст
		CEvent mevSleep;
		JSRuntime* mrt;
		CCriticalSection mcsElemsToDelete;
	public:
		mlUThreadInternal(JSRuntime* art):mrt(art){
			mcxOwn = NULL;
			mevSleep.Create();
			mcsElemsToDelete.Init();
		}
		~mlUThreadInternal(){
			mcsElemsToDelete.Term();
		}

		DWORD Run(){
			mcxOwn = JS_NewContext(mrt, 8192);
			if(mcxOwn == NULL)
				return 1;
			while( !ShouldStop() ) {
				mevSleep.WaitForEvent();
				if(ShouldStop())
					break;
				JS_GC(mcxOwn);
			}
			JS_DestroyContext(mcxOwn);
			mcxOwn = NULL;
			return 0;
		}
		bool MayBeCalled(){
			return (mcxOwn != NULL);
		}
		void StartGC(){
			//mevSleep.PulseEvent();
			mevSleep.SetEvent();
		}
		void LockElemsToDelete(){
			mcsElemsToDelete.Lock();
		}
		void UnlockElemsToDelete(){
			mcsElemsToDelete.Unlock();
		}

	};

	void mlUtilityThread::LockElemsToDelete(){
		mpThread->LockElemsToDelete();
	}

	void mlUtilityThread::UnlockElemsToDelete(){
		mpThread->UnlockElemsToDelete();
	}

	mlUtilityThread::mlUtilityThread(JSRuntime* art){
		mpThread.reset(new mlUThreadInternal(art));
		mpThread->Start();

	}

	mlUtilityThread::~mlUtilityThread(){
		mpThread->SignalStop();
		mpThread->StartGC();
		mpThread->Stop();
	}

	bool mlUtilityThread::StartGC(){
		if(!(mpThread->MayBeCalled()))
			return false;
		mpThread->StartGC();
		return true;
	}
}

