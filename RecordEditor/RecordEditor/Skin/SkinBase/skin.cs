using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Reflection;
using System.Collections;
using System.Xml;
using System.Windows.Forms;

namespace FlashGenie
{
	/// <summary>
	/// </summary>
	public	enum	SkinElements
	{
		Empty,
		icon,// - иконка
		btnClose,// - кнопка закрытия
		btnMaximize,// - кнопка Maximize
		btnMinimize,// - кнопка Minimize
		btnResize// - зона для изменения размеров
	};

	public class SkinElementPropertyInfo
	{
		private string mname;
		private string mval;

		public SkinElementPropertyInfo()
		{
		}

		public string name
		{
			get
			{
				return mname;
			}
			set
			{
				mname = value;
			}
		}

		public string value
		{
			get
			{
				return mval;
			}
			set
			{
				mval = value;
			}
		}
	}

	/// <summary>
	/// 
	/// </summary>
	public class FormSkin
	{
		private	ArrayList	subSkins;
		// Current skin size
		private	Size	skinSize = new Size();
		// Directory with skin files
		private string	sResDir	= "";
		// Array of bitmaps for the skin
		private ArrayList	bitmaps = new ArrayList();
		// Array of elements for the skin
		private ArrayList	levels = new ArrayList();
		// Array of active elements of the form: buttons, title bar.
		private ArrayList	controls = new ArrayList();
		// Array of properties of the control
		private ArrayList	properties = new ArrayList();
		// Indicates that skin must be portion updated
		public PaintEventHandler		OnNeedUpdate=null;

		public FormSkin()
		{
			// 
			// TODO: Add constructor logic here
			//
			subSkins = new ArrayList();
		}

		public	string	ResDir
		{
			get
			{
				return sResDir;
			}
			set
			{
				sResDir = value;
			}
		}

		public	ArrayList	SubSkins
		{
			get
			{
				return	subSkins;
			}
		}

		#region	Skin loading

		/// <summary>
		/// Loads skin from specified file
		/// </summary>
		public bool Load(string dir, string fileName)
		{
			if( fileName == "")	return false;
			dir = SkinSettings.BuildFullPath(dir);
			sResDir = dir;
			XmlDocument xmlDocument = new XmlDocument();
			XmlNode root = null;
			try
			{
				xmlDocument.Load(dir + @"\" + fileName);
				root = xmlDocument.FirstChild;
				if (root == null)
					return false;
			}
			catch(Exception)
			{
				return false;
			}

			try
			{
				root = root.NextSibling;
				LoadFormSkin(root);
			}
			catch(Exception)
			{
				return false;
			}
			return true;
		}

		/// <summary>
		/// Loads skin from xml node file
		/// </summary>
		public bool Load(XmlNode root)
		{
			try
			{
				LoadFormSkin(root);
			}
			catch(Exception)
			{
				return false;
			}
			return true;
		}

		/// <summary>
		/// Loads skin from specified file
		/// </summary>
		public bool Load(string fullPath)
		{
			int iDotIndex = -1;
			string	sDir = "";
			string	sFile = fullPath;
			if((iDotIndex=fullPath.LastIndexOf(@"\")) != -1)
			{
				sDir = fullPath.Substring(0, iDotIndex);
				sFile = fullPath.Substring(iDotIndex+1, fullPath.Length - iDotIndex - 1);
			}
			return Load(sDir, sFile);
		}

		/// <summary>
		/// Load form skin from specified file
		/// </summary>
		/// <param name="xmlNode"></param>
		internal void LoadFormSkin(XmlNode xmlNode)
		{
			if (!xmlNode.HasChildNodes)
				return;

			foreach(XmlNode node in xmlNode.ChildNodes)
			{
				if( node.Name == @"bitmaps")
					bitmaps = LoadBitmaps(node);
				else	if( node.Name == @"skin")
					levels = LoadLevels(node);
				else	if( node.Name == @"controls")
					controls = LoadControls(node);
				else	if( node.Name == @"properties")
					properties = LoadProperties(node);
				else	if( node.Name == @"include")
					LoadInclude(node);
				else	if( node.Name != @"#comment") throw new Exception("Unexpected node in skin file [window child].");
			}
		}

		internal ArrayList LoadProperties(XmlNode xmlNode)
		{
			ArrayList	mProps = new ArrayList();
			foreach(XmlNode node in xmlNode.ChildNodes)
			{
				SkinElementPropertyInfo	propInfo = (SkinElementPropertyInfo)LoadElement(node);
				mProps.Add(propInfo);
			}
			return mProps;
		}

        /// <summary>
        /// Sets property of the object using the Reflection mechanism.
        /// </summary>
        /// <param name="target">target object</param>
        /// <param name="param">node attribute</param>
        static public void SetProperty(object target, string sPropName, string sPropVal)
        {
            PropertyInfo propInfo = target.GetType().GetProperty(sPropName);
            if(propInfo==null)
            {
                Console.WriteLine("Wrong property ["+target.GetType().ToString()+","+sPropName+"]");
                return;
            }
            object propValue = null;
            try
            {
                propValue = ParseType(propInfo.PropertyType, sPropVal);
            }
            catch(Exception exc)
            {
                if( propInfo.PropertyType==typeof(System.Single) || propInfo.PropertyType==typeof(System.Double))
                {
                    string oldSep=",";
                    string newSep=".";
                    string spr = sPropVal;
                    if( spr.IndexOf(".") != -1)
                    {
                        oldSep=".";
                        newSep=",";
                    }
                    spr = spr.Replace(oldSep, newSep);
                    try
                    {
                        propValue = ParseType(propInfo.PropertyType, spr);
                    }
                    catch(Exception exc2)
                    {
                        Console.WriteLine("Exception occured: " + exc2.Message);
                        Console.WriteLine(exc.StackTrace);
                    }
                }
                else
                {
                    Console.WriteLine("Exception occured: " + exc.Message);
                    Console.WriteLine(exc.StackTrace);
                }
            }
            propInfo.SetValue(target, propValue, null);
        }

        /// <summary>
        /// Sets property of the object using the Reflection mechanism.
        /// </summary>
        /// <param name="target">target object</param>
        /// <param name="param">node attribute</param>
        static public void SetProperty(object target, string sPropName, object objValue)
        {
            PropertyInfo propInfo = target.GetType().GetProperty(sPropName);
            if(propInfo==null)
            {
                Console.WriteLine("Wrong property ["+target.GetType().ToString()+","+sPropName+"]");
                return;
            }
            propInfo.SetValue(target, objValue, null);
        }


		public void	SetControlProperties(Control control)
		{
			if( control==null)	return;
			for(int iprop=0; iprop<properties.Count; iprop++)
			{
				SkinElementPropertyInfo	propInfo = (SkinElementPropertyInfo)properties[iprop];
				if( propInfo == null)
					continue;
                string  propName = propInfo.name;
                Control	propControl = control;

                int idx=-1;
                if( (idx=propInfo.name.LastIndexOf(".")) != -1)
                {
                    propControl = ResizableSkinController.GetNamedControl(control, propName.Substring(0, idx));
                    propName = propName.Substring(idx+1, propName.Length-idx-1);
                }
                //
                if( propControl!=null)
				    SetProperty(propControl, propName, propInfo.value);
			}
		}

		/// <summary>
		/// Load bitmaps for the skin
		/// </summary>
		/// <param name="xmlNode"></param>
		/// <returns></returns>
		internal ArrayList	LoadBitmaps(XmlNode xmlNode)
		{
			ArrayList	mBitmaps = new ArrayList();
			foreach(XmlNode node in xmlNode.ChildNodes)
			{
				SkinBitmap	skinBitmap = (SkinBitmap)LoadElement(node);
				mBitmaps.Add(skinBitmap);
			}
			return mBitmaps;
		}

		/// <summary>
		/// Load included skin
		/// </summary>
		/// <param name="xmlNode"></param>
		/// <returns></returns>
		internal void	LoadInclude(XmlNode xmlNode)
		{
			FormSkin	includeSkin = new FormSkin();
			foreach (XmlAttribute node in xmlNode.Attributes)
			{
				if( node.Name == @"file")
				{
					try
					{
						if( includeSkin.Load(ResDir+@"\", node.Value))
						{
							subSkins.Add(includeSkin);
							return;
						}
					}
					catch(Exception)
					{
					}
				}
			}
		}

		/// <summary>
		/// Load all levels for the skin
		/// </summary>
		/// <param name="xmlNode"></param>
		/// <returns></returns>
		internal ArrayList	LoadLevels(XmlNode xmlNode)
		{
			ArrayList	mLevels = new ArrayList();
			foreach(XmlNode node in xmlNode.ChildNodes)
			{
				SkinLevel	skinLevel = (SkinLevel)LoadLevel(node);
				if( skinLevel == null)	continue;
				mLevels.Add(skinLevel);
			}
			return mLevels;
		}

		/// <summary>
		/// Load all hotspots for the skin
		/// </summary>
		/// <param name="xmlNode"></param>
		/// <returns></returns>
		internal ArrayList	LoadControls(XmlNode xmlNode)
		{
			ArrayList	mControls = new ArrayList();
			foreach(XmlNode node in xmlNode.ChildNodes)
			{
				if( node.Name == @"#comment")	continue;
				SkinBaseControl	skinControl = (SkinBaseControl)LoadElement(node);
				if( skinControl == null)	continue;
				if(node.ChildNodes.Count!=0)
				{
					FormSkin	skin = new FormSkin();
					skin.ResDir = ResDir;
					foreach(XmlNode cnode in node.ChildNodes)
						if( cnode.Name=="window" && skin.Load(cnode))
						{
							skinControl.Skin = skin;
							break;
						}
				}
				else
				{
					foreach (XmlAttribute attr in node.Attributes)
					{
						if( attr.Name == @"file")
						{
							FormSkin	skin = new FormSkin();
							skin.ResDir = ResDir;
							try
							{
								if( skin.Load(ResDir+@"\", attr.Value))
								{
									skinControl.Skin = skin;
									break;
								}
							}
							catch(Exception)
							{
								break;
							}
						}
					}
				}
				mControls.Add(skinControl);
			}
			return mControls;
		}

		/// <summary>
		/// Load all elements for the skin level
		/// </summary>
		/// <param name="xmlNode"></param>
		/// <returns></returns>
		internal ArrayList	LoadElements(XmlNode xmlNode)
		{
			ArrayList	mElements = new ArrayList();
			foreach(XmlNode node in xmlNode.ChildNodes)
			{
				SkinBaseElement	skinElement = (SkinBaseElement)LoadElement(node);
				mElements.Add(skinElement);
			}
			return mElements;
		}

		/// <summary>
		/// Load one skin level
		/// </summary>
		/// <param name="xmlNode"></param>
		/// <returns></returns>
		internal SkinLevel	LoadLevel(XmlNode xmlNode)
		{
			if( xmlNode.Name == @"#comment")	return null;
			SkinLevel	mLevel = new SkinLevel();
			mLevel.elements = LoadElements( xmlNode);
			foreach (XmlAttribute node in xmlNode.Attributes)
				SetProperty(mLevel, node);
			return mLevel;
		}

		/// <summary>
		/// Loads the style element from document node. 
		/// </summary>
		/// <param name="objectNode">document node which repersents object</param>
		/// <returns>instance of class SlideElement</returns>
		private Object LoadElement(XmlNode styleNode)
		{
			string objectType = styleNode.Name;
			Object	retobject = null;
			if (objectType == "bitmap")
			{
				retobject = new SkinBitmap(sResDir);
			}
			if (objectType == "element")
			{
				retobject = new SkinBaseElement();
				((SkinBaseElement)retobject).BitmapChanged += new BitmapChangedEventHandler( element_BitmapChanged);
			}
			else	if (objectType == "control")
			{
				retobject = new SkinBaseControl();
				((SkinBaseElement)retobject).BitmapChanged += new BitmapChangedEventHandler( element_BitmapChanged);
			}
			else	if (objectType == "property")
			{
				retobject = new SkinElementPropertyInfo();
			}
			/*else	if (objectType == "button")
			{
				retobject = new FlatToggleButton();
				((SkinBaseElement)retobject).BitmapChanged += new BitmapChangedEventHandler( element_BitmapChanged);
				((FlatToggleButton)retobject).OnNeedUpdate +=new EventHandler(FormSkin_OnNeedUpdate);
				((FlatToggleButton)retobject).OnClick +=new EventHandler(FormSkin_OnClick);
			}
			else	if (objectType == "dragbutton")
			{
				retobject = new DragFormButton();
				((DragFormButton)retobject).OnDragBy +=new MouseEventHandler(FormSkin_OnDragBy);
			}
			else	if (objectType == "resizebutton")
			{
				retobject = new ResizeFormButton();
				((ResizeFormButton)retobject).OnResizeBy +=new MouseEventHandler(FormSkin_OnResizeBy);
			}*/

			if(retobject != null)
				foreach (XmlAttribute node in styleNode.Attributes)
					SetProperty(retobject, node);

			return retobject;
		}

		/// <summary>
		/// Sets property of the object using the Reflection mechanism.
		/// </summary>
		/// <param name="target">target object</param>
		/// <param name="propertyNode">document node of property</param>
		static public void SetProperty(object target, XmlNode propertyNode)
		{
			PropertyInfo propInfo = target.GetType().GetProperty(propertyNode.Name);

			if (propertyNode.FirstChild != null && propertyNode.FirstChild is XmlText)
			{
				object propValue = null;
				try
				{
					propValue = ParseType(propInfo.PropertyType, propertyNode.FirstChild.Value);
				}
				catch(Exception exc)
				{
					Console.WriteLine("Exception occured: " + exc.Message);
					Console.WriteLine(exc.StackTrace);
				}
				propInfo.SetValue(target, propValue, null);
			}
		}


		/// <summary>
		/// Converts the string representation of the 
		/// specified type to an instance of the type.  
		/// </summary>
		/// <param name="type"></param>
		/// <param name="str">string representation</param>
		/// <returns>instance of type</returns>
		internal static object ParseType(Type type, string str)
		{
			object valueObject = null;
			if (type == typeof(string))
				return str;
			else if (type.IsPrimitive)
			{							
				if (type == typeof(System.Boolean))
					valueObject = Boolean.Parse(str);
				else if (type == typeof(System.Int32))
					valueObject = Int32.Parse(str);
				else if (type == typeof(System.UInt32))
					valueObject = UInt32.Parse(str);
				else if (type == typeof(System.Byte))
					valueObject = Byte.Parse(str);
				else if (type == typeof(System.Single))
					valueObject = Single.Parse(str);
				else if (type == typeof(System.Double))
				{
					System.Globalization.NumberFormatInfo nfi = new System.Globalization.NumberFormatInfo();
					nfi.NumberDecimalSeparator = ".";
					valueObject = Double.Parse(str, nfi);
				}
			}
			else if (type.IsEnum)
				valueObject = Enum.Parse(type, str);
			else if (type == typeof(Color))
			{
				if(str[0] == '#')
				{
					int A = 255;
					int R = Int32.Parse(str.Substring(1,2), System.Globalization.NumberStyles.HexNumber);
					int G = Int32.Parse(str.Substring(3,2), System.Globalization.NumberStyles.HexNumber);
					int B = Int32.Parse(str.Substring(5,2), System.Globalization.NumberStyles.HexNumber);
					valueObject = Color.FromArgb(A, R, G, B);
				}
				else
				{
					ArrayList listParsed = CommaParse(str);
					int A = Int32.Parse(listParsed[0].ToString());
					int R = Int32.Parse(listParsed[1].ToString());
					int G = Int32.Parse(listParsed[2].ToString());
					int B = Int32.Parse(listParsed[3].ToString());
					valueObject = Color.FromArgb(A, R, G, B);
				}
			}				
			else if (type == typeof(Point))
			{
				ArrayList listParsed = CommaParse(str);
				int x = Int32.Parse(listParsed[0].ToString());
				int y = Int32.Parse(listParsed[1].ToString());
				valueObject = new Point(x, y);
			}
			else if (type == typeof(Rectangle))
			{
				ArrayList listParsed = CommaParse(str);
				int x = Int32.Parse(listParsed[0].ToString());
				int y = Int32.Parse(listParsed[1].ToString());
				int w = Int32.Parse(listParsed[2].ToString());
				int h = Int32.Parse(listParsed[3].ToString());
				valueObject = new Rectangle(x, y, w, h);
			}
			else if (type == typeof(Size))
			{
				ArrayList listParsed = CommaParse(str);
				int width = Int32.Parse(listParsed[0].ToString());
				int height = Int32.Parse(listParsed[1].ToString());
				valueObject = new Size(width, height);
			}				
			else if (type == typeof(Matrix))
			{
				ArrayList listParsed = CommaParse(str);
				System.Globalization.NumberFormatInfo nfi = new System.Globalization.NumberFormatInfo();
				nfi.NumberDecimalSeparator = ".";
				double m11 = Double.Parse(listParsed[0].ToString(), nfi);
				double m12 = Double.Parse(listParsed[1].ToString(), nfi);
				double m21 = Double.Parse(listParsed[2].ToString(), nfi);
				double m22 = Double.Parse(listParsed[3].ToString(), nfi);
				double m31 = Double.Parse(listParsed[4].ToString(), nfi);
				double m32 = Double.Parse(listParsed[5].ToString(), nfi);
				valueObject = new Matrix((float)m11,(float)m12,(float)m21,(float)m22,(float)m31,(float)m32);
			}
			return valueObject;
		}

		/// <summary>
		/// Converts the specified comma separated string 
		/// to the list of string values.
		/// </summary>
		/// <param name="str">comma separated string</param>
		/// <returns>list of string values</returns>
		internal static ArrayList CommaParse(string str)
		{
			ArrayList list = new ArrayList();
			while (str.IndexOf(",") != -1)
			{
				string before = str.Substring(0, str.IndexOf(","));
				list.Add(before.Trim());
				str = str.Substring(str.IndexOf(",") + 1);
			}
			list.Add(str.Trim());
			return list;
		}

		#endregion

		#region	Bitmap malipulations

		/// <summary>
		/// Try to return skin bitmap for specified name
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		public void element_BitmapChanged(object sender, BitmapChangedArgs e)
		{
			e.BitmapSkin = GetBitmap(e.Name);
		}

		private	SkinBitmap	GetBitmap(string name)
		{
			if( bitmaps == null)	return null;
			foreach(SkinBitmap bitmap in bitmaps)
				if( bitmap.name == name)	return bitmap;
			return null;
		}

		/*private	SkinBaseElement	GetElement(SkinElements type)
		{
			if( elements == null)	return null;
			foreach(SkinBaseElement element in elements)
				if( element.type == type)	return element;
			return null;
		}*/

		#endregion

		#region Clear

		public void ClearResources()
		{
			foreach(SkinBitmap bitmap in bitmaps)
			{
				bitmap.ClearResources();
			}
			bitmaps.Clear();
			foreach(FormSkin formskin in subSkins)
			{
				formskin.ClearResources();
			}
			subSkins.Clear();
		}

		#endregion

		/// <summary>
		/// Calculate regions of skin parts
		/// </summary>
		/// <param name="dwWidth">Width of the skin</param>
		/// <param name="dwHeight">Height of the skin</param>
		public Region UpdateRegions(int dwWidth, int dwHeight)
		{
			if( dwWidth <=0 || dwHeight <=0)	return null;
			Rectangle	pureRectangle = new Rectangle(0, 0, dwWidth, dwHeight);
			skinSize = pureRectangle.Size;
			if( levels != null)
				foreach(SkinLevel level in levels)
				{
					if( pureRectangle.Width==0 || pureRectangle.Height==0)	break;
					Rectangle restRectangle = level.UpdateRegions(pureRectangle);
					pureRectangle = restRectangle;
				}
			Region	region = SkinToRegion(Color.FromArgb(255,167,150));

			foreach(Object obj in SubSkins)
			{
				FormSkin	formSkin = (FormSkin)obj;
				Region	subregion = formSkin.UpdateRegions(dwWidth, dwHeight);
				if( subregion != null)
				{
					if( region != null)
						region.Dispose();
					region = subregion;
				}
			}

			return region;
		}

		protected Region SkinToRegion(Color transparencyColor)
		{
			GraphicsPath path = new GraphicsPath();
			ArrayList	pureRects = new ArrayList();
			Rectangle	skinRectangle = new Rectangle(0, 0, skinSize.Width, skinSize.Height);

			if( levels.Count > 0)
			{
				foreach( SkinLevel level in levels)
					level.GetRegionAndRects(path, pureRects, transparencyColor);
			}
			else
			{
				path.Dispose();
				return null;
			}

			Region region = new Region(path);
			path.Dispose();
			return region;
		}

		/// <summary>
		/// Paint skin if necessary
		/// </summary>
		public void Paint(PaintEventArgs e)
		{
			foreach(SkinLevel level in levels)
				if( e.ClipRectangle.IntersectsWith(level.Bounds))
					level.Paint(e.Graphics);
			foreach(Object obj in SubSkins)
			{
				FormSkin	formSkin = (FormSkin)obj;
				formSkin.Paint( e);
			}
		}

		/// <summary>
		/// Buttons of the skin
		/// </summary>
		public ArrayList Controls
		{
			get
			{
				return controls;
			}
		}

		private void FormSkin_OnNeedUpdate(object sender, EventArgs e)
		{
			/*if( OnNeedUpdate != null)
				OnNeedUpdate( this, new PaintEventArgs(null, ((FlatToggleButton)sender).Bounds));*/
		}

		public	Color	GetPixelColor(int x, int y)
		{
			foreach(SkinLevel level in levels)
				if( level.Bounds.Contains(new Point(x,y)))
					return level.GetPixelColor(x,y);
			return Color.Black;
		}
	}
}
