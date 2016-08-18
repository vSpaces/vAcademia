using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;

namespace FlashGenie
{
	/// <summary>
	/// Base class for all skin elements
	/// </summary>
	public class SkinBaseElement
	{
		// Event for src changing
		public	BitmapChangedEventHandler	BitmapChanged = null;
		// Image portion of the bitmap
		protected	Bitmap	bitmapPortion = null;

		public SkinBaseElement()
		{
			// 
			// TODO: Add constructor logic here
			//
		}

		/// <summary>
		/// Image, that this control contains
		/// </summary>
		public	Bitmap	Image
		{
			get
			{
				return bitmapPortion;
			}
		}

		/// <summary>
		/// Element gorizontal alignment
		/// </summary>
		private AlignEnumeration mAlign=AlignEnumeration.Left;
		public AlignEnumeration align
		{
			get
			{
				return mAlign;
			}
			set
			{
				mAlign = value;
				OnAlignChanged();
			}
		}

        /// <summary>
        /// Element gorizontal alignment
        /// </summary>
        private bool mBitmapTile = false;
        public bool bitmapTile
        {
            get
            {
                return mBitmapTile;
            }
            set
            {
                mBitmapTile = value;
            }
        }

		/// <summary>
		/// Element vertical alignment
		/// </summary>
		private VAlignEnumeration mVAlign=VAlignEnumeration.Top;
		public VAlignEnumeration valign
		{
			get
			{
				return mVAlign;
			}
			set
			{
				mVAlign = value;
				OnAlignChanged();
			}
		}

		/// <summary>
		/// Name of used bitmap
		/// </summary>
		private string mBitmap="";
		public string bitmap
		{
			get
			{
				return mBitmap;
			}
			set
			{
				mBitmap = value;
				OnBitmapChanged();
			}
		}

		/// <summary>
		/// Texture used to drawing
		/// </summary>
		protected SkinBitmap mBitmapSkin = null;
		protected SkinBitmap BitmapSkin
		{
			set
			{
				mBitmapSkin = value;
				OnBitmapSkinChanged();
			}
		}

		/// <summary>
		/// Color used if no bitmap selected
		/// </summary>
		private Color mColor = Color.Black;
		public Color color
		{
			get
			{
				return mColor;
			}
			set
			{
				mColor = value;
				OnColorChanged();
			}
		}

		/// <summary>
		/// Color used if no bitmap selected
		/// </summary>
		private Color mKeyColor = Color.Black;
		public Color keycolor
		{
			get
			{
				if(bKeyColor)
					return mKeyColor;
				if( mBitmapSkin != null)
					return	mBitmapSkin.keycolor;
				return Color.Brown;
			}
			set
			{
				mKeyColor = value;
				OnKeyColorChanged();
			}
		}

		/// <summary>
		/// Determine whether an element has key color
		/// </summary>
		private bool bKeyColor = false;
		public bool haskeycolor
		{
			get
			{
				return bKeyColor || mBitmapSkin.haskeycolor;
			}
			set
			{
				bKeyColor = value;
			}
		}

		/// <summary>
		/// Source rectangle on bitmap
		/// </summary>
		private Rectangle mSrcRect = Rectangle.Empty;
		public Rectangle srcRect
		{
			get
			{
				return	mSrcRect;
			}
			set
			{
				mSrcRect = value;
				OnSrcRectChanged();
			}
		}

		/// <summary>
		/// Destination rectangle on form
		/// </summary>
		private Rectangle mDestRect = Rectangle.Empty;
		public Rectangle destRect
		{
			set
			{
				mDestRect = value;
				//OnDestRectChanged();
			}
			get
			{
				return mDestRect;
			}
		}

		#region	Event raisers

		/// <summary>
		/// Occures when user change bitmap image
		/// </summary>
		protected	void	OnBitmapChanged()
		{
			if( BitmapChanged != null)
			{
				BitmapChangedArgs	args = new BitmapChangedArgs(bitmap);
				BitmapChanged(this, args);
				BitmapSkin = args.BitmapSkin;
			}
		}

		/// <summary>
		/// Occures when user change fill color
		/// </summary>
		protected	void	OnColorChanged()
		{
		}

		/// <summary>
		/// Occures when user change key color
		/// </summary>
		protected	void	OnKeyColorChanged()
		{
			bKeyColor = true;
		}

		/// <summary>
		/// Occures when user change rectangle color
		/// </summary>
		protected	void	OnSrcRectChanged()
		{
		}

		/// <summary>
		/// Occures when user change tiling about any axe
		/// </summary>
		protected	void	OnTileChanged()
		{
		}

		/// <summary>
		/// Occures when user change alignment of the button
		/// </summary>
		virtual protected	void	OnAlignChanged()
		{
		}

		protected	virtual	void	OnBitmapSkinChanged()
		{
			if( bitmapPortion != null)	bitmapPortion.Dispose();
			bitmapPortion = null;
			if( mBitmapSkin==null || mBitmapSkin.Image==null)	return;
			if( srcRect.Size.IsEmpty)	return;

			bitmapPortion = new Bitmap(srcRect.Size.Width, srcRect.Size.Height);
			Graphics	imageGraphics = Graphics.FromImage(bitmapPortion);
			imageGraphics.DrawImage(mBitmapSkin.Image, new Rectangle(Point.Empty, srcRect.Size),
				srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, 
				GraphicsUnit.Pixel);
		}

		#endregion

		/// <summary>
		/// Paints skin element
		/// </summary>
		public void Paint(Graphics g)
		{
			if( bitmapPortion != null)
			{
				if( align==AlignEnumeration.Justify || valign==VAlignEnumeration.Justify )
				{
					Rectangle filledRect = mDestRect;
					if( align!=AlignEnumeration.Justify)	filledRect.Width = srcRect.Width;
					if( valign!=VAlignEnumeration.Justify)	filledRect.Height = srcRect.Height;
					TextureBrush	brush = new TextureBrush(bitmapPortion);
					brush.WrapMode = WrapMode.Tile;
					Matrix	m = new Matrix();
                    if (!bitmapTile)
                    {
                        if (bitmapPortion.Width != 0 && bitmapPortion.Height != 0)
                            m.Scale((float)filledRect.Width / bitmapPortion.Width, (float)filledRect.Height / bitmapPortion.Height);
                    }
					m.Translate(filledRect.X, filledRect.Y, MatrixOrder.Append);
					brush.Transform = m;
					g.FillRectangle( brush, filledRect);
					brush.Dispose();
				}
				else
					g.DrawImage(bitmapPortion, mDestRect.X, mDestRect.Y);
			}
			else
			{
				Rectangle filledRect = mDestRect;
				if( align!=AlignEnumeration.Justify)	filledRect.Width = srcRect.Width;
				if( valign!=VAlignEnumeration.Justify)	filledRect.Height = srcRect.Height;
				SolidBrush brush = new SolidBrush(color);
				g.FillRectangle( brush, filledRect);
				brush.Dispose();
			}
		}

		public	Color	GetPixelColor(int x, int y)
		{
			if( bitmapPortion != null)
			{
				int	lx = x - destRect.Left;
				int	ly = y - destRect.Top;
				lx %= bitmapPortion.Width;
				ly %= bitmapPortion.Height;
				return bitmapPortion.GetPixel(lx,ly);
			}
			return color;
		}

		/// <summary>
		/// Return path for this element
		/// </summary>
        Rectangle       bitmapRectangle;
		GraphicsPath	pBitmapPath = null;
		Point			PathPoint = Point.Empty;
		public GraphicsPath GetPath()
		{
			if( !bKeyColor)
			{
				if( destRect.Width>0 && destRect.Height>0)
				{
					GraphicsPath path = new GraphicsPath();
					path.AddRectangle(destRect);
					return path;
				}
			}
            if (bitmapRectangle != destRect)
            {
                if (pBitmapPath != null)
                    pBitmapPath.Dispose();
                pBitmapPath = null;
            }

			if( pBitmapPath == null)
			{
                bitmapRectangle = destRect;
				pBitmapPath = SkinBaseElement.GetPath(destRect, bitmapPortion, keycolor);
			}
			else
			{
				Matrix	matrix = new Matrix();
                matrix.Translate(destRect.X - PathPoint.X, destRect.Y - PathPoint.Y);
				pBitmapPath.Transform(matrix);
			}
			PathPoint = destRect.Location;
			return pBitmapPath;
		}

		/// <summary>
		/// Return path for this element
		/// </summary>
		public GraphicsPath GetPath(Color transparencyColor)
		{
			return SkinBaseElement.GetPath(destRect, bitmapPortion, transparencyColor);
		}

		/// <summary>
		/// Return path for this element
		/// </summary>
		static public GraphicsPath GetPath(Rectangle aDestRect, Bitmap aBitmapPortion, Color aTransparencyColor)
		{
			Rectangle	bitmapRect = new Rectangle();
			if( aBitmapPortion!=null)
				bitmapRect = new Rectangle( Point.Empty, aBitmapPortion.Size);
			return GetPath(aDestRect, aBitmapPortion, bitmapRect, aTransparencyColor);
		}

		/// <summary>
		/// Return path for this element
		/// </summary>
		static public GraphicsPath GetPath(Rectangle aDestRect, Bitmap aBitmapPortion, Rectangle aBitmapBounds, Color aTransparencyColor)
		{
			if( aDestRect.Width<=0 || aDestRect.Height<=0)	return null;

			GraphicsPath	path = new GraphicsPath();
			if( aBitmapPortion==null)
			{
				path.AddRectangle(aDestRect);
			}
			else
			{
				bool	exc1found=false;
				int width = aDestRect.Width;
				int height = aDestRect.Height;
				int bmwidth = aBitmapBounds.Width;
				int bmheight = aBitmapBounds.Height;
				for (int j=0; j<height; j++ )
					for (int i=0; i<width; i++)
					{
						int bmj = j;
						int bmi = i;

						while( bmj >= bmheight)	bmj -= bmheight;
						while( bmi >= bmwidth)	bmi -= bmwidth;
						if (aBitmapPortion.GetPixel(bmi+aBitmapBounds.Left, bmj+aBitmapBounds.Top) == aTransparencyColor)
							continue;

						int x0 = bmi;

						while ((i < width) && (aBitmapPortion.GetPixel(bmi+aBitmapBounds.Left, bmj+aBitmapBounds.Top) != aTransparencyColor))
						{
							i++;
							bmi = i;
							while( bmi >= bmwidth)	bmi -= bmwidth;
						}

						if(i>x0)
						{
							path.AddRectangle(new Rectangle(aDestRect.Left+x0, aDestRect.Top+j, i-x0, 1));
							exc1found = true;
						}
					}
				if( !exc1found)
				{
					path.Dispose();
					return null;
				}
			}
			return path;
		}

		/// <summary>
		/// Internal skin
		/// </summary>
		protected FormSkin mSkin = null;
		public FormSkin Skin
		{
			get
			{
				return mSkin;
			}
			set
			{
				mSkin = value;
			}
		}
	}

	#region	Interfaces

	/// <summary>
	/// Interface for button elements, sush as "close", "miximize", "maximize" etc. window elements
	/// </summary>
	public interface	IButton
	{
		// When mouse in moved over button
		void	MouseMove(int mx, int my);
		// When mouse in moved out of the button
		void	MouseOut();
		// When mouse button is down
		void	MouseDown(int mx, int my);
		// When mouse button is up on button
		void	MouseReleased();
	}

	/// <summary>
	/// Interface for elements, such as window title
	/// </summary>
	public interface	IDragArea
	{
		// When mouse button is down
		void	MouseDown();
		// When mouse button is moved over the area
		void	MouseMove();
	}
	#endregion

	#region Event and delegates
	/// <summary>
	/// Event args to get image by bitmap name
	/// </summary>
	public class BitmapChangedArgs : EventArgs
	{
		private string name;
		private SkinBitmap skin;

		public string Name
		{
			get
			{
				return name; 
			}
		}

		public SkinBitmap BitmapSkin
		{
			get
			{
				return skin; 
			}
			set
			{
				skin = value;
			}
		}

		public BitmapChangedArgs(string aName)
		{
			this.name = aName;
		}
	}

	public delegate void BitmapChangedEventHandler(object sender, BitmapChangedArgs e);
	#endregion

	/// <summary>
	/// Base class for all skin controls elements
	/// </summary>
	public class SkinBaseControl : SkinBaseElement
	{
		public SkinBaseControl()
		{
		}

		/// <summary>
		/// Name of the control
		/// </summary>
		private string mName = "";
		public string name
		{
			get
			{
				return mName;
			}
			set
			{
				mName = value;
			}
		}

		/// <summary>
		/// Name of the control
		/// </summary>
		private string mFile = "";
		public string file
		{
			get
			{
				return mFile;
			}
			set
			{
				mFile = value;
			}
		}
	}
}
