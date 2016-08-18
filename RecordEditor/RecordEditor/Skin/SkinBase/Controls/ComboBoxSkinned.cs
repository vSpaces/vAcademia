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
	#region Skinned combo box
	/// <summary>
	/// Flat button with custom styles.
	/// </summary>
	public class SkinnedComboBox : ComboBox, ISkinnedControl
	{
		#region ISkinnedControl Members

		public event MeasureItemEventHandler MeasureComboItem;
		public event DrawItemEventHandler	 DrawComboItem;
		public event EventHandler	 PostPaint;

		protected	Platform.COMBOBOXINFO	cbInfo = new Platform.COMBOBOXINFO();

		public	SkinnedComboBox()
		{
			this.DrawMode = DrawMode.OwnerDrawVariable;
			SkinScrollers();
			this.SelectionChangeCommitted += new EventHandler(SkinnedComboBox_SelectionChangeCommitted);
			this.Leave += new EventHandler(SkinnedComboBox_Leave);
			this.SelectedValueChanged += new EventHandler(SkinnedComboBox_SelectedValueChanged);
			this.ItemHeight = 16;
		}

		public	void	SkinScrollers()
		{
			UpdateInfo();
			SkinSettings.SkinController(cbInfo.hwndList);
		}

		public void SetSkinElement(SkinBaseElement element)
		{
			// TODO:  Add ToggleButton.SetSkinElement implementation
			if( element==null || element.Image==null)	return;
			Bitmap	image = element.Image;

			// We need : image, active image, disabled image
			int	buttonWidth = image.Width/4;
			int	buttonHeight = image.Height;
			for( int i=0; i<4; i++)
			{
				Bitmap	buttonBitmap = new Bitmap(buttonWidth, buttonHeight);
				Graphics	imageGraphics = Graphics.FromImage(buttonBitmap);
				imageGraphics.DrawImage(image, new Rectangle(0, 0, buttonWidth, buttonHeight),
					buttonWidth*i, 0, buttonWidth, buttonHeight, 
					GraphicsUnit.Pixel);
				if( i == 0)	Image = buttonBitmap;
				else	if( i == 1)	activeImage = buttonBitmap;
				else	if( i == 2)	pressedImage = buttonBitmap;
				else	if( i == 3)	disabledImage = buttonBitmap;
			}
			OnSkinElementChanged(element);
		}

		protected virtual void OnSkinElementChanged(SkinBaseElement element)
		{
			Image image = null;
			if( activeImage!=null)	image = activeImage;
			else	if( pressedImage!=null)	image = pressedImage;
			else	if( disabledImage!=null)	image = disabledImage;

			if( image != null)
			{
				Platform.MoveWindow(cbInfo.hwndCombo, Location.X, Location.Y, image.Width, image.Height, true);
				this.Size = image.Size;
			}

			if( element.haskeycolor)
			{
				GraphicsPath path = SkinBaseElement.GetPath(new Rectangle(Point.Empty, this.Size), (Bitmap)activeImage, element.keycolor);
				if( path!=null)
				{
					this.Region = new Region(path);
					path.Dispose();
				}
			}
		}

		#endregion

		#region Toggle button routine
		private bool active;
		/// <summary>
		/// Specifies there is a mouse above the button whether or not.
		/// </summary>
		[CategoryAttribute("Behavior")]
		public bool Active
		{
			get
			{
				return active;
			}
			set
			{
				active = value;
				OnActiveChanged(new EventArgs());
			}
		}

		private bool pressed;
		/// <summary>
		/// Specifies the button is pressed whether or not. 
		/// </summary>
		[CategoryAttribute("Behavior")]
		public bool Pressed
		{
			get
			{
				return pressed;
			}
			set
			{
				if( pressed != value)
				{
					pressed = value;
					OnPressedChanged(new EventArgs());
				}
			}
		}

		private Image normalImage;
		/// <summary>
		/// Image for the button in normal state.
		/// </summary>
		[CategoryAttribute("Appearance")]
		public Image Image
		{
			get
			{
				return normalImage;
			}
			set
			{
				normalImage = value;
			}
		}

		private Image activeImage;
		/// <summary>
		/// Image for the button in active state.
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

		private Image disabledImage;
		/// <summary>
		/// Image for the button in disabled state.
		/// </summary>
		[CategoryAttribute("Appearance")]
		public Image DisabledImage
		{
			get
			{
				return disabledImage;
			}
			set
			{
				disabledImage = value;
			}
		}

		private Image pressedImage;
		/// <summary>
		/// Image for the button in pressed state.
		/// </summary>
		[CategoryAttribute("Appearance")]
		public Image PressedImage
		{
			get
			{
				return pressedImage;
			}
			set
			{
				pressedImage = value;
			}
		}

		protected virtual void OnPressedChanged(EventArgs e)
		{
			this.Invalidate();
			this.Update();
		}

		protected virtual void OnActiveChanged(EventArgs e)
		{
			this.Invalidate();
			this.Update();
			this.Refresh();
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Left)
			{
				base.OnMouseDown (e);
				return;
			}
			Pressed = true;
			Thread.Sleep(100);
			Pressed = false;
			base.OnMouseDown (e);
			Active = false;
		}


		protected override void OnMouseEnter(EventArgs e)
		{
			Active = true;
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			Active = false;
		}

		protected new void Paint(Graphics graphics)
		{
			Image image = Image;
			if (!this.Enabled && DisabledImage != null)
				image = DisabledImage;
			else if (Active && ActiveImage != null)
			{
				if( Pressed)
					image = PressedImage;
				else
					image = ActiveImage;

			}
			else
			{
				if( Pressed)
					image = PressedImage;
				else
					image = Image;
			}
			if (image != null)
				DrawButtonImage(graphics, image);
			if(PostPaint != null)
				PostPaint(graphics, new EventArgs());
		}

		protected virtual void DrawButtonImage(Graphics graphics, Image image)
		{
			graphics.DrawImageUnscaled(image, 0, 0);
		}
		#endregion //Toggle button routine

		protected override void WndProc(ref Message m)
		{
			base.WndProc (ref m);

			if ((m.Msg == Platform.WM_PAINT || m.Msg == Platform.WM_ENABLE))
			{
				try
				{
					Graphics g = CreateGraphics();
					Paint(g);
					g.Dispose();
					if( this.DropDownStyle == ComboBoxStyle.DropDownList)
					{
						//Platform.SendMessage((IntPtr)cbInfo.hwndItem, Platform.WM_PAINT, false, 0);
						//;
					}
				}
				catch
				{
					//	Trace.WriteLine("Problem drawing dropdown arrow");
				}
			}
		}

		protected override void OnMeasureItem( MeasureItemEventArgs e)
		{
			if(MeasureComboItem!=null)
			{
				MeasureComboItem(this, e);
				return;
			}
			if( e.Index<0 || e.Index>=this.Items.Count)	return;
			string caption = (string) this.Items[e.Index];
			StringFormat sf = new StringFormat();
			sf.HotkeyPrefix = HotkeyPrefix.Show;
			Size textSize =  e.Graphics.MeasureString(caption, SystemInformation.MenuFont, new PointF(0, 0), sf).ToSize();
			int horizOffset = 19;
			e.ItemWidth = textSize.Width + horizOffset;			
			if (caption != "-")
				e.ItemHeight = 16;//SystemInformation.MenuHeight;
			else 
				e.ItemHeight = 4;
		}

		protected override void OnDrawItem(	DrawItemEventArgs e)
		{
			if(DrawComboItem!=null)
			{
				DrawComboItem(this, e);
				return;
			}
			Rectangle	eBounds = e.Bounds;
			if( (e.State & DrawItemState.ComboBoxEdit) == DrawItemState.ComboBoxEdit)
			{
				UpdateInfo();
				eBounds = new Rectangle(6, 3, this.Width-30, 16);
			}

			if( e.Index == -1)	return;
			e.DrawBackground();
			//if( this.SelectedIndex==e.Index)
			if( (e.State & DrawItemState.Selected) == DrawItemState.Selected)
			{
				SolidBrush b = new SolidBrush(Color.FromArgb(102,144,219));
				e.Graphics.FillRectangle(b, eBounds);
				b.Dispose();
			}
			
			//e.DrawFocusRectangle(); 
			Color	textColor = Color.Black;
			if( (e.State&DrawItemState.Selected) > 0)
				textColor = Color.White;
			Rectangle	textBounds = eBounds;
			string	text = this.Items[e.Index].ToString();
			textBounds.Location = new Point(textBounds.Location.X+5, textBounds.Location.Y+3);
			e.Graphics.DrawString(text,new Font(FontFamily.GenericSansSerif, 8, FontStyle.Regular),new SolidBrush(textColor),textBounds); 
		}

		protected virtual void UpdateEditItemPos(IntPtr hwndItem)
		{
			Platform.MoveWindow(hwndItem, 6, 3, this.Width-30, 16, true);
		}

		protected override void OnLocationChanged(EventArgs e)
		{
			UpdateEditItemPos(cbInfo.hwndItem);
		}

		protected override void OnSizeChanged(EventArgs e)
		{
			UpdateEditItemPos(cbInfo.hwndItem);
			UpdateInfo();
		}

		protected	void UpdateInfo()
		{
			cbInfo.cbSize = Marshal.SizeOf(typeof(Platform.COMBOBOXINFO));
			Platform.SendMessage( this.Handle, Platform.CB_GETCOMBOBOXINFO, 0, ref cbInfo);
		}

		protected override void OnVisibleChanged( EventArgs e)
		{
			UpdateEditItemPos(cbInfo.hwndItem);
		}

		private void SkinnedComboBox_SelectionChangeCommitted(object sender, EventArgs e)
		{
			
		}

		private void SkinnedComboBox_Leave(object sender, EventArgs e)
		{
			Active = false;
		}

		private void SkinnedComboBox_DrawItem(object sender, DrawItemEventArgs e)
		{
			e.Graphics.FillRectangle(Brushes.Yellow, e.Bounds);
		}

		private void SkinnedComboBox_SelectedValueChanged(object sender, EventArgs e)
		{
			Left++; Left--;
			OnSizeChanged(e);
		}
	}

	#endregion
}