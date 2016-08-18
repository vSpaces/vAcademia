	using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace FlashGenie
{
	public class seRButton : seButton
	{
		public seRButton()
		{
		}
		protected override void OnMouseDown(MouseEventArgs e)
		{
			if( !Pressed)
			{
				base.OnMouseDown(e);
			}
		}
	}

	/// <summary>
	/// Summary description for seRadioButton.
	/// </summary>
	public class seRadioButton : System.Windows.Forms.UserControl, ISkinnedControl
	{
		public event EventHandler	CheckedChanged;
		private FlashGenie.seRButton radio_btnBack;
		private System.Windows.Forms.Label label1;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		protected	ResizableSkinController	skinController;
		public seRadioButton()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			skinController = new ResizableSkinController(this);
			UpdateSkin();
			// TODO: Add any initialization after the InitializeComponent call
		}

		#region	Skinning
		public bool UpdateSkin()
		{
			if(skinController==null)	return false;
			skinController.SkinFile = SkinSettings.ControlsSkinFilename;
			return true;
		}
		#endregion	//Skinning

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

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.radio_btnBack = new FlashGenie.seRButton();
			this.label1 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// radio_btnBack
			// 
			this.radio_btnBack.Active = false;
			this.radio_btnBack.ActiveImage = null;
			this.radio_btnBack.DisabledImage = null;
			this.radio_btnBack.Location = new System.Drawing.Point(0, 0);
			this.radio_btnBack.Name = "radio_btnBack";
			this.radio_btnBack.Pressed = false;
			this.radio_btnBack.PressedImage = null;
			this.radio_btnBack.Size = new System.Drawing.Size(16, 16);
			this.radio_btnBack.Style = FlashGenie.FlatToggleButtonStyles.Toggle;
			this.radio_btnBack.TabIndex = 0;
			this.radio_btnBack.PressedChanged += new System.EventHandler(this.radio_btnBack_PressedChanged);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(24, 0);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(100, 16);
			this.label1.TabIndex = 1;
			this.label1.Text = "label1";
			this.label1.Click += new System.EventHandler(this.label1_Click);
			// 
			// seRadioButton
			// 
			this.Controls.Add(this.label1);
			this.Controls.Add(this.radio_btnBack);
			this.Name = "seRadioButton";
			this.Size = new System.Drawing.Size(128, 16);
			this.SizeChanged += new System.EventHandler(this.seRadioButton_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		#region ISkinnedControl Members

		public void SetSkinElement(SkinBaseElement element)
		{
			radio_btnBack.SetSkinElement(element);
			OnButtonSizeChanged();
		}

		#endregion

		public bool Checked
		{
			get
			{
				return radio_btnBack.Pressed;
			}
			set
			{
				if( radio_btnBack.Pressed != value)
				{
					radio_btnBack.Pressed = value;
					OnCheckedChanged();
				}
			}
		}

		public string Label
		{
			get
			{
				return label1.Text;
			}
			set
			{
				label1.Text = value;
			}
		}

		void OnCheckedChanged()
		{
			if(CheckedChanged!=null)
				CheckedChanged(this, new EventArgs());
		}

		private void radio_btnBack_PressedChanged(object sender, EventArgs e)
		{
			OnCheckedChanged();
		}

		void OnButtonSizeChanged()
		{
			label1.Left = radio_btnBack.Right+4;
			label1.Top = radio_btnBack.Top + (radio_btnBack.Height-label1.Height)/2;
			label1.Width = this.Width - label1.Left;
		}

		private void seRadioButton_SizeChanged(object sender, EventArgs e)
		{
			OnButtonSizeChanged();
		}

		#region Skinning
		/// <summary>
		/// 
		/// </summary>
		private string sSkinFile = "";
		[CategoryAttribute("Appearance")]
		public string SkinFile
		{
			set
			{
				sSkinFile = value;
				SetSkin(sSkinFile);
			}
			get
			{
				return sSkinFile;
			}
		}

		public bool SetSkin(string	fullName)
		{
			if(skinController==null)	return false;
			skinController.SkinFile = fullName;
			return true;
		}
		#endregion	//Skinning

		private void label1_Click(object sender, System.EventArgs e)
		{
			if( !radio_btnBack.Pressed)
				radio_btnBack.Pressed = true;
		}
	}
}
