namespace VisualWorldEditor
{
    partial class Options
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
        this.LabelClienPath = new System.Windows.Forms.Label();
        this.ClientBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
        this.ButtonClientPath = new System.Windows.Forms.Button();
        this.textBoxClientPath = new System.Windows.Forms.TextBox();
        this.ButtonSave = new System.Windows.Forms.Button();
        this.ButtonCancel = new System.Windows.Forms.Button();
        this.groupBox1 = new System.Windows.Forms.GroupBox();
        this.mysqlPasswordTBox = new System.Windows.Forms.TextBox();
        this.label3 = new System.Windows.Forms.Label();
        this.mysqlLoginTBox = new System.Windows.Forms.TextBox();
        this.label2 = new System.Windows.Forms.Label();
        this.mysqlHostTBox = new System.Windows.Forms.TextBox();
        this.label1 = new System.Windows.Forms.Label();
        this.groupBox2 = new System.Windows.Forms.GroupBox();
        this.btnServerPath = new System.Windows.Forms.Button();
        this.linkToEditor = new System.Windows.Forms.CheckBox();
        this.groupBox1.SuspendLayout();
        this.groupBox2.SuspendLayout();
        this.SuspendLayout();
        // 
        // LabelClienPath
        // 
        this.LabelClienPath.AutoSize = true;
        this.LabelClienPath.Location = new System.Drawing.Point(7, 16);
        this.LabelClienPath.Name = "LabelClienPath";
        this.LabelClienPath.Size = new System.Drawing.Size(34, 13);
        this.LabelClienPath.TabIndex = 0;
        this.LabelClienPath.Text = "Путь:";
        // 
        // ClientBrowserDialog
        // 
        this.ClientBrowserDialog.Description = "Выбери папку куда установлен клиент";
        this.ClientBrowserDialog.RootFolder = System.Environment.SpecialFolder.MyComputer;
        this.ClientBrowserDialog.SelectedPath = "C:\\Program Files\\Virtual University";
        this.ClientBrowserDialog.ShowNewFolderButton = false;
        // 
        // ButtonClientPath
        // 
        this.ButtonClientPath.Location = new System.Drawing.Point(270, 11);
        this.ButtonClientPath.Name = "ButtonClientPath";
        this.ButtonClientPath.Size = new System.Drawing.Size(26, 23);
        this.ButtonClientPath.TabIndex = 1;
        this.ButtonClientPath.Text = "...";
        this.ButtonClientPath.UseVisualStyleBackColor = true;
        this.ButtonClientPath.Click += new System.EventHandler(this.ButtonClientPath_Click);
        // 
        // textBoxClientPath
        // 
        this.textBoxClientPath.Location = new System.Drawing.Point(53, 13);
        this.textBoxClientPath.Name = "textBoxClientPath";
        this.textBoxClientPath.Size = new System.Drawing.Size(211, 20);
        this.textBoxClientPath.TabIndex = 2;
        // 
        // ButtonSave
        // 
        this.ButtonSave.Location = new System.Drawing.Point(158, 229);
        this.ButtonSave.Name = "ButtonSave";
        this.ButtonSave.Size = new System.Drawing.Size(75, 23);
        this.ButtonSave.TabIndex = 3;
        this.ButtonSave.Text = "Сохранить";
        this.ButtonSave.UseVisualStyleBackColor = true;
        this.ButtonSave.Click += new System.EventHandler(this.ButtonSave_Click);
        // 
        // ButtonCancel
        // 
        this.ButtonCancel.Location = new System.Drawing.Point(239, 229);
        this.ButtonCancel.Name = "ButtonCancel";
        this.ButtonCancel.Size = new System.Drawing.Size(75, 23);
        this.ButtonCancel.TabIndex = 4;
        this.ButtonCancel.Text = "Отмена";
        this.ButtonCancel.UseVisualStyleBackColor = true;
        this.ButtonCancel.Click += new System.EventHandler(this.ButtonCancel_Click);
        // 
        // groupBox1
        // 
        this.groupBox1.Controls.Add(this.mysqlPasswordTBox);
        this.groupBox1.Controls.Add(this.label3);
        this.groupBox1.Controls.Add(this.mysqlLoginTBox);
        this.groupBox1.Controls.Add(this.label2);
        this.groupBox1.Controls.Add(this.mysqlHostTBox);
        this.groupBox1.Controls.Add(this.label1);
        this.groupBox1.Enabled = false;
        this.groupBox1.Location = new System.Drawing.Point(12, 109);
        this.groupBox1.Name = "groupBox1";
        this.groupBox1.Size = new System.Drawing.Size(302, 103);
        this.groupBox1.TabIndex = 5;
        this.groupBox1.TabStop = false;
        this.groupBox1.Text = "Параметры подключения MySQL";
        // 
        // mysqlPasswordTBox
        // 
        this.mysqlPasswordTBox.Location = new System.Drawing.Point(53, 75);
        this.mysqlPasswordTBox.Name = "mysqlPasswordTBox";
        this.mysqlPasswordTBox.PasswordChar = '*';
        this.mysqlPasswordTBox.Size = new System.Drawing.Size(132, 20);
        this.mysqlPasswordTBox.TabIndex = 5;
        // 
        // label3
        // 
        this.label3.AutoSize = true;
        this.label3.Location = new System.Drawing.Point(7, 78);
        this.label3.Name = "label3";
        this.label3.Size = new System.Drawing.Size(48, 13);
        this.label3.TabIndex = 4;
        this.label3.Text = "Пароль:";
        // 
        // mysqlLoginTBox
        // 
        this.mysqlLoginTBox.Location = new System.Drawing.Point(53, 48);
        this.mysqlLoginTBox.Name = "mysqlLoginTBox";
        this.mysqlLoginTBox.Size = new System.Drawing.Size(132, 20);
        this.mysqlLoginTBox.TabIndex = 3;
        // 
        // label2
        // 
        this.label2.AutoSize = true;
        this.label2.Location = new System.Drawing.Point(6, 51);
        this.label2.Name = "label2";
        this.label2.Size = new System.Drawing.Size(41, 13);
        this.label2.TabIndex = 2;
        this.label2.Text = "Логин:";
        // 
        // mysqlHostTBox
        // 
        this.mysqlHostTBox.Location = new System.Drawing.Point(53, 19);
        this.mysqlHostTBox.Name = "mysqlHostTBox";
        this.mysqlHostTBox.Size = new System.Drawing.Size(243, 20);
        this.mysqlHostTBox.TabIndex = 1;
        // 
        // label1
        // 
        this.label1.AutoSize = true;
        this.label1.Location = new System.Drawing.Point(7, 22);
        this.label1.Name = "label1";
        this.label1.Size = new System.Drawing.Size(34, 13);
        this.label1.TabIndex = 0;
        this.label1.Text = "Хост:";
        // 
        // groupBox2
        // 
        this.groupBox2.Controls.Add(this.LabelClienPath);
        this.groupBox2.Controls.Add(this.textBoxClientPath);
        this.groupBox2.Controls.Add(this.ButtonClientPath);
        this.groupBox2.Location = new System.Drawing.Point(12, 6);
        this.groupBox2.Name = "groupBox2";
        this.groupBox2.Size = new System.Drawing.Size(302, 39);
        this.groupBox2.TabIndex = 6;
        this.groupBox2.TabStop = false;
        this.groupBox2.Text = "Клиент";
        // 
        // btnServerPath
        // 
        this.btnServerPath.Location = new System.Drawing.Point(12, 228);
        this.btnServerPath.Name = "btnServerPath";
        this.btnServerPath.Size = new System.Drawing.Size(106, 23);
        this.btnServerPath.TabIndex = 7;
        this.btnServerPath.Text = "Путь до сервера";
        this.btnServerPath.UseVisualStyleBackColor = true;
        this.btnServerPath.Click += new System.EventHandler(this.btnServerPath_Click);
        // 
        // linkToEditor
        // 
        this.linkToEditor.AutoSize = true;
        this.linkToEditor.Location = new System.Drawing.Point(12, 52);
        this.linkToEditor.Name = "linkToEditor";
        this.linkToEditor.Size = new System.Drawing.Size(286, 17);
        this.linkToEditor.TabIndex = 8;
        this.linkToEditor.Text = "Привязать окно клиента к левому краю редактора";
        this.linkToEditor.UseVisualStyleBackColor = true;
        this.linkToEditor.CheckedChanged += new System.EventHandler(this.linkToEditor_CheckedChanged);
        // 
        // Options
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(326, 264);
        this.Controls.Add(this.linkToEditor);
        this.Controls.Add(this.btnServerPath);
        this.Controls.Add(this.groupBox2);
        this.Controls.Add(this.groupBox1);
        this.Controls.Add(this.ButtonCancel);
        this.Controls.Add(this.ButtonSave);
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "Options";
        this.ShowInTaskbar = false;
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
        this.Text = "Настройки";
        this.Load += new System.EventHandler(this.Options_Load);
        this.groupBox1.ResumeLayout(false);
        this.groupBox1.PerformLayout();
        this.groupBox2.ResumeLayout(false);
        this.groupBox2.PerformLayout();
        this.ResumeLayout(false);
        this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label LabelClienPath;
        private System.Windows.Forms.FolderBrowserDialog ClientBrowserDialog;
        private System.Windows.Forms.Button ButtonClientPath;
        private System.Windows.Forms.TextBox textBoxClientPath;
        private System.Windows.Forms.Button ButtonSave;
        private System.Windows.Forms.Button ButtonCancel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox mysqlHostTBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox mysqlPasswordTBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox mysqlLoginTBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnServerPath;
        private System.Windows.Forms.CheckBox linkToEditor;
    }
}