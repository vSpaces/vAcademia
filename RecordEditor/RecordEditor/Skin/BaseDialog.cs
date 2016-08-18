using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
using System.Reflection;
using System.Data;
using System.IO;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for BaseDialog.
	/// </summary>
	public class BaseDialog : System.Windows.Forms.Form, IResizableSkinnedControl
	{
		static private	ArrayList	OpenedWindows = new ArrayList();
		private	ResizableSkinController	skinController;
		protected FlashGenie.seButton btnClose;
		protected FlashGenie.seButton btnMinimize;
		protected FlashGenie.seButton btnMaximize;
		protected FlashGenie.seButton btnNormal;
		protected FlashGenie.sePanel TitlePanel;
		// Moving variables
		private bool movingStarted = false;
		private Point movingPoint;
		private ResizableButton btnResize;
		private bool resizeStarted = false;
		public FlashGenie.sePanel StatusPanel;
		public System.Windows.Forms.Panel ContentPanel;
		protected ResBorderForm resizeForm = null;
		Point	resizeMouseStartPos = Point.Empty;
        private System.Windows.Forms.Timer moveDelayTimer;
        private System.ComponentModel.IContainer components;

		public BaseDialog()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			skinController = new ResizableSkinController(this);
			SkinFile = SkinSettings.SkinDirectory + SkinSettings.BaseFormSkinFilename;
			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			Type type = this.GetType();
			Assembly assembly = Assembly.GetAssembly(type);
			string iconsPath = "FlashGenie.res.";
			Stream stream = null;
#if	QVIZ_VERSION
			stream = assembly.GetManifestResourceStream(iconsPath + "QuickViz.ico");
#else
			stream = assembly.GetManifestResourceStream(iconsPath + "FlashGenie.ico");
#endif
			if( stream!=null)
				this.Icon = new Icon(stream);

			TitlePanel.Paint +=new PaintEventHandler(TitlePanel_Paint);
			//
			mtitleBarHeight = TitlePanel.Height;
			mstatusBarHeight = 27;//StatusPanel.Height;
			//
			StatusBar = true;
			Resizable = true;
			StatusPanel.Visible = StatusBar;
			btnMaximize.Visible = MaximizeBox;
			btnNormal.Visible = false;
			btnMinimize.Visible = MinimizeBox;
			OnResizableChanged();
			CheckLayout();

			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);

			CorrectWindowStyles();
		}

		public virtual void CorrectWindowStyles()
		{
			int styles = Platform.GetWindowExStyle(this.Handle);
			styles |= Platform.WS_EX_TOOLWINDOW;			styles &= ~Platform.WS_EX_APPWINDOW;
			Platform.SetWindowExStyle( this.Handle, styles);
		}

		public virtual	string	GetSkinFileName()
		{
			return	SkinSettings.BaseFormSkinFilename;
		}

		public virtual	void	UpdateSkin()
		{
			SkinFile = SkinSettings.SkinDirectory + GetSkinFileName();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region	Behavior properties

		private	bool	resizable = false;
		/// <summary>
		/// Resizable attribute
		/// </summary>
		[System.ComponentModel.CategoryAttribute("Behavior")]
		public	bool	Resizable
		{
			get
			{
				return resizable;
			}
			set
			{
				if(resizable != value)
				{
					resizable = value;
					OnResizableChanged();
				}
			}
		}

		public Color panelsColor
		{
			get
			{
				return Settings.PanelsColor;
			}
			set
			{
				Settings.PanelsColor = value;
			}
		}

		public string animIcons
		{
			get
			{
				return Settings.AnimIcons;
			}
			set
			{
				Settings.AnimIcons = value;
			}
		}

		public Color animationsTracksColor
		{
			get
			{
				return Settings.AnimationsTracksColor;
			}
			set
			{
				Settings.AnimationsTracksColor = value;
			}
		}

		public Color animationsTracksSelectedColor
		{
			get
			{
				return Settings.AnimationsTracksSelectedColor;
			}
			set
			{
				Settings.AnimationsTracksSelectedColor = value;
			}
		}

		public Color menuItemBackColor
		{
			get
			{
				return Settings.MenuItemBackColor;
			}
			set
			{
				Settings.MenuItemBackColor = value;
			}
		}

		public Color animPanelBackColor
		{
			get
			{
				return Settings.AnimPanelBackColor;
			}
			set
			{
				Settings.AnimPanelBackColor = value;
			}
		}

		public Color animPanelSelectedColor
		{
			get
			{
				return Settings.AnimPanelSelectedColor;
			}
			set
			{
				Settings.AnimPanelSelectedColor = value;
			}
		}

		public Color animPopupBackColor
		{
			get
			{
				return Settings.AnimPopupBackColor;
			}
			set
			{
				Settings.AnimPopupBackColor = value;
			}
		}

		public Color resizeBorderColor
		{
			get
			{
				return Settings.ResizeBorderColor;
			}
			set
			{
				Settings.ResizeBorderColor = value;
			}
		}

		public Color menuItemForeColor
		{
			get
			{
				return Settings.MenuItemForeColor;
			}
			set
			{
				Settings.MenuItemForeColor = value;
			}
		}

		public Color menuItemSelectedForeColor
		{
			get
			{
				return Settings.MenuItemSelectedForeColor;
			}
			set
			{
				Settings.MenuItemSelectedForeColor = value;
			}
		}

		public Color menuItemSelectedBackColor
		{
			get
			{
				return Settings.MenuItemSelectedBackColor;
			}
			set
			{
				Settings.MenuItemSelectedBackColor = value;
			}
		}

		public Color menuItemDisabledForeColor
		{
			get
			{
				return Settings.MenuItemDisabledForeColor;
			}
			set
			{
				Settings.MenuItemDisabledForeColor = value;
			}
		}

		public Color timeLineColor
		{
			get
			{
				return Settings.TimeLineColor;
			}
			set
			{
				Settings.TimeLineColor = value;
			}
		}

		public Color lcSelectedLayerBackColor
		{
			get
			{
				return Settings.SelectedLayerBackColor;
			}
			set
			{
				Settings.SelectedLayerBackColor = value;
			}
		}

		public Color lcObjectNameBackColor
		{
			get
			{
				return Settings.LCObjectNameBackColor;
			}
			set
			{
				Settings.LCObjectNameBackColor = value;
			}
		}

		public Color comboItemSelectedFrameColor
		{
			get
			{
				return Settings.ComboItemSelectedFrameColor;
			}
			set
			{
				Settings.ComboItemSelectedFrameColor = value;
			}
		}

		public Color labelsColor
		{
			get
			{
				return Settings.LabelsForeColor;
			}
			set
			{
				Settings.LabelsForeColor = value;
			}
		}

		string m_iconFileName = "";
		public string iconFileName
		{
			get
			{
				return m_iconFileName;
			}
			set
			{
				if( value != m_iconFileName)
				{
					try
					{
						FileStream	fs = new FileStream(SkinSettings.BuildFullPath(SkinSettings.SkinDirectory+value), FileMode.Open);
						if( fs != null)
						{
							m_iconFileName = value;
							this.Icon = new Icon(fs);
						}
					}
					catch(Exception)
					{
					}
				}
			}
		}

		private	bool	statusbar = false;
		/// <summary>
		/// Resizable attribute
		/// </summary>
		[System.ComponentModel.CategoryAttribute("Behavior")]
		public	bool	StatusBar
		{
			get
			{
				return statusbar;
			}
			set
			{
				if(statusbar != value)
				{
					statusbar = value;
					OnStatusBarChanged();
				}
			}
		}

		protected	void	OnResizableChanged()
		{
			btnResize.Visible = Resizable;
			if( Resizable)
			{
				StatusBar = true;
				if( MaximizeBox)
					btnMaximize.Visible = true;
			}
			else
			{
				btnMaximize.Enabled = false;
			}
			if( StatusPanel.Visible)
				StatusPanel.Refresh();
			CheckLayout();
		}

		protected	void	OnStatusBarChanged()
		{
			StatusPanel.Visible = StatusBar;
		}

		#endregion	Behavior properties

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BaseDialog));
            this.btnClose = new FlashGenie.seButton();
            this.btnMaximize = new FlashGenie.seButton();
            this.btnNormal = new FlashGenie.seButton();
            this.btnMinimize = new FlashGenie.seButton();
            this.TitlePanel = new FlashGenie.sePanel();
            this.StatusPanel = new FlashGenie.sePanel();
            this.btnResize = new FlashGenie.ResizableButton();
            this.ContentPanel = new System.Windows.Forms.Panel();
            this.moveDelayTimer = new System.Windows.Forms.Timer(this.components);
            this.TitlePanel.SuspendLayout();
            this.StatusPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnClose
            // 
            this.btnClose.Active = false;
            this.btnClose.ActiveImage = null;
            this.btnClose.DisabledImage = null;
            this.btnClose.Location = new System.Drawing.Point(352, 7);
            this.btnClose.Name = "btnClose";
            this.btnClose.Pressed = false;
            this.btnClose.PressedImage = null;
            this.btnClose.Size = new System.Drawing.Size(22, 15);
            this.btnClose.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnClose.TabIndex = 18;
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // btnMaximize
            // 
            this.btnMaximize.Active = false;
            this.btnMaximize.ActiveImage = null;
            this.btnMaximize.DisabledImage = null;
            this.btnMaximize.Location = new System.Drawing.Point(337, 7);
            this.btnMaximize.Name = "btnMaximize";
            this.btnMaximize.Pressed = false;
            this.btnMaximize.PressedImage = null;
            this.btnMaximize.Size = new System.Drawing.Size(22, 15);
            this.btnMaximize.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnMaximize.TabIndex = 18;
            this.btnMaximize.Click += new System.EventHandler(this.btnMaximize_Click);
            // 
            // btnNormal
            // 
            this.btnNormal.Active = false;
            this.btnNormal.ActiveImage = null;
            this.btnNormal.DisabledImage = null;
            this.btnNormal.Location = new System.Drawing.Point(337, 7);
            this.btnNormal.Name = "btnNormal";
            this.btnNormal.Pressed = false;
            this.btnNormal.PressedImage = null;
            this.btnNormal.Size = new System.Drawing.Size(15, 14);
            this.btnNormal.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnNormal.TabIndex = 18;
            this.btnNormal.Click += new System.EventHandler(this.btnNormal_Click);
            // 
            // btnMinimize
            // 
            this.btnMinimize.Active = false;
            this.btnMinimize.ActiveImage = null;
            this.btnMinimize.DisabledImage = null;
            this.btnMinimize.Location = new System.Drawing.Point(322, 7);
            this.btnMinimize.Name = "btnMinimize";
            this.btnMinimize.Pressed = false;
            this.btnMinimize.PressedImage = null;
            this.btnMinimize.Size = new System.Drawing.Size(22, 15);
            this.btnMinimize.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnMinimize.TabIndex = 18;
            this.btnMinimize.Click += new System.EventHandler(this.btnMinimize_Click);
            // 
            // TitlePanel
            // 
            this.TitlePanel.Controls.Add(this.btnMaximize);
            this.TitlePanel.Controls.Add(this.btnNormal);
            this.TitlePanel.Controls.Add(this.btnMinimize);
            this.TitlePanel.Controls.Add(this.btnClose);
            this.TitlePanel.Location = new System.Drawing.Point(0, 0);
            this.TitlePanel.Name = "TitlePanel";
            this.TitlePanel.Size = new System.Drawing.Size(384, 27);
            this.TitlePanel.SkinFile = "";
            this.TitlePanel.TabIndex = 1;
            this.TitlePanel.TabStop = false;
            this.TitlePanel.Title = "";
            this.TitlePanel.titleXY = new System.Drawing.Point(3, 3);
            this.TitlePanel.DoubleClick += new System.EventHandler(this.TitlePanel_DoubleClick);
            this.TitlePanel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.TitlePanel_MouseMove);
            this.TitlePanel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.TitlePanel_MouseDown);
            this.TitlePanel.MouseUp += new System.Windows.Forms.MouseEventHandler(this.TitlePanel_MouseUp);
            // 
            // StatusPanel
            // 
            this.StatusPanel.Controls.Add(this.btnResize);
            this.StatusPanel.Location = new System.Drawing.Point(0, 234);
            this.StatusPanel.Name = "StatusPanel";
            this.StatusPanel.Size = new System.Drawing.Size(384, 27);
            this.StatusPanel.SkinFile = "";
            this.StatusPanel.TabIndex = 0;
            this.StatusPanel.TabStop = false;
            this.StatusPanel.Title = "";
            this.StatusPanel.titleXY = new System.Drawing.Point(3, 3);
            this.StatusPanel.Load += new System.EventHandler(this.StatusPanel_Load);
            this.StatusPanel.SizeChanged += new System.EventHandler(this.panel3_SizeChanged);
            // 
            // btnResize
            // 
            this.btnResize.Active = false;
            this.btnResize.ActiveImage = null;
            this.btnResize.Cursor = System.Windows.Forms.Cursors.SizeNWSE;
            this.btnResize.DisabledImage = null;
            this.btnResize.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnResize.Location = new System.Drawing.Point(372, 15);
            this.btnResize.Name = "btnResize";
            this.btnResize.Pressed = false;
            this.btnResize.PressedImage = null;
            this.btnResize.Size = new System.Drawing.Size(12, 12);
            this.btnResize.Style = FlashGenie.FlatToggleButtonStyles.Normal;
            this.btnResize.TabIndex = 0;
            this.btnResize.OnBeforeResizeStart += new System.EventHandler(this.btnResize_OnBeforeResizeStart);
            this.btnResize.OnResizeEnd += new System.EventHandler(this.btnResize_OnResizeEnd);
            this.btnResize.OnResizeStart += new System.EventHandler(this.btnResize_OnResizeStart);
            this.btnResize.OnResize += new System.EventHandler(this.btnResize_OnResize);
            // 
            // ContentPanel
            // 
            this.ContentPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(122)))), ((int)(((byte)(179)))));
            this.ContentPanel.Location = new System.Drawing.Point(1, 27);
            this.ContentPanel.Name = "ContentPanel";
            this.ContentPanel.Size = new System.Drawing.Size(384, 207);
            this.ContentPanel.TabIndex = 2;
            // 
            // moveDelayTimer
            // 
            this.moveDelayTimer.Interval = 300;
            this.moveDelayTimer.Tick += new System.EventHandler(this.moveDelayTimer_Tick);
            // 
            // BaseDialog
            // 
            this.ClientSize = new System.Drawing.Size(384, 261);
            this.Controls.Add(this.ContentPanel);
            this.Controls.Add(this.StatusPanel);
            this.Controls.Add(this.TitlePanel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "BaseDialog";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "BaseDialog";
            this.Load += new System.EventHandler(this.BaseDialog_Load);
            this.SizeChanged += new System.EventHandler(this.BaseDialog_SizeChanged);
            this.VisibleChanged += new System.EventHandler(this.BaseDialog_VisibleChanged);
            this.Closing += new System.ComponentModel.CancelEventHandler(this.BaseDialog_Closing);
            this.TitlePanel.ResumeLayout(false);
            this.StatusPanel.ResumeLayout(false);
            this.ResumeLayout(false);

		}
		#endregion

		#region IResizableSkinnedControl Members

		public bool SetSkin(string	fullName)
		{
			skinController.SkinFile = fullName;
			return true;
		}

		public bool SetSkin(FormSkin	skin)
		{
			if(skinController==null)	return false;
			skinController.Skin = skin;
			return true;
		}

		public	void	SkinControls()
		{
			skinController.SkinControls();
		}

		/// <summary>
		/// 
		/// </summary>
		private string sSkinFile = "";

		[CategoryAttribute("Appearance")]
		public string SkinFile
		{
			set
			{
				if( sSkinFile != value)
				{
					sSkinFile = value;
					SetSkin(sSkinFile);
				}
			}
			get
			{
				return sSkinFile;
			}
		}

		/// <summary>
		/// 
		/// </summary>
		private Form parentForm = null;
		public new Form ParentForm
		{
			get
			{
				return parentForm;
			}
			set
			{
				parentForm = value;
			}
		}

		#endregion

		/// <summary>
		/// Height of the title bar
		/// </summary>
		protected int	mtitleBarHeight=0;
		public int	titleBarHeight
		{
			get
			{
				return mtitleBarHeight;
			}
			set
			{
				mtitleBarHeight = value;
				CheckLayout();
			}
		}

		/// <summary>
		/// Height of the status bar
		/// </summary>
		protected int	mstatusBarHeight=0;
		public int	statusBarHeight
		{
			get
			{
				return mstatusBarHeight;
			}
			set
			{
				mstatusBarHeight = value;
				CheckLayout();
			}
		}

        /// <summary>
        /// Horizontal shift for resize button
        /// </summary>
        protected int	mresizeRightMargin=6;
        public int	resizeBtnRightMargin
        {
            get
            {
                return mresizeRightMargin;
            }
            set
            {
                mresizeRightMargin = value;
                panel3_SizeChanged(btnResize, new EventArgs());
            }
        }

        /// <summary>
        /// Horizontal shift for resize button
        /// </summary>
        protected int	mresizeBottomMargin=8;
        public int	resizeBtnBottomMargin
        {
            get
            {
                return mresizeBottomMargin;
            }
            set
            {
                mresizeBottomMargin = value;
                panel3_SizeChanged(btnResize, new EventArgs());
            }
        }

        /// <summary>
        /// 
        /// </summary>
        protected int	mleftMargin=0;
        protected int	mrightMargin=0;
        protected int	mtopMargin=0;
        protected int	mbottomMargin=0;
        public int	leftMargin
        {
            get{    return mleftMargin;}
            set{    mleftMargin = value;    CheckLayout();}
        }
        public int	rightMargin
        {
            get{    return mrightMargin;}
            set{    mrightMargin = value;    CheckLayout();}
        }
        public int	topMargin
        {
            get{    return mtopMargin;}
            set{    mtopMargin = value;    CheckLayout();}
        }
        public int	bottomMargin
        {
            get{    return mbottomMargin;}
            set{    mbottomMargin = value;    CheckLayout();}
        }

        void CheckLayout()
		{
            TitlePanel.Left = mleftMargin;
            TitlePanel.Top = mtopMargin;
			TitlePanel.Width = this.Width - mleftMargin - mrightMargin;

            ContentPanel.Left = mleftMargin;
			ContentPanel.Width = this.Width - mleftMargin - mrightMargin;;

			// Set panels height
			TitlePanel.Height = mtitleBarHeight;
			StatusPanel.Height = mstatusBarHeight;

			ContentPanel.Top = TitlePanel.Height + mtopMargin;

			if( StatusBar)
			{
                StatusPanel.Left = mleftMargin;
                StatusPanel.Width = this.Width - mleftMargin - mrightMargin;
				StatusPanel.Top = this.Height - StatusPanel.Height + 1 - mbottomMargin;
				ContentPanel.Height = this.Height - StatusPanel.Height - TitlePanel.Height + 1 - mbottomMargin - mtopMargin;
			}
			else
			{
				ContentPanel.Height = this.Height - TitlePanel.Height - 8 - mbottomMargin - mtopMargin;
			}
			//
            int sysBtnsDistance = 3;
			btnClose.Left = this.Width - 37;
            btnMaximize.Left = btnClose.Left - btnClose.Width - sysBtnsDistance;
            btnNormal.Left = btnMaximize.Left;
			if( MaximizeBox)
                btnMinimize.Left = btnMaximize.Left - btnMaximize.Width - sysBtnsDistance;
			else
                btnMinimize.Left = btnMaximize.Left;

			if( ParentForm != null)
				ParentForm.Refresh();
		}
        
		private void BaseDialog_SizeChanged(object sesnder, System.EventArgs e)
		{
			CheckLayout();
			if( WindowState == FormWindowState.Maximized)
			{
				btnMaximize.Visible = false;
				btnNormal.Visible = true;
			}
			else	if( WindowState == FormWindowState.Normal)
			{
				btnMaximize.Visible = true;
				btnNormal.Visible = false;
			}
		}

		private void btnClose_Click(object sender, System.EventArgs e)
		{
			OnClosing();
			this.Close();
		}

		protected	virtual	void	OnClosing()
		{
		}

		#region Moving
		
		private void TitlePanel_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
            movingPoint = new Point(e.X, e.Y);
            if (e.Button == MouseButtons.Left && this.WindowState == FormWindowState.Normal)
                moveDelayTimer.Enabled = true;
		}

        private void onTitlePanelMouseDown(int x, int y)
        {
            movingStarted = true;
            movingPoint = new Point(x, y);
            CreateAndAlignResizeForm();
            ShowResizeForm();
            InvalidateResizeForm();
        }

		private void TitlePanel_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (movingStarted)
			{
				int newX = this.Left + (e.X - movingPoint.X);
				int newY = this.Top + (e.Y - movingPoint.Y);
				//MoveWindow(this.Handle, newX, newY, this.Width, this.Height, true);
				/*if( ParentForm != null)
					ParentForm.Refresh();*/
				Platform.MoveWindow(resizeForm.Handle, newX, newY, this.Width, this.Height, true);
				InvalidateResizeForm();
			}						
		}

		private void TitlePanel_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
            moveDelayTimer.Enabled = false;
            if( movingStarted)
            {
                movingStarted = false;
                if( resizeForm != null)
                    Platform.MoveWindow(Handle, resizeForm.Left, resizeForm.Top, this.Width, this.Height, true);
                HideResizeForm();
            }
		}

		virtual public bool ConfirmClosing()
		{
			return true;
		}

		private void TitlePanel_DoubleClick(object sender, EventArgs e)
		{
			Rectangle	iconRect = new Rectangle(0,0,31,25);
			if( iconRect.Contains( movingPoint))
			{
				this.DialogResult = DialogResult.Cancel;
				if( ConfirmClosing())
					this.Close();
				return;
			}
			if( Resizable)
			{
				if (this.WindowState == FormWindowState.Maximized)
					this.WindowState = FormWindowState.Normal;
				else if (this.WindowState == FormWindowState.Normal)
					this.WindowState = FormWindowState.Maximized;
			}
		}
		#endregion

		private void TitlePanel_SizeChanged(object sender, System.EventArgs e)
		{
			btnClose.Left = this.Width - 30;
		}

		private string title="";
		public string Title
		{
			get
			{
				return title;
			}
			set
			{
				Text = title = value;
				TitlePanel.Refresh();
			}
		}

		private Point mtitleLocation=new Point(37, 5);
		public Point titleLocation
		{
			get
			{
				return mtitleLocation;
			}
			set
			{
				mtitleLocation = value;
				TitlePanel.Refresh();
			}
		}

		private bool mtitleItalic=false;
		public bool titleItalic
		{
			get
			{
				return mtitleItalic;
			}
			set
			{
				mtitleItalic = value;
				TitlePanel.Refresh();
			}
		}

		private void TitlePanel_Paint(object sender, PaintEventArgs e)
		{
			if( Title != "")
			{
				Font defaultFont = new Font("Arial", 11, FontStyle.Bold | (mtitleItalic?FontStyle.Italic:0), GraphicsUnit.Point);
				StringFormat sf = new StringFormat();
				sf.Alignment = StringAlignment.Near;
				sf.LineAlignment = StringAlignment.Near;
				Rectangle textRect = new Rectangle(titleLocation.X,titleLocation.Y,TitlePanel.Width-37-30, TitlePanel.Height);
				e.Graphics.DrawString(Title, defaultFont, Brushes.White, textRect, sf);
				defaultFont.Dispose();
			}
		}

		private void panel3_SizeChanged(object sender, EventArgs e)
		{
			btnResize.Left = StatusPanel.Width - btnResize.Width - mresizeRightMargin;
			btnResize.Top = StatusPanel.Height - btnResize.Height + 1 - mresizeBottomMargin;
		}

		#region Resize point handlers

		void CreateAndAlignResizeForm()
		{
			if( resizeForm == null)
				resizeForm = new ResBorderForm();
			resizeForm.Left = Left;
			resizeForm.Top = Top;
			resizeForm.Width = Width;
			resizeForm.Height = Height;
			resizeForm.BackColor = Settings.ResizeBorderColor;
			Platform.MoveWindow(resizeForm.Handle, this.Left, this.Top, Width, Height, true);
		}

		void ShowResizeForm()
		{
			if( resizeForm != null)
			{
				resizeForm.Show();
			}
		}

		void HideResizeForm()
		{
			if( resizeForm != null)
				resizeForm.Hide();
		}

		void InvalidateResizeForm()
		{
			if( resizeForm == null)
				return;
			resizeForm.RefreshRects();
			//resizeForm.InvalidateForm(this);
			//if( this.ParentForm != null)
			//	resizeForm.InvalidateForm(this.ParentForm);
			for( int i=0; i<BaseDialog.OpenedWindows.Count; i++)
			{
				BaseDialog	bd = (BaseDialog)BaseDialog.OpenedWindows[i];
				resizeForm.InvalidateForm(bd);
			}
		}

		/*private void resizePoint_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (this.WindowState == FormWindowState.Normal)
			{
				btnResize.Capture = true;
				resizeStarted = true;
				// Create resize form
				CreateAndAlignResizeForm();
				ShowResizeForm();
				btnResize.Capture = true;
				resizeMouseStartPos = new Point(e.X, e.Y);
				InvalidateResizeForm();
			}
		}

		private void resizePoint_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (resizeStarted)	
			{
				int newWidth = this.Width + (e.X - resizeMouseStartPos.X); 
				int newHeight = this.Height + (e.Y - resizeMouseStartPos.Y); 
				if( newWidth < this.MinimumSize.Width)
					newWidth = this.MinimumSize.Width;
				if( newHeight < this.MinimumSize.Height)
					newHeight = this.MinimumSize.Height;
				Platform.MoveWindow(resizeForm.Handle, this.Left, this.Top, newWidth, newHeight, true);
				InvalidateResizeForm();
			}
		}

		private void resizePoint_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (resizeStarted)	
			{
				resizeStarted = false;
				btnResize.Capture = false;
				Platform.MoveWindow(Handle, resizeForm.Left, resizeForm.Top, resizeForm.Width, resizeForm.Height, true);
				HideResizeForm();
			}
		}*/

		#endregion

		private void btnMinimize_Click(object sender, EventArgs e)
		{
			WindowState = FormWindowState.Minimized;
		}

		private void btnMaximize_Click(object sender, EventArgs e)
		{
			WindowState = FormWindowState.Maximized;
		}

		private void btnNormal_Click(object sender, EventArgs e)
		{
			WindowState = FormWindowState.Normal;
		}

		private void StatusPanel_Load(object sender, System.EventArgs e)
		{
		
		}

		private void BaseDialog_Load(object sender, System.EventArgs e)
		{
			UpdateSkin();
		}

		private void BaseDialog_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			if( !ConfirmClosing())
				e.Cancel = true;
		}

		private void BaseDialog_VisibleChanged(object sender, System.EventArgs e)
		{
			BaseDialog.OpenedWindows.Remove(this);
			if( this.Visible)
			{
				BaseDialog.OpenedWindows.Add(this);
				UpdateFontsLayouts( this);
			}
		}

        private void moveDelayTimer_Tick(object sender, System.EventArgs e)
        {
            onTitlePanelMouseDown( movingPoint.X, movingPoint.Y);
        }

		protected void btnResize_OnResize(object sender, EventArgs e)
		{
			InvalidateResizeForm();
		}

		protected void btnResize_OnResizeEnd(object sender, EventArgs e)
		{
			Platform.MoveWindow(Handle, resizeForm.Left, resizeForm.Top, resizeForm.Width, resizeForm.Height, true);
			HideResizeForm();
		}

		protected void btnResize_OnResizeStart(object sender, EventArgs e)
		{
			ShowResizeForm();
			InvalidateResizeForm();
		}

		protected void btnResize_OnBeforeResizeStart(object sender, EventArgs e)
		{
			CreateAndAlignResizeForm();
			((ResizableButton)sender).ResizableForm = this;
			((ResizableButton)sender).ResizableBounds = this.resizeForm;
		}

		private void UpdateFontsLayouts( Control aControl)
		{
			foreach(Control control in aControl.Controls)
			{
				UpdateFontsLayouts( control);
			}
			if( aControl.GetType() == typeof(System.Windows.Forms.Label))
			{
				Platform.UpdateFont( this, Platform.GetLabelsFont());
			}
		}
	}

	public class MainFormDialog : BaseDialog
	{
		public MainFormDialog()
		{
		}

		public override void CorrectWindowStyles()
		{
            try
            {
                Platform.SetWindowStyle(this.Handle, Platform.GetWindowStyle(this.Handle) | Platform.WS_MINIMIZEBOX);
            }
            catch (Exception)
            {
                //??
            }
		}
	}
}
