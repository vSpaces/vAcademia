using System;
using System.Drawing;
using System.Collections;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for BitmapLibrary.
	/// </summary>
	public class BitmapLibrary
	{
		static Hashtable hashBitmaps = new Hashtable();

		public BitmapLibrary()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public	static	Image GetBitmap(string fileName)
		{
			IDictionaryEnumerator myEnumerator = hashBitmaps.GetEnumerator();
			while ( myEnumerator.MoveNext() )
			{
				if( myEnumerator.Key.ToString() == fileName)
				{
					return (Image)((Image)myEnumerator.Value).Clone();
				}
			}
			Image image = null;
			try
			{
				image = Image.FromFile(fileName);
				hashBitmaps.Add(fileName, image);
				image = (Image)image.Clone();
			}
			catch(Exception)
			{
				image = null;
			}
			return image;
		}
	}
}
