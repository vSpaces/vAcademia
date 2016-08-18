using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;

namespace FlashGenie
{
	public	enum	AlignEnumeration
	{
		Left, Right, Justify
	}

	public	enum	VAlignEnumeration
	{
		Top, Bottom, Justify
	}

	/// <summary>
	/// Contains all elements for one level
	/// </summary>
	public class SkinLevel
	{
		public SkinLevel()
		{
			// 
			// TODO: Add constructor logic here
			//
		}

		/// <summary>
		/// Level elements
		/// </summary>
		private ArrayList mElements=null;
		public ArrayList elements
		{
			get
			{
				return mElements;
			}
			set
			{
				mElements = value;
			}
		}

		/// <summary>
		/// Level vertical alignment
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
			}
		}

		/// <summary>
		/// Grap rectangle portion for this level and remove recent rectangle
		/// </summary>
		public Rectangle UpdateRegions(Rectangle pureRect)
		{
			if( elements.Count == 0)	return pureRect;
			if( elements[0] == null)	return pureRect;

			// Grab first portion
			Rectangle	levelRect = pureRect;
			SkinBaseElement	felement = (SkinBaseElement)elements[0];
			levelRect.Height = (felement.srcRect.Height<pureRect.Height && felement.srcRect!=Rectangle.Empty)?felement.srcRect.Height:pureRect.Height;

			if( valign == VAlignEnumeration.Bottom)
				levelRect.Location = new Point(levelRect.Left,pureRect.Bottom - levelRect.Height);
			else	if( valign == VAlignEnumeration.Justify)
				levelRect = pureRect;

			mBounds = levelRect;
			foreach(SkinBaseElement	element in elements)
			{
				Rectangle elementRect = levelRect;
				if( element == null)	continue;

				if( element.align == AlignEnumeration.Justify)
				{
					elementRect.Width = levelRect.Width;
					levelRect.Width = 0;
				}
				else
				{
					elementRect.Width = element.srcRect.Width;
					if( element.align == AlignEnumeration.Right)
					{
						elementRect.Location = new Point(levelRect.Right - elementRect.Width, levelRect.Top);
						levelRect.Width -= elementRect.Width;
					}
					else	if( element.align == AlignEnumeration.Left)
					{
						levelRect.Width -= element.srcRect.Width;
						levelRect.Location = new Point(levelRect.Left + element.srcRect.Width, levelRect.Top);;
					}
				}

				if( element.valign == VAlignEnumeration.Justify)
				{
					elementRect.Height = levelRect.Height;
					//levelRect.Height = 0;
				}
				else
				{
					elementRect.Height = element.srcRect.Height;
					if( element.valign == VAlignEnumeration.Bottom)
					{
						elementRect.Location = new Point( element.srcRect.Left, levelRect.Bottom - element.srcRect.Height);
						//levelRect.Height -= element.srcRect.Height;
					}
					else	if( element.valign == VAlignEnumeration.Bottom)
					{
						//levelRect.Height -= element.srcRect.Height;
						levelRect.Location = new Point( levelRect.Left, levelRect.Top+element.srcRect.Height);
					}
				}
				element.destRect = elementRect;
				if( levelRect.Width==0 || levelRect.Height==0)	break;
			}

			// Compute rest of the level
//			if( !levelRect.IsEmpty)
//			{
				if( valign == VAlignEnumeration.Top)
				{
					pureRect.Location = new Point(pureRect.Left, pureRect.Top+mBounds.Height);
					pureRect.Height -= mBounds.Height;
				}
				else	if( valign == VAlignEnumeration.Bottom)
				{
					pureRect.Height -= mBounds.Height;
				}
				else	if( valign == VAlignEnumeration.Justify)
					pureRect = Rectangle.Empty;
//			}
			return pureRect;
		}

		/// <summary>
		/// Level bounds
		/// </summary>
		private Rectangle mBounds = Rectangle.Empty;
		public Rectangle Bounds
		{
			get
			{
				return mBounds;
			}
		}

		/// <summary>
		/// Paints skin level
		/// </summary>
		public void Paint(Graphics g)
		{
			foreach(SkinBaseElement	element in elements)
				element.Paint(g);
		}

		public	Color	GetPixelColor(int x, int y)
		{
			foreach(SkinBaseElement	element in elements)
				if( element.destRect.Contains(new Point(x,y)))
					return	element.GetPixelColor(x, y);
			return Color.Black;
		}

		/// <summary>
		/// Return path for current layer and rectangle
		/// </summary>
		/// <param name="path"></param>
		/// <param name="rects"></param>
		public void GetRegionAndRects(GraphicsPath path, ArrayList	rects, Color transparencyColor)
		{
			if( path == null)		return;
			if( elements == null)	return;
			if( Bounds.Width<=0 || Bounds.Height<=0)	return;
			rects.Add(Bounds);
			foreach(SkinBaseElement	element in elements)
			{
				if( element == null) continue;
				GraphicsPath	gp = element.GetPath();
				if( gp == null) continue;
				path.AddPath(gp, true);
			}
		}
	}
}
