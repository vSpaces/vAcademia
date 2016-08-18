using System;
using System.Windows.Forms;
using System.Text;

namespace FlashGenie
{
	public class SkinSettings
	{
		#region	Skin paths and filenames

		public	const	string FormVideoExportSkinFilename = @"FormVideoExport.xml";
		public	const	string VideoSettingsFormSkinFilename = @"VideoSet.xml";
		public	const	string ExportTLFormSkinFilename = @"exporttlform.xml";
		public	const	string FileFormSkinFilename = @"fileform.xml";
		public	const	string BaseFormSkinFilename = @"SkinSettings.xml";
		public	const	string TextPropsFormSkinFilename = @"TextPropsForm.xml";
		public	const	string ChartPropsFormSkinFilename = @"ChartPropsForm.xml";
		public	const	string ChartNameFormSkinFilename = @"ChartNameForm.xml";
		public	const	string ChartURLFormSkinFilename = @"ChartURLForm.xml";
		public	const	string ImagePropsFormSkinFilename = @"ImagePropsForm.xml";
		public	const	string BaseAFXDialog = @"BaseAFXDialog.xml";
		public	const	string MoviePropsFormSkinFilename = @"MoviePropsForm.xml";
		public	const	string RegFormSkinFilename = @"RegForm.xml";
		public	const	string ElementPropsFormSkinFilename = @"ElementPropsForm.xml";
		public	const	string StylesFormSkinFilename = @"StylesForm.xml";
		public	const	string FormColorSkinFilename = @"FormColor.xml";
		public	const	string FormProgressSkinFilename = @"FormProgress.xml";
		public	const	string FormMessageSkinFilename = @"FormMessage.xml";
		public	const	string FormPreviewSkinFilename = @"FormPreview.xml";
		public	const	string FormPromptSkinFilename = @"FormPromptBox.xml";
		public	const	string DirFormSkinFilename = @"FormDSelect.xml";
		public	const	string BasePanelSkinFilename = @"";

#if	QVIZ_VERSION
		private static string sSkinDirectory = @"skins\quickviz\";
#else
		private static string sSkinDirectory = @"skins\RecordEditor\";
#endif

		public static string SkinDirectory
		{
			get
			{
				if( sSkinDirectory.IndexOf(":")!=-1)	return sSkinDirectory;
				return Application.StartupPath + @"\"+sSkinDirectory;
			}
			set
			{
				sSkinDirectory = value;
			}
		}

		public	static	string	BaseFormSkinPath
		{
			get
			{
				return	SkinDirectory + BaseFormSkinFilename;
			}
		}

		public	static	string	ControlsSkinFilename
		{
			get
			{
				return	SkinDirectory + @"controls.xml";
			}
		}

		static public string CorrectFilePath( string aPath)
		{
			aPath = aPath.Replace(@"/", @"\");
			if( aPath.IndexOf(@"\\")!=-1)
			{
				bool	isNetworkAbs=false;
				if( aPath.Length>2 && aPath.Substring(0,2)==@"\\")
					isNetworkAbs = true;
				aPath = aPath.Replace(@"\\", @"\");
				if( isNetworkAbs)
				{
					aPath = @"\" + aPath;
				}
				// Lñø¨ðõü ÿõ¨òvù ûø°ýøù ¸ûõ°
				if( aPath.Length>2 && aPath[0]=='\\' && aPath[1]!='\\')
					aPath = aPath.Substring(1, aPath.Length-1);
			}
			if( aPath.Length>2 && aPath[aPath.Length-1] == '\\' && aPath[aPath.Length-2] != ':')
			{
				aPath = aPath.Substring(0, aPath.Length-1);
			}
			return aPath;
		}

		static public	string	BuildFullPath(string path)
		{
			if(path.LastIndexOf(@":")!=-1)	return path;
			string stuppath = Application.StartupPath;
			if( stuppath.Length==0)	return path;
			stuppath = stuppath.Replace(@"/", @"\");
			if( stuppath[stuppath.Length-1]!='\\')
				stuppath += @"\";
			if( path.Length==0)	return stuppath;

			path = CorrectFilePath(path);	//STD_DIALOGS
			return stuppath+path;
		}

		static	public	void SkinController(IntPtr	handle)
		{
			try
			{
				string	path = BuildFullPath(SkinDirectory)+ @"scrollers.bmp";
				//path = "";
				ASCIIEncoding textConverter = new ASCIIEncoding();
				byte[]	buffer = textConverter.GetBytes(path);
				//Platform.SkinWindowScrollers(handle, buffer);
			}
			catch(Exception)
			{
			}
		}
		#endregion	//Skin paths and filenames
	}
}