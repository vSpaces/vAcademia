namespace VisualWorldEditor
{
    partial class MaterialForm
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
            this.label2 = new System.Windows.Forms.Label();
            this.templatesComboBox = new System.Windows.Forms.ComboBox();
            this.materialGroupBox = new System.Windows.Forms.GroupBox();
            this.materialParamsPanel = new System.Windows.Forms.Panel();
            this.noMaterialInfo = new System.Windows.Forms.Label();
            this.btnTemplateCreate = new System.Windows.Forms.Button();
            this.btnAddMaterialParam = new System.Windows.Forms.Button();
            this.addinsComboBox = new System.Windows.Forms.ComboBox();
            this.currentLod = new System.Windows.Forms.Label();
            this.incLodButton = new System.Windows.Forms.Button();
            this.decLodButton = new System.Windows.Forms.Button();
            this.texturesCheckCombo = new VisualWorldEditor.EditorCheckCombo();
            this.selectMaterialByClick = new System.Windows.Forms.CheckBox();
            this.materialGroupBox.SuspendLayout();
            this.materialParamsPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(188, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Список используемых материалов:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 68);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(124, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Использовать шаблон:";
            // 
            // templatesComboBox
            // 
            this.templatesComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.templatesComboBox.Enabled = false;
            this.templatesComboBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.templatesComboBox.FormattingEnabled = true;
            this.templatesComboBox.Location = new System.Drawing.Point(16, 84);
            this.templatesComboBox.Name = "templatesComboBox";
            this.templatesComboBox.Size = new System.Drawing.Size(184, 21);
            this.templatesComboBox.TabIndex = 5;
            // 
            // materialGroupBox
            // 
            this.materialGroupBox.Controls.Add(this.materialParamsPanel);
            this.materialGroupBox.Location = new System.Drawing.Point(16, 111);
            this.materialGroupBox.Name = "materialGroupBox";
            this.materialGroupBox.Size = new System.Drawing.Size(269, 259);
            this.materialGroupBox.TabIndex = 6;
            this.materialGroupBox.TabStop = false;
            this.materialGroupBox.Text = "Параметры материала";
            // 
            // materialParamsPanel
            // 
            this.materialParamsPanel.AutoScroll = true;
            this.materialParamsPanel.Controls.Add(this.noMaterialInfo);
            this.materialParamsPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.materialParamsPanel.Location = new System.Drawing.Point(3, 16);
            this.materialParamsPanel.Name = "materialParamsPanel";
            this.materialParamsPanel.Size = new System.Drawing.Size(263, 240);
            this.materialParamsPanel.TabIndex = 0;
            this.materialParamsPanel.ControlAdded += new System.Windows.Forms.ControlEventHandler(this.materialParamsPanel_ControlAdded);
            this.materialParamsPanel.ControlRemoved += new System.Windows.Forms.ControlEventHandler(this.materialParamsPanel_ControlRemoved);
            // 
            // noMaterialInfo
            // 
            this.noMaterialInfo.Location = new System.Drawing.Point(1, 0);
            this.noMaterialInfo.Name = "noMaterialInfo";
            this.noMaterialInfo.Size = new System.Drawing.Size(259, 231);
            this.noMaterialInfo.TabIndex = 0;
            this.noMaterialInfo.Text = " ";
            this.noMaterialInfo.Visible = false;
            // 
            // btnTemplateCreate
            // 
            this.btnTemplateCreate.Enabled = false;
            this.btnTemplateCreate.Location = new System.Drawing.Point(207, 82);
            this.btnTemplateCreate.Name = "btnTemplateCreate";
            this.btnTemplateCreate.Size = new System.Drawing.Size(75, 23);
            this.btnTemplateCreate.TabIndex = 8;
            this.btnTemplateCreate.Text = "Загрузить..";
            this.btnTemplateCreate.UseVisualStyleBackColor = true;
            this.btnTemplateCreate.Click += new System.EventHandler(this.btnTemplateCreate_Click);
            // 
            // btnAddMaterialParam
            // 
            this.btnAddMaterialParam.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F);
            this.btnAddMaterialParam.Location = new System.Drawing.Point(177, 402);
            this.btnAddMaterialParam.Name = "btnAddMaterialParam";
            this.btnAddMaterialParam.Size = new System.Drawing.Size(108, 23);
            this.btnAddMaterialParam.TabIndex = 9;
            this.btnAddMaterialParam.Text = "Добавить свойство";
            this.btnAddMaterialParam.UseVisualStyleBackColor = true;
            this.btnAddMaterialParam.Click += new System.EventHandler(this.btnAddMaterialParam_Click);
            // 
            // addinsComboBox
            // 
            this.addinsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.addinsComboBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.addinsComboBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F);
            this.addinsComboBox.FormattingEnabled = true;
            this.addinsComboBox.Location = new System.Drawing.Point(19, 404);
            this.addinsComboBox.Name = "addinsComboBox";
            this.addinsComboBox.Size = new System.Drawing.Size(152, 20);
            this.addinsComboBox.TabIndex = 10;
            // 
            // currentLod
            // 
            this.currentLod.AutoSize = true;
            this.currentLod.Location = new System.Drawing.Point(16, 379);
            this.currentLod.Name = "currentLod";
            this.currentLod.Size = new System.Drawing.Size(33, 13);
            this.currentLod.TabIndex = 11;
            this.currentLod.Text = "Лод: ";
            // 
            // incLodButton
            // 
            this.incLodButton.Location = new System.Drawing.Point(58, 373);
            this.incLodButton.Name = "incLodButton";
            this.incLodButton.Size = new System.Drawing.Size(25, 25);
            this.incLodButton.TabIndex = 12;
            this.incLodButton.Text = "+";
            this.incLodButton.UseVisualStyleBackColor = true;
            this.incLodButton.Click += new System.EventHandler(this.incLodButton_Click);
            // 
            // decLodButton
            // 
            this.decLodButton.Location = new System.Drawing.Point(89, 373);
            this.decLodButton.Name = "decLodButton";
            this.decLodButton.Size = new System.Drawing.Size(25, 25);
            this.decLodButton.TabIndex = 13;
            this.decLodButton.Text = "-";
            this.decLodButton.UseVisualStyleBackColor = true;
            this.decLodButton.Click += new System.EventHandler(this.decLodButton_Click);
            // 
            // texturesCheckCombo
            // 
            this.texturesCheckCombo.Enabled = false;
            this.texturesCheckCombo.Location = new System.Drawing.Point(16, 42);
            this.texturesCheckCombo.Name = "texturesCheckCombo";
            this.texturesCheckCombo.ReadOnly = true;
            this.texturesCheckCombo.Size = new System.Drawing.Size(269, 23);
            this.texturesCheckCombo.TabIndex = 7;
            this.texturesCheckCombo.TreeImageList = null;
            this.texturesCheckCombo.DropDownHided += new System.EventHandler(this.texturesCheckCombo_DropDownHided);
            this.texturesCheckCombo.ValueChanged += new System.EventHandler(this.texturesCheckCombo_ValueChanged);
            this.texturesCheckCombo.Click += new System.EventHandler(this.texturesCheckCombo_Click);
            this.texturesCheckCombo.CloseDropDown += new UtilityLibrary.Combos.CustomCombo.CloseDropDownHandler(this.texturesCheckCombo_CloseDropDown);
            // 
            // selectMaterialByClick
            // 
            this.selectMaterialByClick.AutoSize = true;
            this.selectMaterialByClick.Location = new System.Drawing.Point(18, 6);
            this.selectMaterialByClick.Name = "selectMaterialByClick";
            this.selectMaterialByClick.Size = new System.Drawing.Size(182, 17);
            this.selectMaterialByClick.TabIndex = 14;
            this.selectMaterialByClick.Text = "выделять материалы по клику";
            this.selectMaterialByClick.UseVisualStyleBackColor = true;
            // 
            // MaterialForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 431);
            this.Controls.Add(this.selectMaterialByClick);
            this.Controls.Add(this.decLodButton);
            this.Controls.Add(this.incLodButton);
            this.Controls.Add(this.currentLod);
            this.Controls.Add(this.addinsComboBox);
            this.Controls.Add(this.btnAddMaterialParam);
            this.Controls.Add(this.btnTemplateCreate);
            this.Controls.Add(this.texturesCheckCombo);
            this.Controls.Add(this.materialGroupBox);
            this.Controls.Add(this.templatesComboBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MaterialForm";
            this.Text = "Материалы";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MaterialForm_FormClosing);
            this.materialGroupBox.ResumeLayout(false);
            this.materialParamsPanel.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox templatesComboBox;
        private System.Windows.Forms.GroupBox materialGroupBox;
        //private UtilityLibrary.Combos.EditorCheckCombo texturesCheckCombo;
        private EditorCheckCombo texturesCheckCombo;
        private System.Windows.Forms.Button btnTemplateCreate;
        private System.Windows.Forms.Panel materialParamsPanel;
        private System.Windows.Forms.Label noMaterialInfo;
        private System.Windows.Forms.Button btnAddMaterialParam;
        private System.Windows.Forms.ComboBox addinsComboBox;
        private System.Windows.Forms.Label currentLod;
        private System.Windows.Forms.Button incLodButton;
        private System.Windows.Forms.Button decLodButton;
        private System.Windows.Forms.CheckBox selectMaterialByClick;
    }
}