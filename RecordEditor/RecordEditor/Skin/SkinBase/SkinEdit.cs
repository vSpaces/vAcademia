using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for SkinEdit.
	/// </summary>
	public class SkinEdit : System.Windows.Forms.UserControl, ISkinnedControl
	{
		protected	ResizableSkinController	skinController;
		private AFXTextEdit textBox1;
		private ToggleResizableButton textedit_btnBack;

		/// <summary>
		/// Events
		/// </summary>
		public new event EventHandler	TextChanged;
		public event EventHandler	TextChangedByUser;
		public new event EventHandler	Leave;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public SkinEdit()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			// TODO: Add any initialization after the InitializeComponent call
			CheckLayout();
			//
			skinController = new ResizableSkinController(this);
			UpdateSkin();
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

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.textedit_btnBack = new FlashGenie.ToggleResizableButton();
			this.textBox1 = new FlashGenie.AFXTextEdit();
			this.SuspendLayout();
			// 
			// textedit_btnBack
			// 
			this.textedit_btnBack.Active = false;
			this.textedit_btnBack.ActiveImage = null;
			this.textedit_btnBack.DisabledImage = null;
			this.textedit_btnBack.Dock = System.Windows.Forms.DockStyle.Fill;
			this.textedit_btnBack.EastWidth = 7;
			this.textedit_btnBack.Location = new System.Drawing.Point(0, 0);
			this.textedit_btnBack.Name = "textedit_btnBack";
			this.textedit_btnBack.Pressed = false;
			this.textedit_btnBack.PressedImage = null;
			this.textedit_btnBack.Size = new System.Drawing.Size(45, 22);
			this.textedit_btnBack.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.textedit_btnBack.TabIndex = 0;
			this.textedit_btnBack.WestWidth = 7;
			// 
			// textBox1
			// 
			this.textBox1.AutoSize = false;
			this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.textBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(204)));
			this.textBox1.Location = new System.Drawing.Point(6, 5);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(32, 20);
			this.textBox1.TabIndex = 1;
			this.textBox1.Text = "";
			this.textBox1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox1_KeyDown);
			this.textBox1.Leave += new System.EventHandler(this.textBox1_Leave);
			this.textBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox1_KeyPress);
			this.textBox1.FontChanged += new System.EventHandler(this.textBox1_FontChanged);
			this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
			this.textBox1.Enter += new System.EventHandler(this.textBox1_Enter);
			// 
			// SkinEdit
			// 
			this.Controls.Add(this.textBox1);
			this.Controls.Add(this.textedit_btnBack);
			this.Name = "SkinEdit";
			this.Size = new System.Drawing.Size(45, 22);
			this.ForeColorChanged += new System.EventHandler(this.SkinEdit_ForeColorChanged);
			this.VisibleChanged += new System.EventHandler(this.SkinEdit_VisibleChanged);
			this.SizeChanged += new System.EventHandler(this.SkinEdit_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		#region ISkinnedControl Members

		public void SetSkinElement(SkinBaseElement element)
		{
			// TODO:  Add SkinEdit.SetSkinElement implementation
			textedit_btnBack.SetSkinElement(element);
		}

		#endregion

		string	sLastText = "";
		private void textBox1_TextChanged(object sender, System.EventArgs e)
		{
			if( numeric)
			{
				if( !CheckNumeric())	return;
			}
			else
			{
				sLastText = textBox1.Text;
			}
			if(TextChanged != null)
				TextChanged(sender, e);
		}

		private bool CheckNumeric()
		{
			double num=0;
			try
			{
				num = Double.Parse(textBox1.Text);
				sLastText = textBox1.Text;
			}
			catch(Exception )
			{
				textBox1.Text = sLastText;
				return false;
			}
			return true;
		}

		private void textBox1_Leave(object sender, System.EventArgs e)
		{
			if(Leave != null)
				Leave(sender, e);
			if( TextChangedByUser != null)
				TextChangedByUser(this, e);
		}

		private void SkinEdit_SizeChanged(object sender, System.EventArgs e)
		{
			CheckLayout();
		}

		void CheckLayout()
		{
			//Platform.MoveWindow(textBox1.Handle, 6, 5, this.Width-12, this.Height-9, true);
			Platform.MoveWindow(textBox1.Handle, 6, 4, this.Width-12, this.Height-7, true);	// 2006-10-09
		}

		public override string Text
		{
			get
			{
				return textBox1.Text;
			}
			set
			{
				textBox1.Text = value;
			}
		}

		bool numeric = false;
		public bool Numeric
		{
			get
			{
				return numeric;
			}
			set
			{
				numeric = value;
			}
		}

		#region	Skinning
		public bool UpdateSkin()
		{
			if(skinController==null)	return false;
			skinController.SkinFile = SkinSettings.ControlsSkinFilename;
			return true;
		}
		#endregion	//Skinning

		private void SkinEdit_ForeColorChanged(object sender, System.EventArgs e)
		{
			textBox1.ForeColor = this.ForeColor;
		}

		private void textBox1_Enter(object sender, System.EventArgs e)
		{
			textBox1.Focus();
		}

		private void textBox1_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
	
		}

		private void textBox1_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			if(numeric)
				CheckNumeric();
			if( e.KeyChar == 13)
			{
				if( TextChangedByUser != null)
					TextChangedByUser(this, e);
			}
		}

		private void SkinEdit_VisibleChanged(object sender, System.EventArgs e)
		{
			CheckLayout();
		}

		private void textBox1_FontChanged(object sender, System.EventArgs e)
		{
			CheckLayout();
		}
	}
}
