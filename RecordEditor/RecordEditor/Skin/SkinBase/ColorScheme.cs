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
    #region Color schemes classes

    public class ColorScheme
	{
		private Color[]		mColors;

		public ColorScheme()
		{
		}

		public Color[] Colors 
		{
			get
			{
				return mColors;
			}
			set
			{
				mColors = value;
			}
		}

		public bool IsEqualTo(Color[] aColors)
		{
			if( aColors.GetLength(0) != mColors.GetLength(0))
			{
				return false;
			}
			for(int i=0; i<aColors.GetLength(0); i++)
			{
				if( aColors[i] != mColors[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	/// <summary>
	/// Class that contains data about color schemes
	/// </summary>
	public class ColorSchemeContainer
	{
		private ArrayList	mSchemes = null;

		public ColorSchemeContainer()
		{
			mSchemes = new ArrayList();
		}

		public ColorScheme AddScheme(Color[] aColors)
		{
			ColorScheme	newScheme = new ColorScheme();
			newScheme.Colors = aColors;
			mSchemes.Add(newScheme);
			return newScheme;
		}

		public ColorScheme GetSceme(int id)
		{
			if( id<0 || id>=mSchemes.Count)
				return null;
			return (ColorScheme)mSchemes[id];
		}

		public int Count
		{
			get
			{
				return mSchemes.Count;
			}
		}
	}

	public class ColorSchemeComboBox : SkinnedResizableComboBox
	{
		ColorSchemeContainer	schemes = new ColorSchemeContainer();

		// Constants
		private	const int itemWidth = 10;
		private	const int itemHeight = 20;
		private	const int border = 2;
		private	const int margins = 2;
		private	Color borderColor = Color.FromArgb(162,174,200);
		private	Color selectedBorderColor = Color.FromArgb(43,73,226);
		private	System.Windows.Forms.Timer	timer = new System.Windows.Forms.Timer();

		public event EventHandler	SelectedSchemeChanged = null;

		public	ColorSchemeComboBox()
		{
			this.DrawMode = DrawMode.OwnerDrawVariable;
			this.BackColor = Color.Blue;
			this.DropDownStyle = ComboBoxStyle.DropDownList;
	
			// Event handlers
			this.MeasureComboItem += new MeasureItemEventHandler(ColorSchemeComboBox_MeasureItem);
			this.DrawComboItem += new DrawItemEventHandler(ColorSchemeComboBox_DrawItem);
			this.SelectedIndexChanged += new EventHandler(ColorSchemeComboBox_SelectedIndexChanged);
			this.PostPaint += new EventHandler(ColorSchemeComboBox_PostPaint);

			timer.Interval = 100;
			timer.Tick += new EventHandler(timer_Tick);

			SetStyle(ControlStyles.AllPaintingInWmPaint, true); 
			SetStyle(ControlStyles.DoubleBuffer, true); 
			SetStyle(ControlStyles.UserPaint, true); 
			SetStyle(ControlStyles.ResizeRedraw, false); 
		}

		public ColorScheme	GetScheme(Color[] aColors)
		{
			for( int i=0; i<schemes.Count; i++)
				if( schemes.GetSceme(i)!=null && schemes.GetSceme(i).IsEqualTo(aColors))
					return schemes.GetSceme(i);
			return null;
		}

		public void SelectScheme(Color[] aColors)
		{
			ColorScheme	scheme = GetScheme(aColors);
			if( scheme != null)	SelectScheme(scheme);
		}

		public void SelectScheme(ColorScheme scheme)
		{
			if( scheme==null)	return;
			for( int i=0; i<schemes.Count; i++)
				if( schemes.GetSceme(i) == scheme)
				{
					this.SelectedIndex = i;
					return;
				}
		}

		public void AddColorScheme(Color[] aColors)
		{
			// ?? поставить проверку на повторения цветовых схем
			ColorScheme	scheme = GetScheme(aColors);
			if( scheme != null)	return;
			schemes.AddScheme( aColors);
			this.Items.Add("color scheme");
		}

		public ColorScheme	SelectedScheme
		{
			get
			{
				ColorScheme	sceme = schemes.GetSceme(this.SelectedIndex);
				return sceme;
			}
		}

		#region Message handlers

		private void ColorSchemeComboBox_MeasureItem(object sender, MeasureItemEventArgs e)
		{
			e.ItemHeight = itemHeight+2*border+2*margins;
			e.ItemWidth = this.DropDownWidth;
		}

		private void DrawComboColors(Graphics graphics, Rectangle bounds, Color[] aColors, bool drawBorder, bool selected)
		{
			//
			// Draw colors
			int count = aColors.GetLength(0);
			if( count==0)	return;
			float xPos = 0;
			int yPos = 2;
			float itemWidth = 0;
			float litemHeight = itemHeight;
//graphics.Clip = new Region(new Rectangle(new Point(-1,-1), Bounds.Size));
//graphics.FillRectangle( Brushes.Wheat, new Rectangle(new Point(-1,-1), Bounds.Size));
			if( !drawBorder)
			{
				itemWidth = (this.Width-18)/(float)count;
				litemHeight -= 4;
				yPos += 1;
				xPos = 1;
			}
			else
			{
				xPos = bounds.Left + border + margins;
				yPos = bounds.Top + border + margins;
				itemWidth = (this.DropDownWidth-2*border-2*margins-16)/(float)count;

				if( selected)
					graphics.FillRectangle(new SolidBrush(selectedBorderColor), bounds);
				else
					graphics.FillRectangle(new SolidBrush(borderColor), bounds);
				// Draw inside white border
				Rectangle insideRect = bounds;
				insideRect.Inflate(-margins,-margins);
				graphics.FillRectangle(new SolidBrush(Color.White), insideRect);
			}

			for( int i=0; i<count; i++)
			{
				SolidBrush	sb = new SolidBrush(aColors[i]);
				graphics.FillRectangle(sb, new Rectangle((int)xPos, yPos, (int)itemWidth, (int)litemHeight));
				sb.Dispose();
				xPos += (int)itemWidth;
			}
		}

		private void ColorSchemeComboBox_DrawItem(object sender, DrawItemEventArgs e)
		{
			ColorScheme	sceme = schemes.GetSceme(e.Index);
			if( sceme==null || e.Index>=schemes.Count)
				return;
			//
			// Draw colors
			Color[]	aColors = sceme.Colors;
			if( (e.State & DrawItemState.ComboBoxEdit) == DrawItemState.ComboBoxEdit)
			{
				DrawComboColors( e.Graphics, e.Bounds, aColors, false, false);
			}
			else
				DrawComboColors( e.Graphics, e.Bounds, aColors, true, (e.State & DrawItemState.Selected) == DrawItemState.Selected);
		}

		private void ColorSchemeComboBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			if( SelectedSchemeChanged!=null)
				SelectedSchemeChanged( sender, e);
			//timer.Enabled=true;
		}

		protected override void UpdateEditItemPos(IntPtr hwndItem)
		{
			Platform.MoveWindow(hwndItem, 0, 0, this.Width-30, 16, true);
		}

		private void ColorSchemeComboBox_PostPaint(object sender, EventArgs e)
		{
			
		}

		#endregion

		protected override void DrawButtonImage(Graphics graphics, Image image)
		{
			base.DrawButtonImage(graphics, image);
			if( SelectedIndex>=0 && SelectedIndex<Items.Count)
			{
				ColorScheme	sceme = schemes.GetSceme(SelectedIndex);
				DrawComboColors( graphics, new Rectangle(Point.Empty, this.Size), sceme.Colors, false, false);
			}
		}

		private void timer_Tick(object sender, EventArgs e)
		{
			this.Invalidate();
			this.Update();
			this.Refresh();
			timer.Enabled=false;
		}
	}

	#endregion
}