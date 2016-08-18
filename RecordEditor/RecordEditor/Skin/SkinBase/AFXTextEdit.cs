using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace FlashGenie
{
	public class AFXTextEdit : TextBox
	{
		private System.ComponentModel.IContainer components = null;

		public AFXTextEdit()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			//SolidBrush	brush = new SolidBrush(Color.Yellow);
			//Platform.SetClassLong( this.Handle, Platform.GCL_HBRBACKGROUND, brush.Ha);
			this.EnabledChanged += new EventHandler(AFXTextEdit_EnabledChanged);
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
			// 
			// AFXTextEdit
			// 
			this.AutoSize = false;

		}
		#endregion

		private void AFXTextEdit_EnabledChanged(object sender, EventArgs e)
		{
			if( this.Enabled)	this.BackColor = Color.White;
			else	this.BackColor = Color.FromArgb(213,209,201);
		}
	}
}

