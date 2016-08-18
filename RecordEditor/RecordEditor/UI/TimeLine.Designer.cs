namespace UI
{
    partial class TimeLine
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
            this.components = new System.ComponentModel.Container();
            this.btnRewind = new FlashGenie.seButton();
            this.btnPlay = new FlashGenie.seButton();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.btnPause = new FlashGenie.seButton();
            this.SuspendLayout();
            // 
            // btnRewind
            // 
            this.btnRewind.Active = false;
            this.btnRewind.ActiveImage = null;
            this.btnRewind.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnRewind.DisabledImage = null;
            this.btnRewind.Location = new System.Drawing.Point(7, 8);
            this.btnRewind.Name = "btnRewind";
            this.btnRewind.Pressed = false;
            this.btnRewind.PressedImage = null;
            this.btnRewind.Size = new System.Drawing.Size(42, 42);
            this.btnRewind.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnRewind.TabIndex = 0;
            this.btnRewind.Click += new System.EventHandler(this.btnRewind_Click);
            // 
            // btnPlay
            // 
            this.btnPlay.Active = false;
            this.btnPlay.ActiveImage = null;
            this.btnPlay.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnPlay.DisabledImage = null;
            this.btnPlay.Location = new System.Drawing.Point(49, 2);
            this.btnPlay.Name = "btnPlay";
            this.btnPlay.Pressed = false;
            this.btnPlay.PressedImage = null;
            this.btnPlay.Size = new System.Drawing.Size(55, 55);
            this.btnPlay.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnPlay.TabIndex = 1;
            this.btnPlay.Click += new System.EventHandler(this.btnPlay_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(115, 2);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 2;
            this.button1.Text = "+10 left";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Visible = false;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(196, 2);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 3;
            this.button2.Text = "+10 right";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Visible = false;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(277, 2);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 4;
            this.button3.Text = "*2";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Visible = false;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // btnPause
            // 
            this.btnPause.Active = false;
            this.btnPause.ActiveImage = null;
            this.btnPause.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.btnPause.DisabledImage = null;
            this.btnPause.Location = new System.Drawing.Point(49, 2);
            this.btnPause.Name = "btnPause";
            this.btnPause.Pressed = false;
            this.btnPause.PressedImage = null;
            this.btnPause.Size = new System.Drawing.Size(55, 55);
            this.btnPause.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnPause.TabIndex = 5;
            this.btnPause.Visible = false;
            this.btnPause.Click += new System.EventHandler(this.btnPause_Click);
            // 
            // TimeLine
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnPause);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btnPlay);
            this.Controls.Add(this.btnRewind);
            this.Name = "TimeLine";
            this.Size = new System.Drawing.Size(436, 60);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.TimeLine_Paint);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.TimeLine_MouseMove);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.TimeLine_MouseDown);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.TimeLine_MouseUp);
            this.ResumeLayout(false);

        }

        #endregion

        private FlashGenie.seButton btnRewind;
        private FlashGenie.seButton btnPlay;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private FlashGenie.seButton btnPause;
    }
}
