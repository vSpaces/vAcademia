#pragma once

// Received Type - типы принимаемых сообщений
#define RT_RecordStart					1  // запись началась
#define RT_RecordStop  					2  // остановка записи
#define RT_VoipServerRequest			3  // запрос информации о голосовом сервере
#define RT_VoipFileCreated				4  // создан файл
#define RT_RecordStartResult			5  // пришел код запуска EVO клиента
#define RT_RecordFinished				6  // клиент записи закрылся
#define RT_AudioPlayed					7  // аудио файл закончился
#define RT_MixedAudioCreated			8  // начал записываться очередной микшированный файл
#define RT_PlayFinished					9  // клиент воспроизведения закрылся
#define RT_PlaySessionConnected			10 // подключились к сессии воспроизведения
#define RT_AudioFilesInfo				11 // список файлов для скачивания
#define RT_EvoClientWorkStateChanged	12 // изменился статус удаленного EvoVOIP клиента
#define RT_Notified						13 // получена нотификация

// Коды нотификаций. См. RT_Notified и VoipSession::public const byte nm..
#define NC_SelfRecordStarted			1 // началась запись собственного голоса
#define NC_SelfRecordStopped			2 // закончилась запись собственного голоса
#define NC_SelfPlayingStarted			3 // началось воспроизведение записанного собственного голоса
#define NC_SelfPlayingStopped			4 // закончилось воспроизведение записанного собственного голоса

// Send Type - типы отправляемых сообщений
#define ST_RecordStart				1  // запись началась
#define ST_RecordStop  				2  // остановка записи
#define ST_ChangeServer				3  // смена сервера во время записи
#define ST_RecordPause 				4  // приостановка записи. Логи останавливаться не должны
#define ST_RecordDelete				5  // удаление записи
#define ST_PlaySessionStart			6  // начать сессию воспроизведения
#define ST_PlayUserAudioFile		7  // проиграть записанный файл в указанной сессии
#define ST_PlaySessionStop			8  // закончить сессию воспроизведения
#define ST_ChangePlayServer			9  // смена сервера во время воспроизведения
#define ST_SetUserName				10 // устанавливает на сервере имя своего пользователя
#define ST_ServerAudioCommand		11 // приостанавливает воспроизведение определенной записи
#define ST_CopyAudioFile			12 // копирует указанный файл из уже существующей записи в новую
#define ST_ForceSynchronization		13 // посылает событие принудительной синхронизации
#define ST_PauseUserAudio			14 // посылает запрос на остановку воспроизведения записанного звукового файла
#define ST_ResumeUserAudio			15 // посылает запрос на продолжение воспроизведения записанного звукового файла
#define ST_JoinToRecord				16 // посылает нотификацию о начале прослушивания записи. Нужно для того, чтобы получить ответ и зажечь микрофон
#define ST_RecordDeletes			17 // удаление записи
#define ST_RecordExport				18 // экспорт записи
#define ST_RecordExportFile			19 // эуспорт звукового файла из записи
#define ST_RecordImportFile			20 // импорт звукового файла в запись
#define ST_RemoveUserAudio			21 // убрать все файлы пользователя
#define ST_StartSelfRecording		22 // начать запись своего голоса
#define ST_ResumeSelfRecording		23 // подготовиться к записи своего голоса
#define ST_PauseSelfRecording		24 // остановить запись своего голоса
#define ST_PlaySelfRecording		25 // воспроизвести запись своего голоса
