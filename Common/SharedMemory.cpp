#include "mumble_pch.hpp" 
//#include "stdafx.h"
#include "SharedMemory.h"
#include "windows.h"
#include "winuser.h"
#include <string>

#ifndef HANDLE
typedef void* HANDLE;
#endif

using namespace std;

typedef unsigned int SMBSize; // тип дл€ размера блока раздел€емой пам€ти

/*
 *  ласс дл€ работы с Shared Memory
 */
class CSharedMemoryBlock: public ISharedMemoryBlock
{
	wstring m_sStringID; // идентификатор shared memory в системе
	HANDLE m_hGlobalMemoryObject; // handle блока shared memory 
	SharedMemoryBlockPtr m_pPointer;	// указатель на самое начало блока пам€ти
	SharedMemoryBlockPtr m_pDataPointer;	// указатель, возвращенный через последний GetPointer
	SMBSize m_uSize;	// размер блока раздел€емой пам€ти
	int m_iLastError;		// ошибка последней операции
public:
	// alpcStringID - идентификатор shared memory
	CSharedMemoryBlock(const wchar_t* apwcStringID, unsigned int auiSize = 0);
	virtual ~CSharedMemoryBlock();

// реализаци€ ISharedMemoryBlock
public:
	// ѕолучить размер блока раздел€емой пам€ти
	virtual unsigned int GetSize(){ return m_uSize; }

	// ѕолучить указатель на блок раздел€емой пам€ти
	virtual SharedMemoryBlockPtr GetPointer();

	// ќсвободить указатель на блок раздел€емой пам€ти
	virtual void FreePointer();

	// получить код ошибки последней операции
	virtual int GetLastError();

	// получить строковый идентификатор блока
	virtual const wchar_t* GetStringID(){ return m_sStringID.c_str(); }

	// ќтпустить интерфейс
	virtual void Release(); 

private:
	// определ€ет размер блока раздел€емой пам€ти
	void DefineSize();

	// —охран€ет размер в блок раздел€емой пам€ти
	//void SaveSize();

	// ¬ытаскивает размер блока раздел€емой пам€ти из нее самой
	// ¬озвращает указатель на сами данные
	//SharedMemoryBlockPtr LoadSize(SharedMemoryBlockPtr apPointer);

	// ѕолучить указатель на самое начало блока пам€ти
	//SharedMemoryBlockPtr GetSM0Pointer();

	// ѕо указателю на данные возвращает указатель на самое начало блока пам€ти
	//SharedMemoryBlockPtr UnshiftTo0(SharedMemoryBlockPtr apDataPointer);
};

// alpcStringID - идентификатор shared memory
// auiSize - размер блока (если == 0, то открываетс€ существующий блок)
CSharedMemoryBlock::CSharedMemoryBlock(const wchar_t* alpcStringID, unsigned int auiSize)
{
	m_sStringID = alpcStringID;
	m_hGlobalMemoryObject = NULL;
	m_uSize = 0;
	m_pPointer = NULL;
	m_pDataPointer = NULL;
	m_iLastError = ERROR_SUCCESS;
	if(auiSize > 0){
		m_hGlobalMemoryObject = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, auiSize, m_sStringID.c_str());
		m_iLastError = ::GetLastError();
		// если блок пам€ти с таким ID уже есть, 
		if(m_iLastError == ERROR_ALREADY_EXISTS){
			this->GetPointer();
			this->FreePointer();
			// но его размер другой
			if(m_uSize < auiSize){
				// то считаем, что создать блок не получилось
				if (m_hGlobalMemoryObject != INVALID_HANDLE_VALUE && m_hGlobalMemoryObject != NULL)
					CloseHandle(m_hGlobalMemoryObject);
				m_hGlobalMemoryObject = INVALID_HANDLE_VALUE;
				m_iLastError = ERROR_INVALID_HANDLE;
				m_uSize = 0;
			}
		}else{
			this->GetPointer();
			if(m_pPointer != NULL){
				*((SMBSize*)m_pPointer) = auiSize;
				m_uSize = auiSize;
			}
			this->FreePointer();
		}
	}else{
		m_hGlobalMemoryObject = ::OpenFileMappingW(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			m_sStringID.c_str());               // name of mapping object
		m_iLastError = ::GetLastError();
		// узнаем размер блока раздел€емой пам€ти
		this->DefineSize();
	}
}

// определ€ет размер блока раздел€емой пам€ти
void CSharedMemoryBlock::DefineSize(){
	if(m_hGlobalMemoryObject == INVALID_HANDLE_VALUE || m_hGlobalMemoryObject == NULL){
		m_uSize = 0;
		return;
	}
	// размер извлекаетс€ из самой пам€ти в GetPointer()
	this->GetPointer();
	this->FreePointer();
}

CSharedMemoryBlock::~CSharedMemoryBlock(){
	FreePointer();
	if (m_hGlobalMemoryObject)
		CloseHandle(m_hGlobalMemoryObject);
}

// ѕолучить указатель на блок раздел€емой пам€ти
SharedMemoryBlockPtr CSharedMemoryBlock::GetPointer()
{
	m_iLastError = ERROR_SUCCESS;
	if(m_pDataPointer != NULL)
		return m_pDataPointer;
	m_pPointer = NULL;
	m_pDataPointer = NULL;
	if(m_hGlobalMemoryObject == INVALID_HANDLE_VALUE || m_hGlobalMemoryObject == NULL){
		m_iLastError = ERROR_INVALID_HANDLE;
		return NULL;
	}
	m_pPointer = (SharedMemoryBlockPtr)::MapViewOfFile(m_hGlobalMemoryObject, FILE_MAP_ALL_ACCESS, 0, 0, m_uSize);
	if(m_pPointer == NULL){
		m_iLastError = ::GetLastError();
		return NULL;
	}
	m_uSize = *((SMBSize*)m_pPointer);
	m_pDataPointer = ((SMBSize*)m_pPointer)+1;
	return m_pDataPointer;
}

// ќсвободить указатель на блок раздел€емой пам€ти
void CSharedMemoryBlock::FreePointer()
{
	if(m_pPointer == NULL)
		return;

	::UnmapViewOfFile(m_pPointer); 
	m_iLastError = ::GetLastError();
	m_pPointer = NULL;
	m_pDataPointer = NULL;
}

// получить код ошибки последней операции
int CSharedMemoryBlock::GetLastError()
{
	return m_iLastError;
}

// ќтпустить интерфейс
void CSharedMemoryBlock::Release()
{
	delete ((CSharedMemoryBlock*)this);
}

ISharedMemoryBlock* CSharedMemory::CreateBlock(const wchar_t* apwcStringID, unsigned int aiSize)
{
	return (ISharedMemoryBlock*)(new CSharedMemoryBlock(apwcStringID, aiSize));
}

ISharedMemoryBlock* CSharedMemory::GetBlock(const wchar_t* apwcStringID)
{
	return (ISharedMemoryBlock*)(new CSharedMemoryBlock(apwcStringID));
}

