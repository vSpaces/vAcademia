using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UI;

namespace RecordEditor.Logic
{
    /*
     * Класс формирует пакеты для общения с плеером
     */
    public class PlayerSession
    {
        //
        private PipeConnector m_pipeConnector = null;

        public PlayerSession()
        {
            m_pipeConnector = new PipeConnector();
            m_pipeConnector.Run();
        }

        /// <summary>
        /// Посылает запрос на открытие записи
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RDE_NOERROR
        /// RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR    OpenRecordRequest(string asURL)
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter data = new DataWriter();
            data.AddData( asURL);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_OPEN);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(asURL);

            m_pipeConnector.addOutCommand(outCommand);
            return RD_ERROR.RDE_NOERROR;
        }

        /// <summary>
        /// Посылает запрос на получение параметров записи
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RDE_NOERROR
        /// RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR    RecordInfoRequest( int auRecordID)
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter data = new DataWriter();
            data.AddData(auRecordID);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_GET_RECORD_INFO);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(data.GetData(), data.GetSize());

            m_pipeConnector.addOutCommand(outCommand);
            return RD_ERROR.RDE_NOERROR;
        }

        /// <summary>
        /// Посылает запрос на получение списка записей
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RDE_NOERROR
        /// RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR GetRecordsListRequest()
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter command = new DataWriter();
            command.AddData(Command.CMD_GET_RECORD_LIST);
            command.AddData((int)0);

            m_pipeConnector.addOutCommand(command);
            return RD_ERROR.RDE_NOERROR;
        }

        /// <summary>
        /// Посылает запрос на начало воспроизведения текущей записи
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RDE_NOERROR
        /// RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR PlayRequest()
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_PLAY);
            outCommand.AddData((int)0);

            m_pipeConnector.addOutCommand(outCommand);
            return RD_ERROR.RDE_NOERROR;
        }

        /// <summary>
        /// Посылает запрос на приостановку воспроизведения текущей записи
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RDE_NOERROR
        /// RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR PauseRequest()
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_PAUSE);
            outCommand.AddData((int)0);

            m_pipeConnector.addOutCommand(outCommand);
            return RD_ERROR.RDE_NOERROR;
        }

        /// <summary>
        /// Посылает запрос на переход на время aPosition в записи
        /// </summary>
        /// <param name="asURL"></param>
        /// <returns>
        /// RDE_NOERROR
        /// RDE_NOT_CONNECTED
        /// </returns>
        public RD_ERROR SeekRequest( uint aPosition)
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter data = new DataWriter();
            data.AddData(aPosition);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_SEEK);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(aPosition);

            m_pipeConnector.addOutCommand(outCommand);
            return RD_ERROR.RDE_NOERROR;
        }

        /*
         * Clipboard operations
         */
        public RD_ERROR CutSelection( TimeInterval aTimeInterval)
        {
            return RD_ERROR.RDE_NOT_IMPLEMENTED;
        }

        public RD_ERROR DeleteSelection(TimeInterval aTimeInterval)
        {
            if (!IsConnected())
                return RD_ERROR.RDE_NOT_CONNECTED;

            DataWriter data = new DataWriter();
            data.AddData(aTimeInterval.IntervalStart);
            data.AddData(aTimeInterval.IntervalEnd);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_DELETE_SELECTION);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(aTimeInterval.IntervalStart);
            outCommand.AddData(aTimeInterval.IntervalEnd);

            m_pipeConnector.addOutCommand(outCommand);
            return RD_ERROR.RDE_NOERROR;
        }

        public RD_ERROR PasteSelection(double aPasteTime, TimeInterval aTimeInterval)
        {
            return RD_ERROR.RDE_NOT_IMPLEMENTED;
        }

        #region Private methods

        bool IsConnected()
        {
            return m_pipeConnector != null && m_pipeConnector.IsClientConnected();
        }

        #endregion // Private methods
    }
}
