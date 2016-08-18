
#if !defined(AFX_ICLIENTSESSION_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_)
#define AFX_ICLIENTSESSION_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_

namespace cs
{
	struct imessagehandler;

	struct isessionlistener
	{
		virtual void OnConnectLost() = 0;
		virtual void OnConnectRestored() = 0;
	};

	struct iclientsession
	{
		virtual int GetSendQueueSize() = 0;
		virtual int GetStatus() = 0;

		// ѕосылает на сервер данные, которые не требуют ответа
		virtual void Send(unsigned char aType, const unsigned char FAR *aData, int aDataSize) = 0;
		// ќтправл€ет сообщени€ c сохранением отправленных дл€ ожидани€ ответа на них
		virtual void SendQuery(unsigned char aType, const unsigned char FAR *aData, int aDataSize) = 0;
		// ѕосылает на сервер данные, вперед всех сообщений очереди. 
		// ѕеред посылкой удал€ет все сообщени€ с указанным типом.
		virtual void SendSignalQuery(unsigned char aType, const unsigned char FAR *aData, int aDataSize) = 0;
		// ”дал€ет запрос из очередей
		virtual void RemoveQueryByType(unsigned char aType) = 0;
		// ”становить интерфейс дл€ обработки сообщений
		virtual void SetMessageHandler(imessagehandler* pMesHandler) = 0;
		// ”становить интерфейс дл€ обработки событий разрыва и восстановлени€ св€зи
		virtual void SetSessionListener(isessionlistener* pListener) = 0;
		// »зменились параметры сетевых настроек
		virtual void ChangeConnection() = 0;
		// —есси€ должна сэмулировать разрыв св€зи
		virtual void EmulateBreak( /*int auConnectionMask*/) = 0;
		// ограничение максимального размера очереди пакетов в байтах
		virtual void SetMaxQueueSize( int aMaxQueueSize) = 0;
	};

	struct iclientsynchsession
	{		
		virtual int SendAndReceive(unsigned char aType, const unsigned char FAR *aData, int aDataSize, DWORD timeout = INFINITE, int* aQueryID = NULL) = 0;
	};
};

#endif