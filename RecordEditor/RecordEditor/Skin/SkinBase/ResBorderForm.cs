using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for ResBorderForm.
	/// </summary>
	public class ResBorderForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ResBorderForm()
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
			// 
			// ResBorderForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(53)), ((System.Byte)(48)), ((System.Byte)(121)));
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.ForeColor = System.Drawing.Color.FromArgb(((System.Byte)(129)), ((System.Byte)(144)), ((System.Byte)(190)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Name = "ResBorderForm";
			this.ShowInTaskbar = false;
			this.Text = "ResBorderForm";
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.ResBorderForm_Paint);

		}
		#endregion

		ArrayList	GetRects()
		{
			int borderWidth = 3;
			ArrayList array = new ArrayList();
			Rectangle		upRect = new Rectangle(0, 0, this.Width, borderWidth);
			Rectangle		rightRect = new Rectangle(this.Width-borderWidth, borderWidth, borderWidth, this.Height-borderWidth*2);
			Rectangle		bottomRect = new Rectangle(0, this.Height-borderWidth, this.Width, borderWidth);
			Rectangle		leftRect = new Rectangle(0, borderWidth, borderWidth, this.Height-borderWidth*2);
			array.Add(upRect);
			array.Add(rightRect);
			array.Add(bottomRect);
			array.Add(leftRect);
			return array;
		}

		public void InvalidateForm(Form form)
		{
			ArrayList rects = GetRects();
			for(int i=0; i<rects.Count; i++)
			{
				Rectangle	rect = (Rectangle)rects[i];
				form.Invalidate(rect, false);
			}
			form.Update();
		}

		private void ResBorderForm_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			GraphicsPath	path = new GraphicsPath();
			ArrayList rects = GetRects();
			for(int i=0; i<rects.Count; i++)
			{
				Rectangle	rect = (Rectangle)rects[i];
				path.AddRectangle( rect);
			}
			this.Region = new Region(path);
		}

		public void RefreshRects()
		{
			InvalidateForm(this);
		}
	}
}
