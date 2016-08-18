using System;
using System.Drawing;
using System.Windows.Forms;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for ResizableButton.
	/// </summary>
	public class ResizableButton : seButton
	{
		public	event	EventHandler	OnResizeStart = null;
		public	event	EventHandler	OnBeforeResizeStart = null;
		public	new event	EventHandler	OnResize = null;
		public	event	EventHandler	OnResizeEnd = null;

		private	Form	myForm = null;
		private	Form	myBounds = null;
		private	bool	resizeStarted = false;
		private	Point	resizeMouseStartPos = Point.Empty;

		public ResizableButton()
		{
			//
			// TODO: Add constructor logic here
			//
			this.MouseUp += new System.Windows.Forms.MouseEventHandler(ResizableButton_MouseUp);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(ResizableButton_MouseMove);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(ResizableButton_MouseDown);
		}

		public Form ResizableForm
		{
			set
			{
				myForm = value;
			}
		}

		public Form ResizableBounds
		{
			set
			{
				myBounds = value;
			}
		}

		private void ResizableButton_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (resizeStarted)	
			{
				resizeStarted = false;
				Capture = false;
				if( OnResizeEnd != null)
					OnResizeEnd( this, new EventArgs());
				//MoveWindow(Handle, resizeForm.Left, resizeForm.Top, resizeForm.Width, resizeForm.Height, true);
				//HideResizeForm();
			}
		}

		private void ResizableButton_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (resizeStarted)	
			{
				int newWidth = myForm.Width + (e.X - resizeMouseStartPos.X); 
				int newHeight = myForm.Height + (e.Y - resizeMouseStartPos.Y); 
				if( newWidth < myForm.MinimumSize.Width)
					newWidth = myForm.MinimumSize.Width;
				if( newHeight < myForm.MinimumSize.Height)
					newHeight = myForm.MinimumSize.Height;
				Platform.MoveWindow(myBounds.Handle, myBounds.Left, myBounds.Top, newWidth, newHeight, true);
				if( OnResize != null)
					OnResize( this, new EventArgs());
				//InvalidateResizeForm();
			}
		}

		private void ResizableButton_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if( OnBeforeResizeStart != null)
				OnBeforeResizeStart( this, new EventArgs());
			if( myForm == null)	return;
			if (myForm.WindowState == FormWindowState.Normal)
			{
				Capture = true;
				resizeStarted = true;
				// Create resize form
				resizeMouseStartPos = new Point(e.X, e.Y);
				//CreateAndAlignResizeForm();
				//ShowResizeForm();
				//InvalidateResizeForm();
				if( OnResizeStart != null)
					OnResizeStart( this, new EventArgs());
			}
		}
	}
}
