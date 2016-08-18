using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace RecordEditor.Logic
{
    /*
     * Класс реализует логику открытия, закрытия записи. Отслеживает возможные ошибки доступа
     * к записи, определяет ошибки серверного соединения.
     */
    class RecordController
    {
        /// <summary>
        /// Вызывается после успешного открытия записи
        /// </summary>
        /// <returns></returns>
        public delegate void OnRecordOpenSuccessDelegate();
        public event RecordControllerEventHandler OnRecordOpenSuccess = null;

        /// <summary>
        /// Вызывается при невозможности открытия записи
        /// </summary>
        /// <returns></returns>
        public event RecordControllerEventHandler OnRecordOpenFailed = null;

        /// <summary>
        /// Вызывается перед закрытием записи
        /// </summary>
        /// <returns></returns>
        public event RecordControllerEventHandler OnRecordClosed = null;

        /// <summary>
        /// Вызывается при обновлении списка записей
        /// </summary>
        /// <returns></returns>
        public event RecordControllerEventHandler OnRecordListChanged = null;

        #region Private members

        static int INCORRECT_RECORD_ID = 0xFFFFFFF;

        // открываемая запись
        int openingRecordID = INCORRECT_RECORD_ID;

        // ссылка на текущую открытую запись
        RecordDocument currentRecord = null;

        // Список записей
        ArrayList m_recordList = new ArrayList();

        #endregion // Private members

        public RecordController()
        {

        }

        #region Public properties

        public ArrayList RecordsList
        {
            get
            {
                return m_recordList;
            }
            set
            {
                if (value == null)
                {
                    m_recordList.Clear();
                }
                else
                {
                    m_recordList.AddRange(value);
                }
                if (OnRecordListChanged != null)
                    OnRecordListChanged(this, new RecordControllerEventArgs());
            }
        }

        public RecordDocument CurrentRecord
        {
            get
            {
                return currentRecord;
            }
        }

        #endregion // Public properties

        #region Public methods

        /// <summary>
        /// Открывает запись по указанному номеру
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RD_ERROR.RDE_NOERROR
        /// RD_ERROR.RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR OpenRecord( int auRecordID)
        {
            if (openingRecordID == auRecordID)
                return RD_ERROR.RDE_NOERROR;

            openingRecordID = auRecordID;
            return Program.context.playerSession.RecordInfoRequest( auRecordID);
        }

        public RD_ERROR UpdateRecordInfo(RecordDescription apDesc)
        {
            if (openingRecordID != apDesc.recordID)
                return RD_ERROR.RDE_COMMAND_OBSOLETE;

            System.Diagnostics.Debug.Assert(currentRecord == null);

            RecordDescription recordDescription = GetRecordDescription(apDesc.recordID);
            if (recordDescription != null)
            {
                recordDescription.Set(apDesc);

                currentRecord = new RecordDocument();
                currentRecord.Description = recordDescription;
                return OpenRecordImpl(currentRecord);
            }
            return RD_ERROR.RDE_COMMAND_OBSOLETE;
        }

        public RD_ERROR OpenRecordImpl(RecordDocument apRecord)
        {
            if (apRecord == null)
                return RD_ERROR.RDE_INCORRECT_PARAMETERS;

            if (apRecord.Description == null)
                return RD_ERROR.RDE_INCORRECT_PARAMETERS;

            if (openingRecordID != apRecord.Description.recordID)
                return RD_ERROR.RDE_COMMAND_OBSOLETE;

            string asURL = apRecord.Description.URL + "&em=1";
            return Program.context.playerSession.OpenRecordRequest(asURL);
        }

        public RD_ERROR OnRecordOpenedStatus( bool anOpened)
        {
            if( anOpened == true)
            {
                OnRecordOpenSuccessDelegate pfn = Fire_OnRecordOpenSuccess;
                Program.context.recordEditor.Invoke(pfn);
            }
            else
            {
                currentRecord = null;
                openingRecordID = INCORRECT_RECORD_ID;
                if (OnRecordOpenFailed != null)
                    OnRecordOpenFailed(this, new RecordControllerEventArgs());
            }
            return RD_ERROR.RDE_NOERROR;
        }

        public void UpdateRecordsList()
        {
            Program.context.playerSession.GetRecordsListRequest();
        }

        #endregion // Public methods

        #region Private methods

        RecordDescription GetRecordDescription(int auRecordID)
        {
            foreach (RecordDescription recordDescription in m_recordList)
            {
                if( recordDescription.recordID == auRecordID)
                    return recordDescription;
            }
            return null;
        }

        void Fire_OnRecordOpenSuccess()
        {
            if (OnRecordOpenSuccess != null)
                OnRecordOpenSuccess( CurrentRecord, new RecordControllerEventArgs());            
        }

        #endregion // Private methods

    }
}
