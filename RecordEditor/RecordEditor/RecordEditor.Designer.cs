using System.Drawing;

namespace RecordEditor
{
    partial class RecordEditor
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
            UI.TimeInterval timeInterval1 = new UI.TimeInterval();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RecordEditor));
            this.mainToolbar = new UI.MainToolbar();
            this.timeLine = new UI.TimeLine();
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuItemOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.остановитьToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.изменитьToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.видToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.справкаToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.TitlePanel.SuspendLayout();
            this.ContentPanel.SuspendLayout();
            this.menuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnClose
            // 
            this.btnClose.Location = new System.Drawing.Point(711, 7);
            // 
            // btnMinimize
            // 
            this.btnMinimize.Location = new System.Drawing.Point(661, 7);
            // 
            // btnMaximize
            // 
            this.btnMaximize.Location = new System.Drawing.Point(686, 7);
            // 
            // btnNormal
            // 
            this.btnNormal.Location = new System.Drawing.Point(686, 7);
            // 
            // TitlePanel
            // 
            this.TitlePanel.Size = new System.Drawing.Size(748, 27);
            // 
            // StatusPanel
            // 
            this.StatusPanel.Location = new System.Drawing.Point(0, 294);
            this.StatusPanel.Size = new System.Drawing.Size(748, 27);
            // 
            // ContentPanel
            // 
            this.ContentPanel.Controls.Add(this.menuStrip);
            this.ContentPanel.Controls.Add(this.timeLine);
            this.ContentPanel.Controls.Add(this.mainToolbar);
            this.ContentPanel.Size = new System.Drawing.Size(748, 267);
            // 
            // mainToolbar
            // 
            this.mainToolbar.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.mainToolbar.Dock = System.Windows.Forms.DockStyle.Top;
            this.mainToolbar.Location = new System.Drawing.Point(0, 0);
            this.mainToolbar.Name = "mainToolbar";
            this.mainToolbar.Size = new System.Drawing.Size(748, 68);
            this.mainToolbar.SkinFile = "";
            this.mainToolbar.TabIndex = 0;
            this.mainToolbar.Title = "";
            this.mainToolbar.titleXY = new System.Drawing.Point(3, 3);
            this.mainToolbar.Load += new System.EventHandler(this.mainToolbar_Load);
            // 
            // timeLine
            // 
            this.timeLine.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.timeLine.Dock = System.Windows.Forms.DockStyle.Top;
            this.timeLine.Location = new System.Drawing.Point(0, 68);
            this.timeLine.Name = "timeLine";
            this.timeLine.Size = new System.Drawing.Size(748, 60);
            this.timeLine.SkinFile = "";
            this.timeLine.TabIndex = 1;
            this.timeLine.TimeInterval = timeInterval1;
            this.timeLine.Title = "";
            this.timeLine.titleXY = new System.Drawing.Point(3, 3);
            // 
            // menuStrip
            // 
            this.menuStrip.AutoSize = false;
            this.menuStrip.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(155)))), ((int)(((byte)(248)))));
            this.menuStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.изменитьToolStripMenuItem,
            this.видToolStripMenuItem,
            this.справкаToolStripMenuItem});
            this.menuStrip.Location = new System.Drawing.Point(4, 1);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Padding = new System.Windows.Forms.Padding(2, 0, 0, 0);
            this.menuStrip.Size = new System.Drawing.Size(321, 16);
            this.menuStrip.TabIndex = 2;
            this.menuStrip.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemOpen,
            this.остановитьToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(54, 16);
            this.fileToolStripMenuItem.Text = "Запись";
            // 
            // menuItemOpen
            // 
            this.menuItemOpen.Name = "menuItemOpen";
            this.menuItemOpen.Size = new System.Drawing.Size(146, 22);
            this.menuItemOpen.Text = "Открыть";
            this.menuItemOpen.Click += new System.EventHandler(this.menuItemOpen_Click);
            // 
            // остановитьToolStripMenuItem
            // 
            this.остановитьToolStripMenuItem.Name = "остановитьToolStripMenuItem";
            this.остановитьToolStripMenuItem.Size = new System.Drawing.Size(146, 22);
            this.остановитьToolStripMenuItem.Text = "Остановить";
            // 
            // изменитьToolStripMenuItem
            // 
            this.изменитьToolStripMenuItem.Name = "изменитьToolStripMenuItem";
            this.изменитьToolStripMenuItem.Size = new System.Drawing.Size(67, 16);
            this.изменитьToolStripMenuItem.Text = "Изменить";
            // 
            // видToolStripMenuItem
            // 
            this.видToolStripMenuItem.Name = "видToolStripMenuItem";
            this.видToolStripMenuItem.Size = new System.Drawing.Size(38, 16);
            this.видToolStripMenuItem.Text = "Вид";
            // 
            // справкаToolStripMenuItem
            // 
            this.справкаToolStripMenuItem.Name = "справкаToolStripMenuItem";
            this.справкаToolStripMenuItem.Size = new System.Drawing.Size(62, 16);
            this.справкаToolStripMenuItem.Text = "Справка";
            // 
            // RecordEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(748, 320);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel, ((byte)(204)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Location = new System.Drawing.Point(0, 0);
            this.MainMenuStrip = this.menuStrip;
            this.Name = "RecordEditor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Редактор занятий";
            this.Title = "Редактор занятий";
            this.TitlePanel.ResumeLayout(false);
            this.ContentPanel.ResumeLayout(false);
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private UI.MainToolbar mainToolbar;
        private UI.TimeLine timeLine;
        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem изменитьToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem видToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem справкаToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuItemOpen;
        private System.Windows.Forms.ToolStripMenuItem остановитьToolStripMenuItem;


    }
}

