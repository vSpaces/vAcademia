using System;
using System.Data;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;
using FlashGenie;

/*
 * Запись
 *  1. Открыть
 *  2. Закрыть
 *  3. Сохранить
 *  4. Сохранить как..
 *  5. Выход
 *  
 * Правка
 *  1. Скопировать
 *  2. Вырезать
 *  3. Вставить
 *  4. Удалить
 *  4. Отмена
 *  5. Повтор
 */

namespace RecordEditor
{
    //public partial class RecordEditor : Form
    public partial class RecordEditor : MainFormDialog
    {
        static string MAIN_WINDOW_TITLE = "Редактор занятий: ";
        // позиция проигрывания
        private uint m_playPosition;
        // позиция начала выделения
        private uint m_beginSelectPosition;
        // позиция конца выделения
        private uint m_endSelectPosition;
        // продолжительность
        private uint m_duration;
        // признак копирования
        private bool m_isCopy;
        // позиция начала выделения
        private uint m_beginClipboardPos;
        // позиция конца выделения
        private uint m_endClipboardPos;

        //private MessageHandlerManager m_messageHandlerManager;

        private ArrayList m_recordList = new ArrayList();

        // диалог выбора занятия для открытия
        private OpenRecordList m_openRecordListDialog;

        public delegate void UpdateTableContent(DataTable dataTable);

        #region 
        #endregion

        public RecordEditor()
        {
            m_openRecordListDialog = new OpenRecordList(this);
            //Program.context.pipeConnector = new PipeConnector(this);
            //m_messageHandlerManager = new MessageHandlerManager(this);

            InitializeComponent();

            CenterToScreen();

            m_duration = 0;
        }

        public override string GetSkinFileName()
        {
            return SkinConsts.MainFormSkinFileName;
        }

        #region Public methods

        public void SubscribeEvents()
        {
            m_openRecordListDialog.SubscribeEvents();
            timeLine.SubscribeEvents();

            Program.context.recordController.OnRecordOpenSuccess += new global::RecordEditor.Logic.RecordControllerEventHandler(recordController_OnRecordOpenSuccess);
            Program.context.recordController.OnRecordClosed += new global::RecordEditor.Logic.RecordControllerEventHandler(recordController_OnRecordClosed);
        }


        void recordController_OnRecordClosed(object sender, global::RecordEditor.Logic.RecordControllerEventArgs args)
        {
            
        }

        void recordController_OnRecordOpenSuccess(object sender, global::RecordEditor.Logic.RecordControllerEventArgs args)
        {
            Title = MAIN_WINDOW_TITLE + Program.context.recordController.CurrentRecord.Description.name;
        }


        #endregion  // Public methods

        public void Destroy()
        {
            //Program.context.playerSession.Stop();
            Application.Exit();
        }

        public void SetPlayPosition(uint position)
        {
            m_playPosition = position;
        }

        public void Stop()
        {
            /*DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_STOP);
            outCommand.AddData((int)0);

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        public void Undo()
        {
            /*DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_UNDO);
            outCommand.AddData((int)0);

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        public void  Redo()
        {
            /*DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_REDO);
            outCommand.AddData((int)0);

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        public void SetBeginSelection(uint position)
        {
            if (position < 0 || position > m_duration)
                return;
            if (position > m_endSelectPosition)
                return;
            m_beginSelectPosition = position;
        }

        public void SetEndSelection(uint position)
        {
            if (position < 0 || position > m_duration)
                return;
            if (position < m_beginSelectPosition)
                return;
            m_endSelectPosition = position;
        }

        public void DeleteSelection()
        {
            /*DataWriter data = new DataWriter();
            data.AddData(m_beginSelectPosition);
            data.AddData(m_endSelectPosition);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_DELETE_SELECTION);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(m_beginSelectPosition);
            outCommand.AddData(m_endSelectPosition);

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        public void CopySelection()
        {
            if (m_endSelectPosition - m_beginSelectPosition < 0.02)
                return;
            m_isCopy = true;

            m_beginClipboardPos = m_beginSelectPosition;
            m_endClipboardPos = m_endSelectPosition;
            
        }

        public void CutSelection()
        {
            if (m_endSelectPosition - m_beginSelectPosition < 0.02)
                return;
            m_isCopy = false;

        }

        public void PasteSelection()
        {
            /*DataWriter data = new DataWriter();
            data.AddData(m_beginSelectPosition);
            data.AddData(m_endSelectPosition);
            data.AddData(m_playPosition);
            data.AddData(Convert.ToByte(m_isCopy));

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_PASTE_SELECTION);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(m_beginSelectPosition);
            outCommand.AddData(m_endSelectPosition);
            outCommand.AddData(m_playPosition);
            outCommand.AddData(Convert.ToByte(m_isCopy));

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        public void OpenRecord(string URL)
        {
            System.Diagnostics.Debug.Assert(false);
            /*SetPlayPosition(0);
            SetBeginSelection(0);
            SetEndSelection(0);

            Program.context.recordController.OpenRecord(URL);*/
       }

        public void OpenRecordByRow(int row)
        {
            System.Diagnostics.Debug.Assert(false);
            /*if (m_recordList.Count == 0)
                return;
            RecordDescription desc = m_recordList[row] as RecordDescription;
            GetRecordInfo(desc.recordID);*/
        }

        public void GetRecordInfo(int recordID)
        {
            System.Diagnostics.Debug.Assert(false);
            /*DataWriter data = new DataWriter();
            data.AddData(recordID);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_GET_RECORD_INFO);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(data.GetData(), data.GetSize());

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        private void Save( RecordDescription desc)
        {
            /*DataWriter data = new DataWriter();
            data.AddData(desc.author);
            data.AddData(desc.creationTime);
            data.AddData(desc.name);
            data.AddData(desc.description);

            DataWriter outCommand = new DataWriter();
            outCommand.AddData(Command.CMD_SAVE);
            outCommand.AddData((int)data.GetSize());
            outCommand.AddData(data.GetData(), data.GetSize());

            Program.context.pipeConnector.addOutCommand(outCommand);*/
        }

        public void GetRecordList()
        {
            /*DataWriter command = new DataWriter();
            command.AddData(Command.CMD_GET_RECORD_LIST);
            command.AddData((int)0);
            Program.context.pipeConnector.addOutCommand(command);*/
        }

        public void AddRecordsToList(ArrayList records)
        {
            /*m_recordList.AddRange(records);
            int recordsCount = records.Count;
            m_openRecordListDialog.DoTableAddRecordsUI(m_recordList.GetRange(m_recordList.Count - recordsCount, recordsCount));*/
        }

        private void mainToolbar_Load(object sender, EventArgs e)
        {

        }

        private void menuItemOpen_Click(object sender, EventArgs e)
        {
            try
            {
                //GetRecordList();
                Program.context.recordController.UpdateRecordsList();
                m_openRecordListDialog.ShowDialog();
            }
            catch (System.Exception)
            {
                MessageBox.Show("Ошибка отображения списка записей");
            }
        }
    }
}