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

	/// <summary>
	/// Base interface for all skinned elements
	/// </summary>
	interface ISkinnedControl
	{
		void SetSkinElement(SkinBaseElement	element);
    }
}