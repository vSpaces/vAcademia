using System;
using System.Linq;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace RecordEditor
{
    class MessageHandlerManager
    {
        /// Делегат обработчика сообщения 
        /// </summary>
        /// <param name="dataReader">объект для чтения данных</param>
        public delegate void MessageHandler(DataReader dataReader);

        private MessageHandler[] handlers = null;

        protected RecordEditor m_recordEditor;

        public MessageHandlerManager(RecordEditor recordEditor)
        {
            m_recordEditor = recordEditor;
            InitHandlers();
        }

        public void HandleMessage(byte type, DataReader dataReader)
        {
            System.Diagnostics.Debug.Assert(type < handlers.Length);
            System.Diagnostics.Debug.Assert(type >= 0);

            handlers[type](dataReader);
        }

        private void InitHandlers()
        {
            handlers = new MessageHandler[17];
            handlers[Command.CMD_ON_UNKNOWN] = new MessageHandler(OnIdle);
            handlers[Command.CMD_ON_IDLE] = new MessageHandler(OnIdle);
            handlers[Command.CMD_ON_HANDSHAKE] = new MessageHandler(OnIdle);
            handlers[Command.CMD_ON_GET_RECORD_LIST] = new MessageHandler(OnGetRecordList);
            handlers[Command.CMD_ON_EXIT] = new MessageHandler(OnExit);
            handlers[Command.CMD_ON_OPEN] = new MessageHandler(OnOpenRecordResult);
            handlers[Command.CMD_ON_GET_RECORD_INFO] = new MessageHandler(OnGetRecordInfo);

        }

        private void OnExit(DataReader dataReader)
        {

        }

        private void OnGetRecordList(DataReader dataReader)
        {
            int recordsTotal = 0;
            int recordsCount = 0;

            dataReader.Read(ref recordsTotal);
            dataReader.Read(ref recordsCount);

            ArrayList records = new ArrayList();
            for (int i = 0; i < recordsCount; ++i)
            {
                RecordDescription desc = new RecordDescription();

                dataReader.Read(ref desc.recordID);
                dataReader.Read(ref desc.realityID);
                dataReader.Read(ref desc.duration);
                dataReader.ReadUnicode(ref desc.name);
                dataReader.ReadUnicode(ref desc.author);
                dataReader.ReadUnicode(ref desc.creationTime);
                dataReader.ReadUnicode(ref desc.location);
                dataReader.ReadUnicode(ref desc.lesson);
                dataReader.ReadUnicode(ref desc.description);
                dataReader.ReadUnicode(ref desc.URL);

                records.Add(desc);
            }

            m_recordEditor.AddRecordsToList(records);

        }

        private void OnIdle(DataReader dataReader)
        {

        }

        private void OnOpenRecordResult(DataReader dataReader)
        {
            bool opened = false;
            dataReader.Read(ref opened);
        }

        private void OnGetRecordInfo(DataReader dataReader)
        {
            RecordDescription desc = new RecordDescription();

            dataReader.Read(ref desc.recordID);
            dataReader.Read(ref desc.realityID);
            dataReader.Read(ref desc.duration);
            dataReader.ReadUnicode(ref desc.name);
            dataReader.ReadUnicode(ref desc.author);
            dataReader.ReadUnicode(ref desc.creationTime);
            dataReader.ReadUnicode(ref desc.location);
            dataReader.ReadUnicode(ref desc.lesson);
            dataReader.ReadUnicode(ref desc.description);
            dataReader.ReadUnicode(ref desc.URL);

            m_recordEditor.OpenRecord(desc.URL);

            //            labelDurationValue.Text = Convert.ToString(desc.duration);
        }
    }
}
