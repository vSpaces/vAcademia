using System.Data;
using System.Collections;
using System.Windows.Forms;

namespace RecordEditor
{
    partial class OpenRecordList
    {

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.gridRecordList = new System.Windows.Forms.DataGridView();
            this.buttonOpenRecord = new System.Windows.Forms.Button();
            this.TitlePanel.SuspendLayout();
            this.ContentPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridRecordList)).BeginInit();
            this.SuspendLayout();
            // 
            // btnClose
            // 
            this.btnClose.Location = new System.Drawing.Point(635, 7);
            // 
            // btnMinimize
            // 
            this.btnMinimize.Location = new System.Drawing.Point(585, 7);
            // 
            // btnMaximize
            // 
            this.btnMaximize.Location = new System.Drawing.Point(610, 7);
            // 
            // btnNormal
            // 
            this.btnNormal.Location = new System.Drawing.Point(610, 7);
            // 
            // TitlePanel
            // 
            this.TitlePanel.Size = new System.Drawing.Size(672, 27);
            // 
            // StatusPanel
            // 
            this.StatusPanel.Location = new System.Drawing.Point(0, 335);
            this.StatusPanel.Size = new System.Drawing.Size(672, 30);
            // 
            // ContentPanel
            // 
            this.ContentPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(155)))), ((int)(((byte)(248)))));
            this.ContentPanel.Controls.Add(this.gridRecordList);
            this.ContentPanel.Size = new System.Drawing.Size(672, 308);
            // 
            // gridRecordList
            // 
            this.gridRecordList.AllowUserToOrderColumns = true;
            this.gridRecordList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gridRecordList.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.gridRecordList.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            this.gridRecordList.BackgroundColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.gridRecordList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.gridRecordList.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(155)))), ((int)(((byte)(204)))));
            this.gridRecordList.Location = new System.Drawing.Point(3, 6);
            this.gridRecordList.Name = "gridRecordList";
            this.gridRecordList.ReadOnly = true;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(155)))), ((int)(((byte)(204)))));
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.gridRecordList.RowHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.gridRecordList.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.gridRecordList.Size = new System.Drawing.Size(663, 296);
            this.gridRecordList.TabIndex = 1;
            this.gridRecordList.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.gridRecordList_CellClick);
            // 
            // buttonOpenRecord
            // 
            this.buttonOpenRecord.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOpenRecord.Location = new System.Drawing.Point(573, 338);
            this.buttonOpenRecord.Name = "buttonOpenRecord";
            this.buttonOpenRecord.Size = new System.Drawing.Size(75, 23);
            this.buttonOpenRecord.TabIndex = 3;
            this.buttonOpenRecord.Text = "Открыть";
            this.buttonOpenRecord.UseVisualStyleBackColor = true;
            this.buttonOpenRecord.Click += new System.EventHandler(this.buttonOpenRecord_Click);
            // 
            // OpenRecordList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(672, 364);
            this.Controls.Add(this.buttonOpenRecord);
            this.Location = new System.Drawing.Point(0, 0);
            this.Name = "OpenRecordList";
            this.statusBarHeight = 30;
            this.Text = "Открытие записи";
            this.Title = "Открытие записи";
            this.titleLocation = new System.Drawing.Point(15, 5);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.OpenRecordList_FormClosing);
            this.Controls.SetChildIndex(this.TitlePanel, 0);
            this.Controls.SetChildIndex(this.StatusPanel, 0);
            this.Controls.SetChildIndex(this.ContentPanel, 0);
            this.Controls.SetChildIndex(this.buttonOpenRecord, 0);
            this.TitlePanel.ResumeLayout(false);
            this.ContentPanel.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridRecordList)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private DataGridView gridRecordList;
        private Button buttonOpenRecord;
    }
}