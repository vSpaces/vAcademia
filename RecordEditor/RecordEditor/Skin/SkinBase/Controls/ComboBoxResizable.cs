using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using FlashGenie; 
using System.Drawing.Imaging;

namespace FlashGenie
{

	#region Resizable combo box
	/// <summary>
	/// Flat button with custom styles.
	/// </summary>
	public class SkinnedResizableComboBox : SkinnedComboBox, ISkinnedControl
	{
		protected	ResizableSkinController	skinController;

		public	SkinnedResizableComboBox()
		{
			skinController = new ResizableSkinController(this);
			this.VisibleChanged +=new EventHandler(SkinnedResizableComboBox_VisibleChanged);
		}

		/// <summary>
		/// Width of the left button size area
		/// </summary>
		protected	int	westWidth=7;
		public	int	WestWidth
		{
			get
			{
				return westWidth;
			}
			set
			{
				westWidth = value;
				OnPartsChanged();
			}
		}

		/// <summary>
		/// Width of the right button size area
		/// </summary>
		protected	int	eastWidth=26;
		public	int	EastWidth
		{
			get
			{
				return eastWidth;
			}
			set
			{
				eastWidth = value;
				OnPartsChanged();
			}
		}

		void OnPartsChanged()
		{
			Invalidate();
			Update();
		}

		protected override void DrawButtonImage(Graphics graphics, Image image)
		{
			if( WestWidth != 0)
			{
				graphics.DrawImage(image, new Rectangle(Point.Empty, new Size(WestWidth, this.Height)),
					0, 0, WestWidth, this.Height, GraphicsUnit.Pixel);
			}
			if( EastWidth != 0)
			{
				graphics.DrawImage(image, new Rectangle(this.Width-EastWidth, 0, EastWidth, this.Height),
					image.Width-EastWidth, 0, EastWidth, this.Height, GraphicsUnit.Pixel);
			}
			graphics.DrawImage(image, new Rectangle(WestWidth, 0, this.Width-EastWidth-WestWidth, this.Height),
				WestWidth, 0, image.Width-EastWidth-WestWidth, this.Height, GraphicsUnit.Pixel);
		}

		[DllImportAttribute("user32.dll")]
		private static extern bool MoveWindow(IntPtr hWnd, int x, int y, int width, int height, bool repaint);

		protected override void OnSkinElementChanged(SkinBaseElement element)
		{
			Image image = null;
			if( ActiveImage!=null)	image = ActiveImage;
			else	if( PressedImage!=null)	image = PressedImage;
			else	if( DisabledImage!=null)	image = DisabledImage;

			if( image == null)	return;

			if( element.haskeycolor)
			{
				GraphicsPath path = null;//SkinBaseElement.GetPath(new Rectangle(Point.Empty, this.Size), (Bitmap)ActiveImage, element.keycolor);
				if( WestWidth != 0)
				{
					GraphicsPath west_path = SkinBaseElement.GetPath(
						new Rectangle(Point.Empty, new Size(WestWidth, this.Height)), 
						(Bitmap)ActiveImage, 
						new Rectangle(Point.Empty, new Size(WestWidth, image.Height)), 
						element.keycolor);

					if( west_path!=null)
					{
						path = west_path;
					}
				}
				if( EastWidth != 0)
				{
					GraphicsPath east_path = SkinBaseElement.GetPath(
						new Rectangle(new Point(this.Width-EastWidth, 0), new Size(EastWidth, this.Height)), 
						(Bitmap)ActiveImage, 
						new Rectangle(new Point(image.Width-EastWidth, 0), new Size(EastWidth, image.Height)), 
						element.keycolor);
					if( east_path!=null)
					{
						if( path == null)
							path = east_path;
						else
							path.AddPath( east_path, true);
					}
				}
				GraphicsPath center_path = SkinBaseElement.GetPath(
					new Rectangle(new Point(WestWidth, 0), new Size(this.Width - EastWidth - WestWidth, this.Height)), 
					(Bitmap)ActiveImage, 
					new Rectangle(new Point(WestWidth, 0), new Size(image.Width - EastWidth - WestWidth, image.Height)), 
					element.keycolor);
				if( path == null)
					path = center_path;
				else
					path.AddPath( center_path, true);

				if( path!=null)
				{
					this.Region = new Region(path);
					path.Dispose();
				}
			}
		}

		#region	Skinning
		public bool UpdateSkin()
		{
			if(skinController==null)	return false;
			skinController.SkinFile = SkinSettings.ControlsSkinFilename;
			SkinScrollers();
			return true;
		}
		#endregion	//Skinning

		private void SkinnedResizableComboBox_VisibleChanged(object sender, EventArgs e)
		{
			UpdateSkin();
		}

		public new void UpdateBounds()
		{
			this.Left+=1;
			this.Left-=1;
		}
    }
    #endregion
}