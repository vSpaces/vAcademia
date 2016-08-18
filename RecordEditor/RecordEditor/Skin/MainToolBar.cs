using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace FlashGenie
{
	public class MainToolBar : FlashGenie.sePanel
	{
		public FlashGenie.seButton btnNew;
		public FlashGenie.seButton btnOpen;
        public FlashGenie.seButton btnSave;
		public FlashGenie.seButton btnCopy;
		public FlashGenie.seButton btnCut;
		public FlashGenie.seButton btnPoly;
		public FlashGenie.seButton btnCircle;
		public FlashGenie.seButton btnRect;
		public FlashGenie.seButton btnText;
		public FlashGenie.seButton btnImage;
		public FlashGenie.seButton btnPublish;
		public FlashGenie.seButton btnPreview;
		public FlashGenie.seButton btnZoomOut;
		public FlashGenie.seButton btnZoomIn;
		public FlashGenie.seButton btnPan;
		public SkinnedResizableComboBox ZoomCombo;
		public System.ComponentModel.IContainer components = null;
		public FlashGenie.seButton btnMP;
		public FlashGenie.seButton btnFlash;
        public FlashGenie.seButton btnFlashChart;
		public FlashGenie.seButton btnAviPublish;
        public seButton btnPaste;

		public	event	EventHandler	OnCommand=null;
		public	event	EventHandler	OnPressedChanged=null;

		static	public	int[]	ZoomVals = new int[]{25,50,75,100,150,200,500};

		public MainToolBar()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();
			// TODO: Add any initialization after the InitializeComponent call
			ZoomCombo.UpdateSkin();
			for(int i=0; i<ZoomVals.GetLength(0); i++)
				ZoomCombo.Items.Add(ZoomVals[i].ToString()+@"%");
			//ZoomCombo.Items.Add("Fit on window");
			ZoomCombo.SelectedIndex = 3;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainToolBar));
            this.btnNew = new FlashGenie.seButton();
            this.btnOpen = new FlashGenie.seButton();
            this.btnSave = new FlashGenie.seButton();
            this.btnCopy = new FlashGenie.seButton();
            this.btnCut = new FlashGenie.seButton();
            this.btnPoly = new FlashGenie.seButton();
            this.btnCircle = new FlashGenie.seButton();
            this.btnRect = new FlashGenie.seButton();
            this.btnText = new FlashGenie.seButton();
            this.btnImage = new FlashGenie.seButton();
            this.btnPublish = new FlashGenie.seButton();
            this.btnPreview = new FlashGenie.seButton();
            this.btnZoomOut = new FlashGenie.seButton();
            this.btnZoomIn = new FlashGenie.seButton();
            this.btnPan = new FlashGenie.seButton();
            this.ZoomCombo = new FlashGenie.SkinnedResizableComboBox();
            this.btnMP = new FlashGenie.seButton();
            this.btnFlash = new FlashGenie.seButton();
            this.btnFlashChart = new FlashGenie.seButton();
            this.btnAviPublish = new FlashGenie.seButton();
            this.btnPaste = new FlashGenie.seButton();
            this.SuspendLayout();
            // 
            // btnNew
            // 
            this.btnNew.Active = false;
            this.btnNew.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnNew.ActiveImage")));
            this.btnNew.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnNew.DisabledImage")));
            this.btnNew.Image = ((System.Drawing.Image)(resources.GetObject("btnNew.Image")));
            this.btnNew.Location = new System.Drawing.Point(16, 8);
            this.btnNew.Name = "btnNew";
            this.btnNew.Pressed = false;
            this.btnNew.PressedImage = null;
            this.btnNew.Size = new System.Drawing.Size(30, 30);
            this.btnNew.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnNew.TabIndex = 0;
            this.btnNew.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnOpen
            // 
            this.btnOpen.Active = false;
            this.btnOpen.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnOpen.ActiveImage")));
            this.btnOpen.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnOpen.DisabledImage")));
            this.btnOpen.Image = ((System.Drawing.Image)(resources.GetObject("btnOpen.Image")));
            this.btnOpen.Location = new System.Drawing.Point(48, 8);
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Pressed = false;
            this.btnOpen.PressedImage = null;
            this.btnOpen.Size = new System.Drawing.Size(30, 30);
            this.btnOpen.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnOpen.TabIndex = 1;
            this.btnOpen.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnSave
            // 
            this.btnSave.Active = false;
            this.btnSave.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnSave.ActiveImage")));
            this.btnSave.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnSave.DisabledImage")));
            this.btnSave.Enabled = false;
            this.btnSave.Image = ((System.Drawing.Image)(resources.GetObject("btnSave.Image")));
            this.btnSave.Location = new System.Drawing.Point(80, 8);
            this.btnSave.Name = "btnSave";
            this.btnSave.Pressed = false;
            this.btnSave.PressedImage = null;
            this.btnSave.Size = new System.Drawing.Size(30, 30);
            this.btnSave.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnSave.TabIndex = 2;
            this.btnSave.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnCopy
            // 
            this.btnCopy.Active = false;
            this.btnCopy.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnCopy.ActiveImage")));
            this.btnCopy.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnCopy.DisabledImage")));
            this.btnCopy.Enabled = false;
            this.btnCopy.Image = ((System.Drawing.Image)(resources.GetObject("btnCopy.Image")));
            this.btnCopy.Location = new System.Drawing.Point(272, 8);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Pressed = false;
            this.btnCopy.PressedImage = null;
            this.btnCopy.Size = new System.Drawing.Size(30, 30);
            this.btnCopy.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnCopy.TabIndex = 4;
            this.btnCopy.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnCut
            // 
            this.btnCut.Active = false;
            this.btnCut.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnCut.ActiveImage")));
            this.btnCut.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnCut.DisabledImage")));
            this.btnCut.Enabled = false;
            this.btnCut.Image = ((System.Drawing.Image)(resources.GetObject("btnCut.Image")));
            this.btnCut.Location = new System.Drawing.Point(240, 8);
            this.btnCut.Name = "btnCut";
            this.btnCut.Pressed = false;
            this.btnCut.PressedImage = null;
            this.btnCut.Size = new System.Drawing.Size(30, 30);
            this.btnCut.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnCut.TabIndex = 3;
            this.btnCut.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnPoly
            // 
            this.btnPoly.Active = false;
            this.btnPoly.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnPoly.ActiveImage")));
            this.btnPoly.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnPoly.DisabledImage")));
            this.btnPoly.Enabled = false;
            this.btnPoly.Image = ((System.Drawing.Image)(resources.GetObject("btnPoly.Image")));
            this.btnPoly.Location = new System.Drawing.Point(544, 8);
            this.btnPoly.Name = "btnPoly";
            this.btnPoly.Pressed = false;
            this.btnPoly.PressedImage = null;
            this.btnPoly.Size = new System.Drawing.Size(30, 30);
            this.btnPoly.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnPoly.TabIndex = 8;
            this.btnPoly.Click += new System.EventHandler(this.btn_Click);
            this.btnPoly.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnCircle
            // 
            this.btnCircle.Active = false;
            this.btnCircle.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnCircle.ActiveImage")));
            this.btnCircle.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnCircle.DisabledImage")));
            this.btnCircle.Enabled = false;
            this.btnCircle.Image = ((System.Drawing.Image)(resources.GetObject("btnCircle.Image")));
            this.btnCircle.Location = new System.Drawing.Point(512, 8);
            this.btnCircle.Name = "btnCircle";
            this.btnCircle.Pressed = false;
            this.btnCircle.PressedImage = null;
            this.btnCircle.Size = new System.Drawing.Size(30, 30);
            this.btnCircle.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnCircle.TabIndex = 7;
            this.btnCircle.Click += new System.EventHandler(this.btn_Click);
            this.btnCircle.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnRect
            // 
            this.btnRect.Active = false;
            this.btnRect.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnRect.ActiveImage")));
            this.btnRect.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnRect.DisabledImage")));
            this.btnRect.Enabled = false;
            this.btnRect.Image = ((System.Drawing.Image)(resources.GetObject("btnRect.Image")));
            this.btnRect.Location = new System.Drawing.Point(480, 8);
            this.btnRect.Name = "btnRect";
            this.btnRect.Pressed = false;
            this.btnRect.PressedImage = null;
            this.btnRect.Size = new System.Drawing.Size(30, 30);
            this.btnRect.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnRect.TabIndex = 6;
            this.btnRect.Click += new System.EventHandler(this.btn_Click);
            this.btnRect.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnText
            // 
            this.btnText.Active = false;
            this.btnText.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnText.ActiveImage")));
            this.btnText.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnText.DisabledImage")));
            this.btnText.Enabled = false;
            this.btnText.Image = ((System.Drawing.Image)(resources.GetObject("btnText.Image")));
            this.btnText.Location = new System.Drawing.Point(576, 8);
            this.btnText.Name = "btnText";
            this.btnText.Pressed = false;
            this.btnText.PressedImage = null;
            this.btnText.Size = new System.Drawing.Size(30, 30);
            this.btnText.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnText.TabIndex = 10;
            this.btnText.Click += new System.EventHandler(this.btn_Click);
            this.btnText.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnImage
            // 
            this.btnImage.Active = false;
            this.btnImage.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnImage.ActiveImage")));
            this.btnImage.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnImage.DisabledImage")));
            this.btnImage.Enabled = false;
            this.btnImage.Image = ((System.Drawing.Image)(resources.GetObject("btnImage.Image")));
            this.btnImage.Location = new System.Drawing.Point(424, 8);
            this.btnImage.Name = "btnImage";
            this.btnImage.Pressed = false;
            this.btnImage.PressedImage = null;
            this.btnImage.Size = new System.Drawing.Size(30, 30);
            this.btnImage.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnImage.TabIndex = 9;
            this.btnImage.Click += new System.EventHandler(this.btn_Click);
            this.btnImage.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnPublish
            // 
            this.btnPublish.Active = false;
            this.btnPublish.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnPublish.ActiveImage")));
            this.btnPublish.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnPublish.DisabledImage")));
            this.btnPublish.Enabled = false;
            this.btnPublish.Image = ((System.Drawing.Image)(resources.GetObject("btnPublish.Image")));
            this.btnPublish.Location = new System.Drawing.Point(112, 8);
            this.btnPublish.Name = "btnPublish";
            this.btnPublish.Pressed = false;
            this.btnPublish.PressedImage = null;
            this.btnPublish.Size = new System.Drawing.Size(30, 30);
            this.btnPublish.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnPublish.TabIndex = 11;
            this.btnPublish.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnPreview
            // 
            this.btnPreview.Active = false;
            this.btnPreview.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnPreview.ActiveImage")));
            this.btnPreview.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnPreview.DisabledImage")));
            this.btnPreview.Enabled = false;
            this.btnPreview.Image = ((System.Drawing.Image)(resources.GetObject("btnPreview.Image")));
            this.btnPreview.Location = new System.Drawing.Point(144, 8);
            this.btnPreview.Name = "btnPreview";
            this.btnPreview.Pressed = false;
            this.btnPreview.PressedImage = null;
            this.btnPreview.Size = new System.Drawing.Size(30, 30);
            this.btnPreview.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnPreview.TabIndex = 12;
            this.btnPreview.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnZoomOut
            // 
            this.btnZoomOut.Active = false;
            this.btnZoomOut.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnZoomOut.ActiveImage")));
            this.btnZoomOut.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnZoomOut.DisabledImage")));
            this.btnZoomOut.Enabled = false;
            this.btnZoomOut.Image = ((System.Drawing.Image)(resources.GetObject("btnZoomOut.Image")));
            this.btnZoomOut.Location = new System.Drawing.Point(768, 8);
            this.btnZoomOut.Name = "btnZoomOut";
            this.btnZoomOut.Pressed = false;
            this.btnZoomOut.PressedImage = null;
            this.btnZoomOut.Size = new System.Drawing.Size(30, 30);
            this.btnZoomOut.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnZoomOut.TabIndex = 15;
            this.btnZoomOut.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnZoomIn
            // 
            this.btnZoomIn.Active = false;
            this.btnZoomIn.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnZoomIn.ActiveImage")));
            this.btnZoomIn.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnZoomIn.DisabledImage")));
            this.btnZoomIn.Enabled = false;
            this.btnZoomIn.Image = ((System.Drawing.Image)(resources.GetObject("btnZoomIn.Image")));
            this.btnZoomIn.Location = new System.Drawing.Point(736, 8);
            this.btnZoomIn.Name = "btnZoomIn";
            this.btnZoomIn.Pressed = false;
            this.btnZoomIn.PressedImage = null;
            this.btnZoomIn.Size = new System.Drawing.Size(30, 30);
            this.btnZoomIn.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnZoomIn.TabIndex = 14;
            this.btnZoomIn.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnPan
            // 
            this.btnPan.Active = false;
            this.btnPan.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnPan.ActiveImage")));
            this.btnPan.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnPan.DisabledImage")));
            this.btnPan.Enabled = false;
            this.btnPan.Image = ((System.Drawing.Image)(resources.GetObject("btnPan.Image")));
            this.btnPan.Location = new System.Drawing.Point(632, 8);
            this.btnPan.Name = "btnPan";
            this.btnPan.Pressed = false;
            this.btnPan.PressedImage = null;
            this.btnPan.Size = new System.Drawing.Size(30, 30);
            this.btnPan.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnPan.TabIndex = 13;
            this.btnPan.Click += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // ZoomCombo
            // 
            this.ZoomCombo.Active = false;
            this.ZoomCombo.ActiveImage = null;
            this.ZoomCombo.DisabledImage = null;
            this.ZoomCombo.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
            this.ZoomCombo.DropDownWidth = 120;
            this.ZoomCombo.EastWidth = 26;
            this.ZoomCombo.Enabled = false;
            this.ZoomCombo.Image = null;
            this.ZoomCombo.ItemHeight = 16;
            this.ZoomCombo.Location = new System.Drawing.Point(664, 12);
            this.ZoomCombo.MaxDropDownItems = 32;
            this.ZoomCombo.Name = "ZoomCombo";
            this.ZoomCombo.Pressed = false;
            this.ZoomCombo.PressedImage = null;
            this.ZoomCombo.Size = new System.Drawing.Size(65, 22);
            this.ZoomCombo.TabIndex = 16;
            this.ZoomCombo.WestWidth = 7;
            // 
            // btnMP
            // 
            this.btnMP.Active = false;
            this.btnMP.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnMP.ActiveImage")));
            this.btnMP.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnMP.DisabledImage")));
            this.btnMP.Enabled = false;
            this.btnMP.Image = ((System.Drawing.Image)(resources.GetObject("btnMP.Image")));
            this.btnMP.Location = new System.Drawing.Point(192, 8);
            this.btnMP.Name = "btnMP";
            this.btnMP.Pressed = false;
            this.btnMP.PressedImage = null;
            this.btnMP.Size = new System.Drawing.Size(30, 30);
            this.btnMP.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnMP.TabIndex = 17;
            this.btnMP.Click += new System.EventHandler(this.btn_Click);
            // 
            // btnFlash
            // 
            this.btnFlash.Active = false;
            this.btnFlash.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnFlash.ActiveImage")));
            this.btnFlash.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnFlash.DisabledImage")));
            this.btnFlash.Enabled = false;
            this.btnFlash.Image = ((System.Drawing.Image)(resources.GetObject("btnFlash.Image")));
            this.btnFlash.Location = new System.Drawing.Point(360, 8);
            this.btnFlash.Name = "btnFlash";
            this.btnFlash.Pressed = false;
            this.btnFlash.PressedImage = null;
            this.btnFlash.Size = new System.Drawing.Size(30, 30);
            this.btnFlash.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnFlash.TabIndex = 18;
            this.btnFlash.Click += new System.EventHandler(this.btn_Click);
            this.btnFlash.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnFlashChart
            // 
            this.btnFlashChart.Active = false;
            this.btnFlashChart.ActiveImage = null;
            this.btnFlashChart.DisabledImage = null;
            this.btnFlashChart.Enabled = false;
            this.btnFlashChart.Location = new System.Drawing.Point(392, 8);
            this.btnFlashChart.Name = "btnFlashChart";
            this.btnFlashChart.Pressed = false;
            this.btnFlashChart.PressedImage = null;
            this.btnFlashChart.Size = new System.Drawing.Size(30, 30);
            this.btnFlashChart.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
            this.btnFlashChart.TabIndex = 18;
            this.btnFlashChart.Click += new System.EventHandler(this.btn_Click);
            this.btnFlashChart.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnAviPublish
            // 
            this.btnAviPublish.Active = false;
            this.btnAviPublish.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnAviPublish.ActiveImage")));
            this.btnAviPublish.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnAviPublish.DisabledImage")));
            this.btnAviPublish.Enabled = false;
            this.btnAviPublish.Image = ((System.Drawing.Image)(resources.GetObject("btnAviPublish.Image")));
            this.btnAviPublish.Location = new System.Drawing.Point(824, 8);
            this.btnAviPublish.Name = "btnAviPublish";
            this.btnAviPublish.Pressed = false;
            this.btnAviPublish.PressedImage = null;
            this.btnAviPublish.Size = new System.Drawing.Size(30, 30);
            this.btnAviPublish.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnAviPublish.TabIndex = 19;
            this.btnAviPublish.Visible = false;
            this.btnAviPublish.Click += new System.EventHandler(this.btn_Click);
            this.btnAviPublish.PressedChanged += new System.EventHandler(this.btnPoly_PressedChanged);
            // 
            // btnPaste
            // 
            this.btnPaste.Active = false;
            this.btnPaste.ActiveImage = ((System.Drawing.Image)(resources.GetObject("btnPaste.ActiveImage")));
            this.btnPaste.DisabledImage = ((System.Drawing.Image)(resources.GetObject("btnPaste.DisabledImage")));
            this.btnPaste.Enabled = false;
            this.btnPaste.Image = ((System.Drawing.Image)(resources.GetObject("btnPaste.Image")));
            this.btnPaste.Location = new System.Drawing.Point(304, 8);
            this.btnPaste.Name = "btnPaste";
            this.btnPaste.Pressed = false;
            this.btnPaste.PressedImage = null;
            this.btnPaste.Size = new System.Drawing.Size(30, 30);
            this.btnPaste.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnPaste.TabIndex = 5;
            this.btnPaste.Click += new System.EventHandler(this.btn_Click);
            // 
            // MainToolBar
            // 
            this.Controls.Add(this.btnAviPublish);
            this.Controls.Add(this.btnFlash);
            this.Controls.Add(this.btnFlashChart);
            this.Controls.Add(this.btnMP);
            this.Controls.Add(this.ZoomCombo);
            this.Controls.Add(this.btnZoomOut);
            this.Controls.Add(this.btnZoomIn);
            this.Controls.Add(this.btnPan);
            this.Controls.Add(this.btnPreview);
            this.Controls.Add(this.btnPublish);
            this.Controls.Add(this.btnText);
            this.Controls.Add(this.btnImage);
            this.Controls.Add(this.btnPoly);
            this.Controls.Add(this.btnCircle);
            this.Controls.Add(this.btnRect);
            this.Controls.Add(this.btnPaste);
            this.Controls.Add(this.btnCopy);
            this.Controls.Add(this.btnCut);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.btnOpen);
            this.Controls.Add(this.btnNew);
            this.Name = "MainToolBar";
            this.Size = new System.Drawing.Size(856, 40);
            this.ResumeLayout(false);

		}
		#endregion

		private void btn_Click(object sender, System.EventArgs e)
		{
			if( OnCommand!=null)	OnCommand(sender, e);
		}

		private void btnPoly_PressedChanged(object sender, System.EventArgs e)
		{
			if( OnPressedChanged!=null)	OnPressedChanged(sender, e);		
		}

		public void ShowButton(bool aviButton)
		{
			int	aviBtnShift = aviButton?32:0;
			if( aviButton)
			{
				btnAviPublish.Location = new Point(144, 8);
				btnAviPublish.Visible = aviButton;
			}
			btnPreview.Left += aviBtnShift;
			btnMP.Left += aviBtnShift;
			btnCut.Left += aviBtnShift;
			btnCopy.Left += aviBtnShift;
			btnPaste.Left += aviBtnShift;
			btnFlash.Left += aviBtnShift;
			btnFlashChart.Left += aviBtnShift;
			btnImage.Left += aviBtnShift;
			btnRect.Left += aviBtnShift;
			btnCircle.Left += aviBtnShift;
			btnPoly.Left += aviBtnShift;
			btnText.Left += aviBtnShift;
			btnPan.Left += aviBtnShift;
			ZoomCombo.Left += aviBtnShift;
			btnZoomIn.Left += aviBtnShift;
			btnZoomOut.Left += aviBtnShift;
		}
	}
}

