using System;
using System.Data;
using System.Linq;
using System.Text;
using System.Drawing;
using RecordEditor.Logic;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;

namespace RecordEditor
{
    public partial class OpenRecordList : FlashGenie.BaseDialog
    {
        // данные таблицы
        private DataTable m_dataTable = new DataTable();

        private RecordEditor m_recordEditor = null;

        private int m_selectedRow = 0;

        public OpenRecordList(RecordEditor recordEditor)
        {
            m_recordEditor = recordEditor;
            InitializeComponent();
            ReinitDataTable();
            this.gridRecordList.DataSource = m_dataTable;
        }

        public override string GetSkinFileName()
        {
            return SkinConsts.OpenRecordFormSkinFileName;
        }

        public void SubscribeEvents()
        {
            Program.context.recordController.OnRecordListChanged += new global::RecordEditor.Logic.RecordControllerEventHandler(recordController_OnRecordListChanged);
        }

        void recordController_OnRecordListChanged(object sender, global::RecordEditor.Logic.RecordControllerEventArgs args)
        {
            ArrayList recordsList = Program.context.recordController.RecordsList;
            DoTableAddRecordsUI(recordsList.GetRange(m_dataTable.Rows.Count, recordsList.Count - m_dataTable.Rows.Count));
        }

        public void AddRecordList(ArrayList list)
        {
            foreach (RecordDescription desc in list)
            {
                DataRow row = m_dataTable.Rows.Add(desc.recordID,desc.name, desc.author, desc.duration, desc.location, desc.creationTime);
            }
        }

        private void ReinitDataTable()
        {
            m_dataTable.Rows.Clear();
            m_dataTable.Columns.Clear();

            DataColumn column = m_dataTable.Columns.Add("Идентификатор", typeof(string));
            column.ColumnMapping = MappingType.Hidden;

            m_dataTable.Columns.Add("Название", typeof(string));
            m_dataTable.Columns.Add("Автор", typeof(string));
            m_dataTable.Columns.Add("Длительность", typeof(string));
            m_dataTable.Columns.Add("Локация", typeof(string));
            m_dataTable.Columns.Add("Дата", typeof(string));
        }

        public delegate void DoTableAddRecords(ArrayList list);

        public void DoTableAddRecordsUI(ArrayList list)
        {
            if (this.InvokeRequired)
            {
                DoTableAddRecords theTableAddRecordsDelegate = new DoTableAddRecords(DoTableAddRecordsUI);
                this.Invoke(theTableAddRecordsDelegate, new object[] { list });
            }
            else
            {
                this.AddRecordList(list);
                this.Update();
                this.Refresh();
                this.Invalidate();
            }
        }

        public delegate void DoReinitTable();

        public void DoReinitUITable()
        {
            if (this.InvokeRequired)
            {
                DoReinitTable theReinitTableDelegate = new DoReinitTable(DoReinitUITable);
                this.Invoke(theReinitTableDelegate);
            }
            else
            {
                this.ReinitDataTable();
                this.Update();
                this.Refresh();
                this.Invalidate();
            }
        }

        private void OpenRecordList_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void buttonOpenRecord_Click(object sender, EventArgs e)
        {
            if (m_dataTable.Columns.Count == 0)
                return;
            if (m_selectedRow >= m_dataTable.Rows.Count)
                return;

            DataRow selectedRow = m_dataTable.Rows[m_selectedRow];
            if (selectedRow != null)
            {
                int recordID = Int32.Parse(selectedRow[0] as string);
                string recordName = selectedRow[1] as string;
                if (Program.context.recordController.OpenRecord(recordID) == RD_ERROR.RDE_NOERROR)
                {
                    this.Hide();
                }
                else
                {
                    MessageBox.Show("Запись не может быть открыта");
                }
            }
        }

        private void gridRecordList_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            m_selectedRow = e.RowIndex;
        }
    }
}
