// Общие для всех менеджеров константы

// Имя ini-файла
#define INI_FILE_NAME _T( "setup.ini")

// Общая секция в ini-файле
#define INI_GENERAL _T( "general")
// Имя параметра в  ini-файле, который содержит путь к ключу в регистре для приложения
#define INI_REG_KEY _T( "regSection")

#define MAX_CACHE_SIZE	300000000
	
// Ошибки запросов
enum EQueryError {
	qeNoError = 0, qeServerError, qeQueryNotFound, qeScriptError, qeReadError,
	qeNotImplError, qeServerNotResolved, qeTimeOut, qeNotExistFileReadError
};
