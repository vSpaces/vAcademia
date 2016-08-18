using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Data;
using System.Runtime.InteropServices;

namespace FlashGenie
{
	/// <summary>
	/// Interface for all resizible skinnable elements
	/// </summary>
	interface	IResizableSkinnedControl
	{
		string  GetSkinFileName();
		bool	SetSkin(FormSkin	skin);
		bool	SetSkin(string	fullName);
		void	SkinControls();
		void	UpdateSkin();
	}

	/// <summary>
	/// 
	/// </summary>
	public class SkinPanel : UserControl, IResizableSkinnedControl
	{
		protected	ResizableSkinController	skinController;

		public SkinPanel()
		{
			skinController = new ResizableSkinController(this);
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
		}

		#region IResizableSkinnedControl Members

		public virtual string  GetSkinFileName()
		{
			return	SkinSettings.BasePanelSkinFilename;
		}

		public virtual	void	UpdateSkin()
		{
		}

		public bool SetSkin(string	fullName)
		{
			if(skinController==null)	return false;
			skinController.SkinFile = fullName;
			return true;
		}

		public bool SetSkin(FormSkin	skin)
		{
			if(skinController==null)	return false;
			skinController.Skin = skin;
			return true;
		}

		public	void	SkinControls()
		{
			skinController.SkinControls();
		}

		#endregion

		/// <summary>
		/// 
		/// </summary>
		private string sSkinFile = "";
		[CategoryAttribute("Appearance")]
		public string SkinFile
		{
			set
			{
				sSkinFile = value;
				SetSkin(sSkinFile);
			}
			get
			{
				return sSkinFile;
			}
		}

	}

}
