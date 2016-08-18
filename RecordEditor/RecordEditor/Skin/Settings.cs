using System;
using System.Drawing;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for Settings.
	/// </summary>
	public class Settings
	{
		public Settings()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		static public string AnimIcons = "";
		static public Color AnimationsTracksColor = Color.FromArgb(255, 248, 195);
		static public Color AnimationsTracksSelectedColor = Color.FromArgb(255,169,43);
		static public Color PanelsColor = Color.FromArgb(127, 152, 202);
		static public Color LabelsForeColor = Color.Black;//Color.FromArgb(127, 152, 202);
		static public Color MenuItemBackColor = Color.FromArgb(68, 89, 155);
		static public Color AnimPanelBackColor = Color.FromArgb(255,255,255);	// цвет фона ListView панели анимации
		static public Color AnimPanelSelectedColor = Color.FromArgb(102,144,219);	// цвет выделенной линии на панели анимации
		static public Color AnimPopupBackColor = Color.FromArgb(66,89,156);	// цвет фона popup меню
		static public Color ResizeBorderColor = Color.FromArgb(53, 48, 121);
		static public Color MenuItemSelectedForeColor = Color.FromArgb(255,228,205); 
		static public Color MenuItemSelectedBackColor = Color.FromArgb(101,156,205); 
		static public Color ComboItemSelectedFrameColor = Color.FromArgb(10,36,106); 
		static public Color MenuItemDisabledForeColor = Color.FromArgb(112,132,189); 
		static public Color TimeLineColor = Color.White; 
		static public Color SelectedLayerBackColor = Color.FromArgb(208,239,255);
		static public Color LCObjectNameBackColor = Color.White;
		static public Color MenuItemForeColor = Color.White;
		static public Color RegTextColorForeColor = Color.White;
	}
}
