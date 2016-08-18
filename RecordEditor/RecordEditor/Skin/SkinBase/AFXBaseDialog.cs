using System;
using System.Drawing;
using System.Data;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for AFXBaseDialog.
	/// </summary>
	public class AFXBaseDialog : BaseDialog
	{
		private FlashGenie.seButton btnOk;
		private FlashGenie.seButton btnCancel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public AFXBaseDialog()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.btnOk = new FlashGenie.seButton();
			this.btnCancel = new FlashGenie.seButton();
			this.ContentPanel.SuspendLayout();
			// 
			// btnClose
			// 
			this.btnClose.Location = new System.Drawing.Point(285, 7);
			this.btnClose.Name = "btnClose";
			// 
			// btnMinimize
			// 
			this.btnMinimize.Location = new System.Drawing.Point(251, 7);
			this.btnMinimize.Name = "btnMinimize";
			// 
			// btnMaximize
			// 
			this.btnMaximize.Location = new System.Drawing.Point(268, 7);
			this.btnMaximize.Name = "btnMaximize";
			// 
			// btnNormal
			// 
			this.btnNormal.Location = new System.Drawing.Point(268, 7);
			this.btnNormal.Name = "btnNormal";
			// 
			// TitlePanel
			// 
			this.TitlePanel.Name = "TitlePanel";
			this.TitlePanel.Size = new System.Drawing.Size(312, 27);
			// 
			// StatusPanel
			// 
			this.StatusPanel.Location = new System.Drawing.Point(0, 110);
			this.StatusPanel.Name = "StatusPanel";
			this.StatusPanel.Size = new System.Drawing.Size(312, 27);
			// 
			// ContentPanel
			// 
			this.ContentPanel.Controls.Add(this.btnOk);
			this.ContentPanel.Controls.Add(this.btnCancel);
			this.ContentPanel.Name = "ContentPanel";
			this.ContentPanel.Size = new System.Drawing.Size(312, 83);
			// 
			// btnOk
			// 
			this.btnOk.Active = false;
			this.btnOk.ActiveImage = null;
			this.btnOk.DisabledImage = null;
			this.btnOk.Location = new System.Drawing.Point(152, 73);
			this.btnOk.Name = "btnOk";
			this.btnOk.Pressed = false;
			this.btnOk.PressedImage = null;
			this.btnOk.Size = new System.Drawing.Size(65, 22);
			this.btnOk.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.btnOk.TabIndex = 3;
			this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Active = false;
			this.btnCancel.ActiveImage = null;
			this.btnCancel.DisabledImage = null;
			this.btnCancel.Location = new System.Drawing.Point(232, 73);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Pressed = false;
			this.btnCancel.PressedImage = null;
			this.btnCancel.Size = new System.Drawing.Size(65, 22);
			this.btnCancel.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.btnCancel.TabIndex = 4;
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// AFXBaseDialog
			// 
            this.AutoScaleMode = AutoScaleMode.None;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(312, 136);
			this.Location = new System.Drawing.Point(0, 0);
			this.Name = "AFXBaseDialog";
			this.Resizable = false;
			this.StatusBar = false;
			this.Text = "Change name";
			this.Title = "Change name";
			this.SizeChanged += new System.EventHandler(this.AFXBaseDialog_SizeChanged);
			this.ContentPanel.ResumeLayout(false);

		}

		// overrides methods
		public override	string	GetSkinFileName()
		{
			return	SkinSettings.ChartNameFormSkinFilename;
		}

		#endregion

		#region Message wrappers

		private void btnOk_Click(object sender, System.EventArgs e)
		{
			OnOk();
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			OnCancel();
		}

		#endregion


		#region Message handlers

		protected virtual void OnOk()
		{
			this.DialogResult = DialogResult.OK;
			this.Close();
		}

		protected virtual void OnCancel()
		{
			this.DialogResult = DialogResult.Cancel;
			this.Close();
		}

		#endregion

		private void AFXBaseDialog_SizeChanged(object sender, System.EventArgs e)
		{
			btnOk.Top = this.Height - 63;
			btnOk.Left = this.Width - 160;
			btnCancel.Top = this.Height - 63;
			btnCancel.Left = this.Width - 80;
		}
	}
}
