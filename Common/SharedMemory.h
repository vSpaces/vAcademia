/*
	»нтерфейс к механизму работы с раздел€емой (между процессами) пам€тью

	ѕример использовани€:
		ѕроцесс-"сервер": 
			// создаем блок раздел€емой пам€ти с именем "SharedMemBlockName1" и размером - 256 байт
			ISharedMemoryBlock* sharedMemBlock = CSharedMemory::CreateBlock(L"SharedMemBlockName1", 256);
			... (по мере необходимости:)
			// ѕолучаем указатель на блок раздел€емой пам€ти
			int* pMemory = (int*)sharedMemBlock->GetPointer();
			// «аполн€ем блок какими-то значени€ми
			int iMemSize = sharedMemBlock->GetSize();
			for(int i = 0; i < iMemSize; i++)
				pMemory[i] = i;
			// разрешаем другому процессу работать с этим блоком пам€ти
			sharedMemBlock->FreePointer();
			...
			// больше не будем работать с этим блоком пам€ти
			sharedMemBlock->Release();

		ѕроцесс-"клиент": 
			// находим блок раздел€емой пам€ти с именем "SharedMemBlockName1"
			ISharedMemoryBlock* sharedMemBlock = CSharedMemory::GetBlock(L"SharedMemBlockName1");
			...
			// ѕолучаем указатель на блок раздел€емой пам€ти
			int* pMemory = (int*)sharedMemBlock->GetPointer();
			// »звлекаем значени€ из блока
			int iMemSize = sharedMemBlock->GetSize();
			int* values = new int[iMemSize];
			for(int i = 0; i < iMemSize; i++)
				values[i] = pMemory[i];
			delete[] values;
			// разрешаем другому процессу работать с этим блоком пам€ти
			sharedMemBlock->FreePointer();
			...
			// больше не будем работать с этим блоком пам€ти
			sharedMemBlock->Release();
*/
#include "stdafx.h"

typedef void* SharedMemoryBlockPtr;

// значени€, возвращаемые ISharedMemoryBlock::GetLastError() (соответствуют системным)
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0
#define ERROR_INVALID_HANDLE 6
#endif

/*
 * »нтерфейс доступа к блоку раздел€емой пам€ти
 */
struct ISharedMemoryBlock
{
	// ѕолучить размер блока раздел€емой пам€ти
	virtual unsigned int GetSize() = 0;

	// ѕолучить указатель на блок раздел€емой пам€ти
	virtual SharedMemoryBlockPtr GetPointer() = 0;

	// ќсвободить указатель на блок раздел€емой пам€ти
	virtual void FreePointer() = 0;

	// получить код ошибки последней операции
	virtual int GetLastError() = 0;

	// получить строковый идентификатор блока
	virtual const wchar_t* GetStringID() = 0;

	// ќтпустить интерфейс
	virtual void Release() = 0; 
};

class CSharedMemory
{
public:
	/*
	 * ѕолучить интерфейс к вновь созданному именованному блоку раздел€емой пам€ти
	 */
	static ISharedMemoryBlock* CreateBlock(const wchar_t* apwcStringID, unsigned int aiSize);

	/*
	 * ѕолучить интерфейс к именованному блоку раздел€емой пам€ти
	 */
	static ISharedMemoryBlock* GetBlock(const wchar_t* apwcStringID);
};

