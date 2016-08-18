namespace UI
{
    partial class MainToolbar
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.btnCut = new FlashGenie.ToggleButton();
            this.btnCopy = new FlashGenie.ToggleButton();
            this.btnPaste = new FlashGenie.ToggleButton();
            this.btnDelete = new FlashGenie.ToggleButton();
            this.btnRedo = new FlashGenie.ToggleButton();
            this.btnUndo = new FlashGenie.ToggleButton();
            this.SuspendLayout();
            // 
            // btnCut
            // 
            this.btnCut.Active = false;
            this.btnCut.ActiveImage = null;
            this.btnCut.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnCut.DisabledImage = null;
            this.btnCut.Location = new System.Drawing.Point(11, 28);
            this.btnCut.Name = "btnCut";
            this.btnCut.Pressed = false;
            this.btnCut.PressedImage = null;
            this.btnCut.Size = new System.Drawing.Size(30, 30);
            this.btnCut.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnCut.TabIndex = 1;
            this.btnCut.Click += new System.EventHandler(this.btnCut_Click);
            // 
            // btnCopy
            // 
            this.btnCopy.Active = false;
            this.btnCopy.ActiveImage = null;
            this.btnCopy.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnCopy.DisabledImage = null;
            this.btnCopy.Location = new System.Drawing.Point(47, 28);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Pressed = false;
            this.btnCopy.PressedImage = null;
            this.btnCopy.Size = new System.Drawing.Size(30, 30);
            this.btnCopy.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnCopy.TabIndex = 2;
            this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
            // 
            // btnPaste
            // 
            this.btnPaste.Active = false;
            this.btnPaste.ActiveImage = null;
            this.btnPaste.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnPaste.DisabledImage = null;
            this.btnPaste.Location = new System.Drawing.Point(83, 28);
            this.btnPaste.Name = "btnPaste";
            this.btnPaste.Pressed = false;
            this.btnPaste.PressedImage = null;
            this.btnPaste.Size = new System.Drawing.Size(30, 30);
            this.btnPaste.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnPaste.TabIndex = 3;
            this.btnPaste.Click += new System.EventHandler(this.btnPaste_Click);
            // 
            // btnDelete
            // 
            this.btnDelete.Active = false;
            this.btnDelete.ActiveImage = null;
            this.btnDelete.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnDelete.DisabledImage = null;
            this.btnDelete.Location = new System.Drawing.Point(119, 28);
            this.btnDelete.Name = "btnDelete";
            this.btnDelete.Pressed = false;
            this.btnDelete.PressedImage = null;
            this.btnDelete.Size = new System.Drawing.Size(30, 30);
            this.btnDelete.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnDelete.TabIndex = 4;
            this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
            // 
            // btnRedo
            // 
            this.btnRedo.Active = false;
            this.btnRedo.ActiveImage = null;
            this.btnRedo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnRedo.DisabledImage = null;
            this.btnRedo.Location = new System.Drawing.Point(203, 28);
            this.btnRedo.Name = "btnRedo";
            this.btnRedo.Pressed = false;
            this.btnRedo.PressedImage = null;
            this.btnRedo.Size = new System.Drawing.Size(30, 30);
            this.btnRedo.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnRedo.TabIndex = 6;
            this.btnRedo.Click += new System.EventHandler(this.btnRedo_Click);
            // 
            // btnUndo
            // 
            this.btnUndo.Active = false;
            this.btnUndo.ActiveImage = null;
            this.btnUndo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnUndo.DisabledImage = null;
            this.btnUndo.Location = new System.Drawing.Point(167, 28);
            this.btnUndo.Name = "btnUndo";
            this.btnUndo.Pressed = false;
            this.btnUndo.PressedImage = null;
            this.btnUndo.Size = new System.Drawing.Size(30, 30);
            this.btnUndo.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnUndo.TabIndex = 5;
            this.btnUndo.Click += new System.EventHandler(this.btnUndo_Click);
            // 
            // MainToolbar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnRedo);
            this.Controls.Add(this.btnUndo);
            this.Controls.Add(this.btnDelete);
            this.Controls.Add(this.btnPaste);
            this.Controls.Add(this.btnCopy);
            this.Controls.Add(this.btnCut);
            this.Name = "MainToolbar";
            this.Size = new System.Drawing.Size(605, 68);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ColorDialog colorDialog1;
        private FlashGenie.ToggleButton btnCut;
        private FlashGenie.ToggleButton btnCopy;
        private FlashGenie.ToggleButton btnPaste;
        private FlashGenie.ToggleButton btnDelete;
        private FlashGenie.ToggleButton btnRedo;
        private FlashGenie.ToggleButton btnUndo;
    }
}
