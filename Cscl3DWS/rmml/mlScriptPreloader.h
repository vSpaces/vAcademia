#pragma once

namespace rmml {

// класс информации о скрипте
class mlScriptInfo{
public:
	MP_WSTRING mwsSrc;
	bool mbInclude;	// =true, если скрипт является include-ом по отношению к родительскому скрипту
public:
	mlScriptInfo(const wchar_t* apwcSrc):
		MP_WSTRING_INIT(mwsSrc)
	{
		if(apwcSrc != NULL && *apwcSrc != L'\0')
			mwsSrc = apwcSrc;
		mbInclude = false;
	}
//	~mlScriptInfo();
};

class mlSceneManager;

// Класс объекта, производящего учет предзагружаемых include-ов
class mlScriptPreloader
{
public:
	// класс информации об include-е
	class mlIncludeInfoInternal: public mlScriptInfo{
	public:
		typedef MP_VECTOR<mlIncludeInfoInternal*> VecIncludeInfo;
		VecIncludeInfo mvIncludes;
		mlSceneManager* mpSceneManager;
		//mlScriptInfoInternal* mpParent;
		bool mbExtractResult; // получилось ли извлечь список include-ов?
		bool mbIncludesExtracted; // произведена попытка извлечь include-ы из скрипта
		bool mbAllSubIncludesExtracted; // произведена попытка извлечь include-ы из всех include-ов
		bool mbLoaded; // скрипт загружен (в том числе и скомпилирован)
	public:

		static void startElementNs(void* ctx ATTRIBUTE_UNUSED,
			const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
			int nb_namespaces, const xmlChar** namespaces, int nb_attributes,
			int nb_defaulted, const xmlChar** attributes);
		static void endElementNs(void* ctx ATTRIBUTE_UNUSED,
			const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI);

	private:
public: // временно для отладки
		// проверяет наличие скрипта в собственном списке include-ов
		bool FindInclude(const mlScriptInfo& aScriptInfo);
private:
		// находит указанный скрипт в собственном списке include-ов
		mlIncludeInfoInternal* FindIncludeInternal(const mlScriptInfo& aScriptInfo);
	public:
		mlIncludeInfoInternal(const mlScriptInfo& aScriptInfo, mlSceneManager* apSceneManager);
		virtual ~mlIncludeInfoInternal();

		// предзагрузка include-ов указанного скрипта больше не требуется: убрать из списка
		//bool RemoveScript(const mlScriptInfo& aScriptInfo);

		// извлечь из файла mwsSrc список include-ов
		bool ExtractIncludes();

		// загружены ли все include-ы для указанного скрипта?
		// возвращает -1, если скрипт не найден
		//int IsIncludesPreloadedFor(const mlScriptInfo& aScriptInfo);
	protected:
		// собрать все include-ы в один список, удаляя повторения
		//void CollectIncludesIntoOneList();

		// ищем в дереве непросканенный include и сканим
		virtual bool ExtractOneInclude();

		// ищем незагруженный include и грузим его
		virtual bool LoadOneInclude(mlScriptPreloader* apScriptPreloader);
	};

	// класс информации о скрипте
	class mlScriptInfoInternal: public mlIncludeInfoInternal{
	public:
		bool mbNeedToBeLoaded; // скрипт нужно загрузить
		bool mbAllIncludesLoaded; // все include-ы загружены
	public:
		mlScriptInfoInternal(const mlScriptInfo& aScriptInfo, mlSceneManager* apSceneManager);
		~mlScriptInfoInternal();

		// ищем в дереве непросканенный include и сканим
		virtual bool ExtractOneInclude();

		// ищем незагруженный include и грузим его
		virtual bool LoadOneInclude(mlScriptPreloader* apScriptPreloader);

		// загружены ли все include-ы для этого скрипта?
		bool IsIncludesAllPreloaded();
	};

private:
	mlSceneManager* mpSceneManager;

	typedef MP_VECTOR<MP_WSTRING> VecExtractedIncludeInfo;
	VecExtractedIncludeInfo mvExtractedIncludes;

	//// корневая запись: содержит список скритов, для которых подгружаются include-ы
	//mlScriptInfoInternal mRoot;
	typedef MP_VECTOR<mlScriptInfoInternal*> VecScriptInfo;
	VecScriptInfo mvScripts;

	// индекс скрипта (в mRoot.mvIncludes), 
	// загрузку которого надо продолжить на следующем update-е
	int miNextScriptToProcessIdx;

	// мапа вообще всех загруженных за время работы программы include-ов
	typedef MP_MAP<MP_WSTRING, int> MapAllIncludedScripts;
	MapAllIncludedScripts mAllIncludedScripts; 

public:
	mlScriptPreloader(mlSceneManager* apSceneManager);
	~mlScriptPreloader();

private:
	bool UpdateInternal();

	// найти скрипт в списке
	VecScriptInfo::iterator FindScriptInternal(const mlScriptInfo& aScriptInfo);

	// привести путь к скрипту к унифицированному виду
	// чтобы он был одинаковым независимо от того, откуда он загружается
	mlString NormalizeSrcPath(const wchar_t* apwcSrc);

public:
	// добавить скрипт для предзагрузки его include-ов
	// если abPreloadIncludes=true, то загружать только include-ы, а сам скрипт нет
	// возвращает true, есть скрипт действительно добавлен (есть include-ы, которые можно грузить)
	bool AddScript(const mlScriptInfo& aScriptInfo, bool abPreloadIncludes);

	// предзагрузка include-ов указанного скрипта больше не требуется: убрать из списка
	bool RemoveScript(const mlScriptInfo& aScriptInfo);

	// загружены ли все include-ы для указанного скрипта?
	int IsIncludesPreloadedFor(const mlScriptInfo& aScriptInfo);

	// Выполнить очередной шаг предзагрузки
	// aiTimeQuota (ms) - квота по времени, которая отведена под очередной шаг предзагрузки
	void Update(int aiTimeQuota);

	// Добавить скрипт в мапу всех загруженных include-ов
	void RegIncludedScript(const wchar_t* apwcSrc);

	// Выдать true, если скрипт уже загружался в процессе работы программы
	bool IsAlreadyIncluded(const wchar_t* apwcSrc);

	// Выдать инклуды по указанному файлу
	int ExtractIncludes( const wchar_t* apcFileSrc, const wchar_t** appIncludes, int count);
};

}
