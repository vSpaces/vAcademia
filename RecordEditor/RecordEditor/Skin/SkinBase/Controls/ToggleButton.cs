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
	#region Flat Toggle Botton

	/// <summary>
	/// Flat button with custom styles.
	/// </summary>
	public class ToggleButton : Label, ISkinnedControl
	{
		/// <summary>
		/// Occures when property Pressed is changed.
		/// </summary>
		public event EventHandler PressedChanged;

		private FlatToggleButtonStyles style;
		/// <summary>
		/// Button style: push botton or toggle button.
		/// </summary>
		[CategoryAttribute("Behavior")]
		public FlatToggleButtonStyles Style
		{
			get
			{
				return style;
			}
			set
			{
				style = value;
			}
		}

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
					if (PressedChanged != null && Style == FlatToggleButtonStyles.Toggle)
						PressedChanged(this, new EventArgs());
					OnPressedChanged(new EventArgs());
				}
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
		}

		protected override void OnPaint(PaintEventArgs e)
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
				if( Pressed && Style == FlatToggleButtonStyles.Toggle)
					image = PressedImage;
			}
			if (image != null)
			{
				DrawButtonImage(e.Graphics, image);
			}

			if (this.Text != null && this.Text.Length > 0)
			{
				StringFormat sf = new StringFormat();
				sf.Alignment = StringAlignment.Near;
				sf.LineAlignment = StringAlignment.Center;
				SolidBrush brush = new SolidBrush(Color.Black); 
				e.Graphics.DrawString(this.Text, this.Font, brush, new Rectangle(0, 0, Width, this.Height), sf);
			}
		}

		protected virtual void DrawButtonImage(Graphics graphics, Image image)
		{
			graphics.DrawImageUnscaled(image, 0, 0);
		}

		protected override void OnClick(EventArgs e)
		{
			if (Style != FlatToggleButtonStyles.Normal)
			{
				base.OnClick(e);
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			if( !mouseDown)	return;
			if (Style == FlatToggleButtonStyles.Normal)
				Pressed = false;
			if( Active && !Capture)
				base.OnClick(e);
			base.OnMouseUp(e);
			mouseDown = false;
		}

		protected bool	mouseDown = false;
		protected override void OnMouseDown(MouseEventArgs e)
		{
			mouseDown = true;
			if (e.Button != MouseButtons.Left)
			{
				base.OnMouseDown (e);
				return;
			}
			if (Style == FlatToggleButtonStyles.Normal)
			{
				Pressed = true;
				//Thread.Sleep(100);
				//Pressed = false;
			}
			else if (Style == FlatToggleButtonStyles.Toggle)
			{
				Pressed = !Pressed;
				this.Invalidate();
				this.Update();
			}
			this.Capture = false;
			base.OnMouseDown (e);
		}


		protected override void OnMouseEnter(EventArgs e)
		{
			if (Style == FlatToggleButtonStyles.Normal)
			{
				if( !(Control.MouseButtons == MouseButtons.Left))
					Pressed = false;
			}
			Active = true;
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			Active = false;
		}
		#region ISkinnedControl Members

		public void SetSkinElement(SkinBaseElement element)
		{
			// TODO:  Add ToggleButton.SetSkinElement implementation
			if( element==null || element.Image==null)	return;
			Bitmap	image = element.Image;

			try
			{
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
				CreateRegion(element);
			}
			catch(Exception)
			{
				
			}
		}

		protected virtual void CreateRegion(SkinBaseElement element)
		{
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

        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // ToggleButton
            // 
            this.Name = "ToggleButton";
            this.Size = new System.Drawing.Size(323, 256);
            this.ResumeLayout(false);

        }
	}
	#endregion
}