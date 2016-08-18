using System;
using System.Runtime.InteropServices;
using System.Drawing;

namespace FlashGenie
{
	/// <summary>
	/// 
	/// </summary>
	public class Platform
	{
		public Platform()
		{
			// 
			// TODO: Add constructor logic here
			//
		}

		internal	enum	DIRCHANGEINFO
		{
			DC_CHANGED,
			DC_WRONGDIRECTORY,
		}

		internal	enum	DRIVETYPE
		{
			Removable=1,
			LocalDisk=2,
			CD=3,
			Network=4,
			Directory=5
		}

		internal	struct	DRIVEINFO
		{
			public	int				depth;	// directory depth
			public	string			path;	// path
			public	string			name;	// letter or directory name
			public	DRIVETYPE		type;
		}

		internal enum ResizeDirection
		{
			North,
			South,
			West,
			East,
			NorthWest,
			NorthEast,
			SouthWest,
			SouthEast
		}

		public const	int	WS_VSCROLL = 0x00200000;
		public const	int	WS_HSCROLL = 0x00100000;
		public const	int	WS_CLIPCHILDREN = 0x02000000;
		public const	int	WS_CLIPSIBLINGS = 0x04000000;
		public const	int	WS_EX_TRANSPARENT = 0x00000020;
		public const	int	WS_MINIMIZEBOX = 0x00020000;
		public const	int WM_LBUTTONDOWN = 0x0201;
		public const	int WM_ERASEBKGND = 0x0014;
		public const	int WM_PAINT = 0x000f;
		public const	int WM_ENABLE = 0x000a;
		public const	int CB_GETCOMBOBOXINFO = 0x0164;
		public const	int CB_SETEXTENDEDUI = 0x0155;
		public const	int WS_EX_TOOLWINDOW = 0x00000080;
		public const	int WS_EX_APPWINDOW = 0x00040000;
		//public const	uint GCL_HBRBACKGROUND = (-10);

		public const	int	GWL_STYLE  = -16;
		public const	int	GWL_EXTSTYLE  = -20;

		#region Imported functions

		[DllImportAttribute(@"user32.dll")]
		public static extern int GetWindowLong(IntPtr hWnd, int index);
		[DllImportAttribute(@"user32.dll")]
		public static extern int SetWindowLong(IntPtr hWnd, int index, int newLong);
		[DllImportAttribute(@"scrdll.dll")]
		public static extern void SkinWindowScrollers(IntPtr hWnd, byte[] buffer);
		[DllImportAttribute(@"scrdll.dll")]
		public static extern void CoolShowScrollBar(IntPtr hWnd, int wBar, bool bShow);

		// File info
		[StructLayout(LayoutKind.Sequential)]
		public struct SHFILEINFO
		{
			public IntPtr hIcon;
			public IntPtr iIcon;
			public uint dwAttributes;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
			public string szDisplayName;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
			public string szTypeName;
		};

		public const uint SHGFI_ICON = 0x100;
		public const uint SHGFI_LARGEICON = 0x0;    // 'Large icon
		public const uint SHGFI_SMALLICON = 0x1;    // 'Small icon
		[DllImport("shell32.dll")]
		public static extern IntPtr SHGetFileInfo(string pszPath,
			uint dwFileAttributes,
			ref SHFILEINFO psfi,
			uint cbSizeFileInfo,
			uint uFlags);

		public static IntPtr	GetFileIconHandle(string fileName)
		{
			SHFILEINFO	shinfo = new SHFILEINFO();
			IntPtr hImgSmall = SHGetFileInfo(fileName, 0, ref shinfo,
				(uint)Marshal.SizeOf(shinfo),
				SHGFI_ICON |
				SHGFI_SMALLICON);
			return shinfo.hIcon;
		}

		public static Icon	GetFileIcon(string fileName)
		{
			IntPtr	hHandle = GetFileIconHandle( fileName);
			if( hHandle == IntPtr.Zero)	return null;
			System.Drawing.Icon myIcon =
				System.Drawing.Icon.FromHandle(hHandle);
			return myIcon;
		}

		#endregion

		#region Window functions

		public static void SetWindowStyle(IntPtr hWnd, int style)
		{
			SetWindowLong( hWnd, GWL_STYLE, style);
		}

		public static void SetWindowExStyle(IntPtr hWnd, int style)
		{
			SetWindowLong( hWnd, GWL_EXTSTYLE, style);
		}

		public static int GetWindowStyle(IntPtr hWnd)
		{
			return GetWindowLong( hWnd, GWL_STYLE);
		}

		public static int GetWindowExStyle(IntPtr hWnd)
		{
			return GetWindowLong( hWnd, GWL_EXTSTYLE);
		}

		[DllImportAttribute("user32.dll")]
		public static extern bool MoveWindow(IntPtr hWnd, int x, int y, int width, int height, bool repaint);

		[DllImport("user32.dll", CharSet=CharSet.Auto)]
		public static extern int SendMessage(IntPtr hWnd, int msg, int wParam, ref COMBOBOXINFO lParam);
		[DllImport("user32.dll", CharSet=CharSet.Auto)]
		public static extern int SendMessage(IntPtr hWnd, int msg, bool wParam, int lParam);
		[DllImport("user32.dll", CharSet=CharSet.Auto)]
		public static extern IntPtr SetCapture(IntPtr hWnd);

		#endregion

		#region Structures
		[StructLayout(LayoutKind.Sequential)]
		public struct RECT
		{
			public int left;
			public int top;
			public int right;
			public int bottom;
		}


		[StructLayout(LayoutKind.Sequential)]
		public struct COMBOBOXINFO
		{
			public int cbSize;
			public RECT rcItem;
			public RECT rcButton;
			public uint cbStateButton;
			public IntPtr hwndCombo;
			public IntPtr hwndItem;
			public IntPtr hwndList;
		}
		#endregion

		static public System.Drawing.Font	GetLabelsFont()
		{
			return new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel, ((System.Byte)(204)));
		}

		static public System.Drawing.Font	GetLabelsFontBold()
		{
			return new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Pixel, ((System.Byte)(204)));
		}

		static public void UpdateFont( System.Windows.Forms.Control aControl, Font aFont)
		{
			if( aControl != null)
			{
				if( aControl.Font.Unit != GraphicsUnit.Pixel)
					aControl.Font = aFont;
			}
		}

		static public void UpdateFonts( System.Windows.Forms.Control[] aControls, Font aFont)
		{
			for( int i=0; i<aControls.Length; i++)
			{
				UpdateFont(aControls[i], aFont);
			}
		}
	}
}
