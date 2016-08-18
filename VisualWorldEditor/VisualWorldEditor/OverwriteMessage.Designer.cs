namespace VisualWorldEditor
{
    partial class OverwriteMessage
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
            this.yesBtn = new System.Windows.Forms.Button();
            this.yesAllBtn = new System.Windows.Forms.Button();
            this.noBtn = new System.Windows.Forms.Button();
            this.noAllBtn = new System.Windows.Forms.Button();
            this.cancelBtn = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // yesBtn
            // 
            this.yesBtn.Location = new System.Drawing.Point(12, 25);
            this.yesBtn.Name = "yesBtn";
            this.yesBtn.Size = new System.Drawing.Size(75, 23);
            this.yesBtn.TabIndex = 0;
            this.yesBtn.Text = "Да";
            this.yesBtn.UseVisualStyleBackColor = true;
            this.yesBtn.Click += new System.EventHandler(this.yesBtn_Click);
            // 
            // yesAllBtn
            // 
            this.yesAllBtn.Location = new System.Drawing.Point(93, 25);
            this.yesAllBtn.Name = "yesAllBtn";
            this.yesAllBtn.Size = new System.Drawing.Size(82, 23);
            this.yesAllBtn.TabIndex = 1;
            this.yesAllBtn.Text = "Да для всех";
            this.yesAllBtn.UseVisualStyleBackColor = true;
            this.yesAllBtn.Click += new System.EventHandler(this.yesAllBtn_Click);
            // 
            // noBtn
            // 
            this.noBtn.Location = new System.Drawing.Point(181, 25);
            this.noBtn.Name = "noBtn";
            this.noBtn.Size = new System.Drawing.Size(75, 23);
            this.noBtn.TabIndex = 2;
            this.noBtn.Text = "Нет";
            this.noBtn.UseVisualStyleBackColor = true;
            this.noBtn.Click += new System.EventHandler(this.noBtn_Click);
            // 
            // noAllBtn
            // 
            this.noAllBtn.Location = new System.Drawing.Point(262, 25);
            this.noAllBtn.Name = "noAllBtn";
            this.noAllBtn.Size = new System.Drawing.Size(82, 23);
            this.noAllBtn.TabIndex = 3;
            this.noAllBtn.Text = "Нет для всех";
            this.noAllBtn.UseVisualStyleBackColor = true;
            this.noAllBtn.Click += new System.EventHandler(this.noAllBtn_Click);
            // 
            // cancelBtn
            // 
            this.cancelBtn.Location = new System.Drawing.Point(350, 25);
            this.cancelBtn.Name = "cancelBtn";
            this.cancelBtn.Size = new System.Drawing.Size(75, 23);
            this.cancelBtn.TabIndex = 4;
            this.cancelBtn.Text = "Отмена";
            this.cancelBtn.UseVisualStyleBackColor = true;
            this.cancelBtn.Click += new System.EventHandler(this.cancelBtn_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(88, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(256, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Копируемый объект уже существует. Заменить?";
            // 
            // OverwriteMessage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(431, 57);
            this.ControlBox = false;
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cancelBtn);
            this.Controls.Add(this.noAllBtn);
            this.Controls.Add(this.noBtn);
            this.Controls.Add(this.yesAllBtn);
            this.Controls.Add(this.yesBtn);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "OverwriteMessage";
            this.Text = "Копирование";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button yesBtn;
        private System.Windows.Forms.Button yesAllBtn;
        private System.Windows.Forms.Button noBtn;
        private System.Windows.Forms.Button noAllBtn;
        private System.Windows.Forms.Button cancelBtn;
        private System.Windows.Forms.Label label1;
    }
}