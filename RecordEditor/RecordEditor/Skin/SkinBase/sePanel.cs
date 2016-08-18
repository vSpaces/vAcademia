using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace FlashGenie
{
	public class sePanel : FlashGenie.SkinPanel
	{
		private System.ComponentModel.IContainer components = null;

		public sePanel()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();
			this.Paint +=new PaintEventHandler(sePanel_Paint);
			// TODO: Add any initialization after the InitializeComponent call
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
			components = new System.ComponentModel.Container();
		}
		#endregion

		public	Color	GetPixelColor(int x, int y)
		{
			return this.skinController.Skin.GetPixelColor(x,y);
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
				title = value;
				Refresh();
			}
		}

		private	Point	mtitleXY = new Point(3,3);
		public	Point	titleXY
		{
			get
			{
				return mtitleXY;
			}
			set
			{
				mtitleXY = value;
			}
		}

		private void sePanel_Paint(object sender, PaintEventArgs e)
		{
			if( Title != "")
			{
				Font defaultFont = new Font("Arial", 7, FontStyle.Bold, GraphicsUnit.Point);
				StringFormat sf = new StringFormat();
				sf.Alignment = StringAlignment.Near;
				sf.LineAlignment = StringAlignment.Far;
				Rectangle textRect = new Rectangle(titleXY.X,titleXY.Y,this.Width-6, 12);
				e.Graphics.DrawString(Title, defaultFont, Brushes.White, textRect, sf);
				defaultFont.Dispose();
			}
		}
	}
}

