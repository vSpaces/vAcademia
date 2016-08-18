using System;
using System.Drawing;

namespace FlashGenie
{
	/// <summary>
	/// Class for loading and containing bitmaps
	/// </summary>
	public class SkinBitmap
	{
		// Path to Root catalog
		private	string	sResPath = "";
		// Image instance
		private	Image	image = null;

		public SkinBitmap(string aResPath)
		{
			// 
			// TODO: Add constructor logic here
			//
			sResPath = aResPath;
		}

		public void ClearResources()
		{
			if( image != null)
			{
				image.Dispose();
				image = null;
			}
		}

		/// <summary>
		/// Identifier of the bitmap
		/// </summary>
		private string sName = "";
		public string name
		{
			get
			{
				return sName;
			}
			set
			{
				sName = value;
			}
		}

		/// <summary>
		/// Source of the bitmap
		/// </summary>
		private string sSrc= "";
		public string src
		{
			get
			{
				return sSrc;
			}
			set
			{
				sSrc = value;
				OnSrcChanged();
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
				return mKeyColor;
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
				return bKeyColor;
			}
			set
			{
				bKeyColor = value;
			}
		}

		/// <summary>
		/// Image data
		/// </summary>
		public Image Image
		{
			get
			{
				return image;
			}
		}

		#region	Event raisers

		/// <summary>
		/// Occures when user change src of the bitmap
		/// </summary>
		protected	void	OnSrcChanged()
		{
			if( image != null)	image.Dispose();
			string	fullPath = "";
			if( sResPath != null)	fullPath = sResPath + @"\";
			fullPath +=  src;

			/*try
			{
				image = Image.FromFile(fullPath);
			}
			catch(Exception)
			{
			}*/
			image = BitmapLibrary.GetBitmap( fullPath);
		}

		protected	void	OnKeyColorChanged()
		{
			bKeyColor = true;
		}

		#endregion
	}
}
