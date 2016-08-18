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
	#region Menu item classes
	/// <summary>
	/// The menu item which can be placed on the panel.
	/// </summary>
	class PopupMenuItem : MenuItem
	{
		public PopupMenuItem()
		{
			OwnerDraw = true;
			this.MeasureItem +=new MeasureItemEventHandler(PopupMenuItem_MeasureItem);
			this.DrawItem +=new DrawItemEventHandler(PopupMenuItem_DrawItem);
		}

		private Color backColor = Color.FromArgb(68,89,155);
		public Color BackColor
		{
			get
			{
				return backColor;
			}
			set
			{
				if(backColor != value)
				{
					backColor = value;
				}
			}
		}

		public static void PopupMenu_MeasureItem(object sender, System.Windows.Forms.MeasureItemEventArgs e)
		{
			MenuItem item = (MenuItem) sender;
			string caption = item.Text;
			StringFormat sf = new StringFormat();
			sf.HotkeyPrefix = HotkeyPrefix.Show;
			Size textSize =  e.Graphics.MeasureString(caption, SystemInformation.MenuFont, new PointF(0, 0), sf).ToSize();
			int horizOffset = 16;
			e.ItemWidth = textSize.Width + horizOffset;			
			if (item.Text != "-")
				e.ItemHeight = SystemInformation.MenuHeight;
			else 
				e.ItemHeight = 4;
		}

		static void	DrawMenyGlypth(Graphics g, Bitmap bm, Rectangle source, Rectangle dest, bool sel)
		{
			if( !sel)
			{
				g.DrawImageUnscaled(bm, dest.Left, dest.Top);
			}
			else
			{
				ImageAttributes imageAttr = new ImageAttributes();
				ColorMap colorMap1 = new ColorMap();
				colorMap1.NewColor = Color.White;
				colorMap1.OldColor = Color.Black;
				imageAttr.SetRemapTable( new ColorMap[]{colorMap1});
				g.DrawImage(bm,         // Image
					dest,
					source.Left, source.Top, source.Width, source.Height,
					GraphicsUnit.Pixel, // srcUnit
					imageAttr);      // ImageAttributes
			}
		}

		public static void PopupMenu_DrawItem(object sender, System.Windows.Forms.DrawItemEventArgs e, bool withArrows)
		{
			PopupMenuItem item = (PopupMenuItem) sender;
			string caption = item.Text; 

			//Paint background
			Color backColor = item.BackColor;
			SolidBrush brush = new SolidBrush(Settings.MenuItemBackColor);
			e.Graphics.FillRectangle(brush, e.Bounds.Left, e.Bounds.Top, e.Bounds.Width, e.Bounds.Height);

			
			if ((e.State & DrawItemState.Selected) == DrawItemState.Selected)
			{
				brush.Color = Settings.MenuItemSelectedBackColor; 
				e.Graphics.FillRectangle(brush, e.Bounds.Left + 1, e.Bounds.Top + 3, e.Bounds.Width - 1, e.Bounds.Height - 6);
			}

			//Draw text
			if (item.Text != "-")
			{
				int horizOffset = 16;
				if(item.Enabled)
				{
					if ((e.State & DrawItemState.Selected) == DrawItemState.Selected)
						brush.Color = Settings.MenuItemSelectedForeColor;
					else
						brush.Color = Settings.MenuItemForeColor; 
				}
				else
					brush.Color = Settings.MenuItemDisabledForeColor; 

				StringFormat sf = new StringFormat();
				sf.HotkeyPrefix = HotkeyPrefix.Hide;
				sf.Alignment = StringAlignment.Near;
				sf.LineAlignment = StringAlignment.Center;
				Rectangle textRect = new Rectangle(
					e.Bounds.Left + horizOffset, e.Bounds.Top, 
					e.Bounds.Width - horizOffset, e.Bounds.Height);
				e.Graphics.DrawString(caption, e.Font, brush, textRect, sf);

				if( item.Image != null)
				{
					//e.Graphics.DrawImageUnscaled(item.Image, e.Bounds.Location.X, e.Bounds.Location.Y);
					e.Graphics.DrawImage(item.Image, e.Bounds.Location.X, e.Bounds.Location.Y+1, item.Image.Width, item.Image.Height);
					//item.Image.Save("c:\\1.bmp", System.Drawing.Imaging.ImageFormat.Bmp);
				}
			}
			else 
			{
				//Draw separator line
				Pen pen = new Pen(Color.White, 1);
				Point src = new Point(e.Bounds.Left, e.Bounds.Top + 1);
				Point dst = new Point(e.Bounds.Left + e.Bounds.Width, e.Bounds.Top + 1);
				e.Graphics.DrawLine(pen, src, dst);
				pen.Dispose();
			}
			//Draw check mark
			if ((e.State & DrawItemState.Checked) == DrawItemState.Checked)
			{
				Size	arrSize = new Size(e.Bounds.Height-4, e.Bounds.Height-4);
				Bitmap bitmap = new Bitmap(arrSize.Height, arrSize.Height);
				Graphics g = Graphics.FromImage(bitmap);
				g.Clear(Color.Transparent);
				ControlPaint.DrawMenuGlyph(g, new Rectangle(0, 1, arrSize.Width, arrSize.Height), MenuGlyph.Checkmark);
				bitmap.MakeTransparent();
				DrawMenyGlypth(e.Graphics, bitmap, new Rectangle(0,0,arrSize.Width, arrSize.Height), new Rectangle(e.Bounds.Left + 2,               // srcX
					e.Bounds.Top+1,               // srcY
					arrSize.Width, 
					arrSize.Height), (e.State & DrawItemState.Selected) == DrawItemState.Selected);
			}
			//Draw arrow
			if (item.IsParent && withArrows)
			{
				Size	arrSize = new Size(e.Bounds.Height-2, e.Bounds.Height-2);
				Bitmap bitmap = new Bitmap(arrSize.Height, arrSize.Height);
				Graphics g = Graphics.FromImage(bitmap);
				g.Clear(Color.Transparent);
				ControlPaint.DrawMenuGlyph(g, new Rectangle(0, 1, arrSize.Width, arrSize.Height), MenuGlyph.Arrow);
				bitmap.MakeTransparent();
				DrawMenyGlypth(e.Graphics, bitmap, new Rectangle(0,0,arrSize.Width, arrSize.Height), new Rectangle(e.Bounds.Left + (e.Bounds.Width - e.Bounds.Height),               // srcX
					e.Bounds.Top+1,               // srcY
					arrSize.Width, 
					arrSize.Height), (e.State & DrawItemState.Selected) == DrawItemState.Selected);
			}

			brush.Dispose();
		}

		public void ProcessClick(EventArgs e)
		{
			OnClick(e);
		}

		private void PopupMenuItem_MeasureItem(object sender, MeasureItemEventArgs e)
		{
			MenuItem item = (MenuItem) this;
			string caption = item.Text;
			StringFormat sf = new StringFormat();
			sf.HotkeyPrefix = HotkeyPrefix.Show;
			Size textSize =  e.Graphics.MeasureString(caption, SystemInformation.MenuFont, new PointF(0, 0), sf).ToSize();
			int horizOffset = 19;
			e.ItemWidth = textSize.Width + horizOffset;			
			if (item.Text != "-")
				e.ItemHeight = SystemInformation.MenuHeight;
			else 
				e.ItemHeight = 4;
		}

		private void PopupMenuItem_DrawItem(object sender, DrawItemEventArgs e)
		{
			PopupMenu_DrawItem( sender, e, false);
		}

		/// <summary>
		/// Image to be drawn
		/// </summary>
		public	Image	image	= null;
		public	Image	Image
		{
			get
			{
				return image;
			}
			set
			{
				image=value;
			}
		}

		
	}

	/// <summary>
	/// The menu item which can be placed on the panel.
	/// </summary>
	class TopLevelMenuItem : Label
	{
		public event EventHandler FocusChanged;
		[Category("Appearance")]
		public event EventHandler OnPopup;

		private bool focus = false;
		public new bool Focus 
		{
			get
			{
				return focus;
			}
			set
			{
				if (focus != value)
				{
					focus = value;
					OnFocusChanged(new EventArgs());
					if (FocusChanged != null)
						FocusChanged(this, new EventArgs());
				}
			}
		}

		private bool selected = false;
		public bool Selected 
		{
			get
			{
				return selected;
			}
			set
			{
				if (selected != value)
				{
					selected = value;
					OnSelectedChanged(new EventArgs());
				}
			}
		}

		private FlatPopupMenu popupMenu;
		public FlatPopupMenu PopupMenu
		{
			get
			{
				return popupMenu;
			}
			set
			{
				popupMenu = value;
				if (popupMenu != null)
					popupMenu.OwnerMenu = this;
			}
		}

        private bool bDrawShadow=false;
        public bool drawShadow
        {
            get
            {
                return bDrawShadow;
            }
            set
            {
                bDrawShadow = value;
            }
        }

		protected virtual void OnFocusChanged(EventArgs e)
		{
			this.Invalidate();
			this.Update();
		}

		protected virtual void OnSelectedChanged(EventArgs e)
		{
			this.Invalidate();
			this.Update();
			if (PopupMenu != null)
			{
				PopupMenu.Left = this.Parent.Left + this.Left;
				PopupMenu.Top = this.Parent.Top + this.Top + this.Height;
				PopupMenu.Visible = Selected;
				PopupMenu.BringToFront();
				if( OnPopup != null)
					OnPopup(PopupMenu, new EventArgs());
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			//paint background
			SolidBrush brush = new SolidBrush(this.BackColor);
			e.Graphics.FillRectangle(brush, 0, 0, this.Width, this.Height);
			//draw text
			brush.Color = Color.White;
			if( !this.Enabled)
				brush.Color = Color.FromArgb(131,159,206);
			StringFormat sf = new StringFormat();
			//sf.HotkeyPrefix = HotkeyPrefix.Hide;
			sf.Alignment = StringAlignment.Near;
			sf.LineAlignment = StringAlignment.Center;
			int horizOffset = 6;
			Rectangle textRect = new Rectangle(
				horizOffset, 0, 
				this.Width - horizOffset, this.Height);

			Font	menuFont = new Font("Tahoma", 11, GraphicsUnit.World);
            if( drawShadow)
            {
                Rectangle   shadowRect = new Rectangle(textRect.X, textRect.Y+1, textRect.Width, textRect.Height);
                e.Graphics.DrawString(this.Text, menuFont/*SystemInformation.MenuFont*/, Brushes.Black, shadowRect, sf);
            }
			e.Graphics.DrawString(this.Text, menuFont/*SystemInformation.MenuFont*/, brush, textRect, sf);
			brush.Dispose();
			//Draw selection rectangle			
			if (Focus || Selected)
			{
				Pen pen = new Pen(Color.FromArgb(10, 36, 106), 1);
				e.Graphics.DrawRectangle(pen, 0, 0, this.Width - 2, this.Height - 2);
				pen.Dispose();
			}
		}

		protected override void OnMouseEnter(EventArgs e)
		{
			Focus = true;
		}
		
		protected override void OnMouseLeave(EventArgs e)
		{
			Focus = false;
			//			if( PopupMenu != null && PopupMenu.SelectedIndex==-1)
			//				Selected = false;
		}

		protected override void OnClick(EventArgs e)
		{
			base.OnClick(e);
			Selected = !Selected;
		}

	}

	/// <summary>
	/// MenuItem with images (normal and selected) 
	/// </summary>
	class ImageMenuItem : MenuItem
	{
		private Image activeImage;
		/// <summary>
		/// Image, which is drawn when the menu item is selected
		/// </summary>
		[CategoryAttribute("Appearance")]
		public Image ActiveImage
		{
			get
			{
				return activeImage;
			}
			set
			{
				activeImage = value;
			}
		}

		/// <summary>
		/// Active image for the toolbar button, which has shown the menu
		/// </summary>
		private Image activeImage_tb;
		[CategoryAttribute("Appearance")]
		public Image ActiveImage_ToolBar
		{
			get
			{
				return activeImage_tb;
			}
			set
			{
				activeImage_tb = value;
			}
		}

		/// <summary>
		/// Image, which is drawn when the menu item is not selected
		/// </summary>
		private Image image;
		[CategoryAttribute("Appearance")]
		public Image Image
		{
			get
			{
				return image;
			}
			set
			{
				image = value;
			}
		}

		/// <summary>
		/// Image for the toolbar button, which has shown the menu
		/// </summary>
		private Image image_tb;
		[CategoryAttribute("Appearance")]
		public Image Image_ToolBar
		{
			get
			{
				return image_tb;
			}
			set
			{
				image_tb = value;
			}
		}

	}
	#endregion
}