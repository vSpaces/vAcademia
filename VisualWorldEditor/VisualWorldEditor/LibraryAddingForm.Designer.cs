namespace VisualWorldEditor
{
    partial class LibraryAddingForm
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
            this.btnAdd = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.categoryGroup = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.categoryCombo = new System.Windows.Forms.ComboBox();
            this.categoryTBox = new System.Windows.Forms.TextBox();
            this.subcategoryGroup = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.subcategoryCombo = new System.Windows.Forms.ComboBox();
            this.subcategoryTBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.objectNameTBox = new System.Windows.Forms.TextBox();
            this.categoryGroup.SuspendLayout();
            this.subcategoryGroup.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnAdd
            // 
            this.btnAdd.Location = new System.Drawing.Point(84, 218);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 7;
            this.btnAdd.Text = "Добавить";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(165, 218);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 8;
            this.btnCancel.Text = "Отмена";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // categoryGroup
            // 
            this.categoryGroup.Controls.Add(this.label2);
            this.categoryGroup.Controls.Add(this.categoryCombo);
            this.categoryGroup.Controls.Add(this.categoryTBox);
            this.categoryGroup.Location = new System.Drawing.Point(12, 12);
            this.categoryGroup.Name = "categoryGroup";
            this.categoryGroup.Size = new System.Drawing.Size(234, 75);
            this.categoryGroup.TabIndex = 0;
            this.categoryGroup.TabStop = false;
            this.categoryGroup.Text = "Выбериет категорию:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(42, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Новая:";
            // 
            // categoryCombo
            // 
            this.categoryCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.categoryCombo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.categoryCombo.FormattingEnabled = true;
            this.categoryCombo.Location = new System.Drawing.Point(6, 19);
            this.categoryCombo.Name = "categoryCombo";
            this.categoryCombo.Size = new System.Drawing.Size(221, 21);
            this.categoryCombo.TabIndex = 1;
            this.categoryCombo.SelectedIndexChanged += new System.EventHandler(this.categoryCombo_SelectedIndexChanged);
            // 
            // categoryTBox
            // 
            this.categoryTBox.Location = new System.Drawing.Point(55, 46);
            this.categoryTBox.Name = "categoryTBox";
            this.categoryTBox.Size = new System.Drawing.Size(172, 20);
            this.categoryTBox.TabIndex = 2;
            // 
            // subcategoryGroup
            // 
            this.subcategoryGroup.Controls.Add(this.label3);
            this.subcategoryGroup.Controls.Add(this.subcategoryCombo);
            this.subcategoryGroup.Controls.Add(this.subcategoryTBox);
            this.subcategoryGroup.Location = new System.Drawing.Point(12, 93);
            this.subcategoryGroup.Name = "subcategoryGroup";
            this.subcategoryGroup.Size = new System.Drawing.Size(234, 75);
            this.subcategoryGroup.TabIndex = 3;
            this.subcategoryGroup.TabStop = false;
            this.subcategoryGroup.Text = "Выбериет подкатегорию:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 49);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(42, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Новая:";
            // 
            // subcategoryCombo
            // 
            this.subcategoryCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.subcategoryCombo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.subcategoryCombo.FormattingEnabled = true;
            this.subcategoryCombo.Location = new System.Drawing.Point(6, 19);
            this.subcategoryCombo.Name = "subcategoryCombo";
            this.subcategoryCombo.Size = new System.Drawing.Size(221, 21);
            this.subcategoryCombo.TabIndex = 4;
            // 
            // subcategoryTBox
            // 
            this.subcategoryTBox.Location = new System.Drawing.Point(55, 46);
            this.subcategoryTBox.Name = "subcategoryTBox";
            this.subcategoryTBox.Size = new System.Drawing.Size(173, 20);
            this.subcategoryTBox.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(16, 176);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(105, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Название объекта:";
            // 
            // objectNameTBox
            // 
            this.objectNameTBox.Location = new System.Drawing.Point(18, 192);
            this.objectNameTBox.Name = "objectNameTBox";
            this.objectNameTBox.Size = new System.Drawing.Size(222, 20);
            this.objectNameTBox.TabIndex = 6;
            // 
            // LibraryAddingForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(258, 251);
            this.Controls.Add(this.objectNameTBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.subcategoryGroup);
            this.Controls.Add(this.categoryGroup);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnAdd);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "LibraryAddingForm";
            this.Text = "Добавление объекта в библиотеку";
            this.categoryGroup.ResumeLayout(false);
            this.categoryGroup.PerformLayout();
            this.subcategoryGroup.ResumeLayout(false);
            this.subcategoryGroup.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.GroupBox categoryGroup;
        private System.Windows.Forms.ComboBox categoryCombo;
        private System.Windows.Forms.TextBox categoryTBox;
        private System.Windows.Forms.GroupBox subcategoryGroup;
        private System.Windows.Forms.ComboBox subcategoryCombo;
        private System.Windows.Forms.TextBox subcategoryTBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox objectNameTBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
    }
}