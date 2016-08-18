using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.IO;
using System.Collections;

namespace FlashGenie
{
	/// <summary>
	/// 
	/// </summary>
	public class ResizableSkinController
	{
		protected	Control		slaver;
		protected	Size		lastUpdatedSize;

		public ResizableSkinController(Control	aslaver)
		{
			// 
			// TODO: Add constructor logic here
			//
			if( aslaver != null)
			{
				aslaver.SizeChanged += new EventHandler(slaver_SizeChanged);
				aslaver.Paint +=new PaintEventHandler(slaver_Paint);
				aslaver.VisibleChanged +=new EventHandler(aslaver_VisibleChanged);
			}
			slaver = aslaver;
			lastUpdatedSize = new Size();
		}

		/// <summary>
		/// Path to skin directory
		/// </summary>
		private string sSkinDirectory = "";
		[Description("Skin path"), Category("Appearance")]
		public string SkinDirectory
		{
			get
			{
				return sSkinDirectory;
			}
			set
			{
				sSkinDirectory = value;
				if( !SetSkin(sSkinDirectory))
					sSkinDirectory = "";
			}
		}

		/// <summary>
		/// Path to skin directory
		/// </summary>
		private string sSkinFile = "";
		[Description("Skin file"), Category("Appearance")]
		public string SkinFile
		{
			get
			{
				return sSkinFile;
			}
			set
			{
				sSkinFile = value;
				if( !SetSkinFromFile(sSkinFile))
					sSkinFile = "";
			 }
		}


		// Current form skin
		private	FormSkin	mSkin;
		public	FormSkin	Skin
		{
			get
			{
				return mSkin;
			}
			set
			{
				if( mSkin != null)
				{
					mSkin.ClearResources();
					mSkin = null;
				}
				mSkin = value;
				OnSkinChanged();
			}
		}


		/// <summary>
		/// Path to directory with skin files
		/// </summary>
		private bool SetSkin(string dirPath)
		{
			if( dirPath==null || dirPath=="")	return false;
			DirectoryInfo	info = new DirectoryInfo(dirPath);
			if( !info.Exists)	return false;
			FileInfo[] files = info.GetFiles();

			FormSkin	skin = new FormSkin();
			foreach(FileInfo file in files)
			{
				if( file.Extension == @".xml")
				{
					if( SetSkinFromFile(file.FullName)) return true;
				}
			}
			return false;
		}
	

		/// <summary>
		/// Path to directory with skin files
		/// </summary>
		private bool SetSkinFromFile(string fullPath)
		{
			FormSkin skin = new FormSkin();
			if( skin.Load(fullPath))
			{
				Skin = skin;
				return true;
			}
			else
			{
				Skin = null;
			}
			return false;
		}

		protected void OnSkinChanged()
		{
			SkinControls();
			if( mSkin != null)
				mSkin.OnNeedUpdate += new PaintEventHandler( skin_OnNeedUpdate);
			slaver.ResumeLayout(false);
			lastUpdatedSize = Size.Empty;
			slaver_SizeChanged( this, new EventArgs());
		}

		/// <summary>
		/// Skin all controls if necessary
		/// </summary>
		public void SkinControls()
		{
			if( mSkin!=null && mSkin.SubSkins!=null)
			{
				foreach(Object obj in mSkin.SubSkins)
				{
					FormSkin	formSkin = (FormSkin)obj;
					SkinControls( formSkin);
				}
			}
			SkinControls( mSkin);
		}

		protected	void	SkinControls(FormSkin	skin)
		{
			if( skin == null)	return;
			ArrayList	controls = skin.Controls;
			if( controls == null)	return;

			foreach(object control in controls)
			{
				Control	formControl = GetNamedControl(((SkinBaseControl)control).name);
				if( formControl != null)
				{
					if(formControl is ISkinnedControl)
						((ISkinnedControl)formControl).SetSkinElement((SkinBaseElement)control);
					else	if(formControl is IResizableSkinnedControl)
						((IResizableSkinnedControl)formControl).SetSkin(((SkinBaseElement)control).Skin);
					if( ((SkinBaseElement)control).Skin!=null)
						((SkinBaseElement)control).Skin.SetControlProperties(formControl);
				}
				else
				{
					if( slaver.GetType().ToString()==((SkinBaseControl)control).name && slaver is ISkinnedControl)
					{
						((ISkinnedControl)slaver).SetSkinElement((SkinBaseElement)control);
						if( ((SkinBaseElement)control).Skin!=null)
							((SkinBaseElement)control).Skin.SetControlProperties(formControl);
					}
				}
			}
			if( skin != null)
				skin.SetControlProperties(slaver);
		}

		private void slaver_Paint(object sender, PaintEventArgs e)
		{
			if( Skin != null)
				Skin.Paint(e);
		}

		private void slaver_SizeChanged(object sender, EventArgs e)
		{
			if( (lastUpdatedSize.Width!=slaver.Size.Width) || (lastUpdatedSize.Height!=slaver.Size.Height))
			{
				if( Skin != null)	
				{
					Region region = Skin.UpdateRegions(slaver.Width, slaver.Height);
					if( region != null)
					{
						if( slaver.Name == "PromptBox")
						{
							//RectangleF	rcBounds = region.GetBounds(slaver.CreateGraphics());
							//int i=0;
							//slaver.Region = region;
						}
						else
						{
							//slaver.Region = region;
						}
						//if( slaver.Name != "PromptBox")
#if QVIZ_VERSION
						if( !(slaver is Form))
						{
							if( slaver.Region != null)
								slaver.Region.Dispose();
							slaver.Region = region;
						}
#else
						slaver.Region = region;
#endif
					}
					lastUpdatedSize = slaver.Size;
				}
				slaver.Invalidate();
				slaver.Update();
			}
		}

		static public Control GetNamedControl(Control control, string name)
		{
			if( control == null)	return null;
			int dotidx = name.IndexOf(@".");
			if( dotidx!=-1)
			{
				Control	ret=null;
				ret = GetNamedControl(control, name.Substring(0, dotidx));
				name = name.Substring(dotidx+1, name.Length-dotidx-1);
				return GetNamedControl(ret, name);
			}
			else
			{
				foreach(Control ctrl in control.Controls)
					if( ctrl.Name == name)
						return ctrl;
			}
			return null;
		}

		protected Control GetNamedControl(string name)
		{
			if( slaver == null)	return null;
			return GetNamedControl(slaver, name);
		}

		protected void skin_OnNeedUpdate(object target, PaintEventArgs e)
		{
			if( slaver != null)
				slaver.Invalidate(e.ClipRectangle);
		}

		private void aslaver_VisibleChanged(object sender, EventArgs e)
		{
			if( slaver!=null && slaver.Visible==true)
				SkinControls();
		}
	}
}
