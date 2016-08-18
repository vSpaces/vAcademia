namespace VisualWorldEditor
{
    partial class LodInfo
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
            this.label1 = new System.Windows.Forms.Label();
            this.distanceTBox = new System.Windows.Forms.TextBox();
            this.btnCreateLod = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.shadowCheckBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(66, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Дистанция:";
            // 
            // distanceTBox
            // 
            this.distanceTBox.Location = new System.Drawing.Point(82, 6);
            this.distanceTBox.Name = "distanceTBox";
            this.distanceTBox.Size = new System.Drawing.Size(116, 20);
            this.distanceTBox.TabIndex = 1;
            // 
            // btnCreateLod
            // 
            this.btnCreateLod.Location = new System.Drawing.Point(73, 36);
            this.btnCreateLod.Name = "btnCreateLod";
            this.btnCreateLod.Size = new System.Drawing.Size(125, 23);
            this.btnCreateLod.TabIndex = 2;
            this.btnCreateLod.Text = "Создать";
            this.btnCreateLod.UseVisualStyleBackColor = true;
            this.btnCreateLod.Click += new System.EventHandler(this.btnCreateLod_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(204, 36);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(54, 23);
            this.btnCancel.TabIndex = 3;
            this.btnCancel.Text = "Отмена";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // shadowCheckBox
            // 
            this.shadowCheckBox.AutoSize = true;
            this.shadowCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.shadowCheckBox.Location = new System.Drawing.Point(204, 8);
            this.shadowCheckBox.Name = "shadowCheckBox";
            this.shadowCheckBox.Size = new System.Drawing.Size(54, 17);
            this.shadowCheckBox.TabIndex = 4;
            this.shadowCheckBox.Text = "Тень:";
            this.shadowCheckBox.UseVisualStyleBackColor = true;
            // 
            // LodInfo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(275, 65);
            this.ControlBox = false;
            this.Controls.Add(this.shadowCheckBox);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnCreateLod);
            this.Controls.Add(this.distanceTBox);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "LodInfo";
            this.Text = "Информация о лоде";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox distanceTBox;
        private System.Windows.Forms.Button btnCreateLod;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.CheckBox shadowCheckBox;
    }
}