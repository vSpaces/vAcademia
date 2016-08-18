using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace FlashGenie
{
	public class slider : FlashGenie.sePanel
	{
		private bool		drag=false;
		private Point		dragPoint=Point.Empty;
		private seButton	btnSlider;
		private	float		minValue=0f;
		private	float		maxValue=1f;
		private	int			trackSize=13;
		private	float		currentValue=0f;
		private System.ComponentModel.IContainer components = null;
		public	event	EventHandler	OnValueChanged = null;

		public slider()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.SizeChanged += new EventHandler(slider_SizeChanged);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.btnSlider = new FlashGenie.seButton();
			this.SuspendLayout();
			// 
			// btnSlider
			// 
			this.btnSlider.Active = false;
			this.btnSlider.ActiveImage = null;
			this.btnSlider.DisabledImage = null;
			this.btnSlider.Location = new System.Drawing.Point(1, 1);
			this.btnSlider.Name = "btnSlider";
			this.btnSlider.Pressed = false;
			this.btnSlider.PressedImage = null;
			this.btnSlider.Size = new System.Drawing.Size(7, 13);
			this.btnSlider.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.btnSlider.TabIndex = 0;
			this.btnSlider.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btnSlider_MouseUp);
			this.btnSlider.MouseMove += new System.Windows.Forms.MouseEventHandler(this.btnSlider_MouseMove);
			this.btnSlider.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnSlider_MouseDown);
			// 
			// slider
			// 
			this.Controls.Add(this.btnSlider);
			this.Name = "slider";
			this.Size = new System.Drawing.Size(208, 15);
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.slider_Paint);
			this.ResumeLayout(false);

		}
		#endregion

		public	float	MinValue
		{
			get
			{
				return minValue;
			}
			set
			{
				if( minValue!=value && value<maxValue)
				{
					minValue=value;
					OnMinMaxChanged();
				}
			}
		}

		public	float	MaxValue
		{
			get
			{
				return maxValue;
			}
			set
			{
				if( maxValue!=value && value>minValue)
				{
					maxValue=value;
					OnMinMaxChanged();
				}
			}
		}

		public	int	TrackSize
		{
			get
			{
				return trackSize;
			}
			set
			{
				if( trackSize!=value && value>0)
				{
					trackSize=value;
					OnTrackSizeChanged();
				}
			}
		}

		public	float	Value
		{
			get
			{
				return minValue + (maxValue-minValue)*currentValue;
			}
			set
			{
				if( value>=minValue && value<=maxValue)
				{
					if(maxValue!=minValue)
					{
						currentValue = (value-minValue)/(maxValue-minValue);
					}
				}
				OnExternValueChanged();
			}
		}

		void OnExternValueChanged()
		{
			int	semiWidth = btnSlider.Width/2;
			this.btnSlider.Left = semiWidth + (int)((this.Width-semiWidth*2)*currentValue);
			ValueChanged();
			Refresh();
		}

		void OnMinMaxChanged()
		{
			ValueChanged();
		}

		void OnTrackSizeChanged()
		{
			btnSlider.Size = new Size(trackSize, trackSize);
			btnSlider.Top = (int)((this.Height - trackSize)/2f);
		}

		#region Mouse handlers
		private void btnSlider_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			drag = true;
			btnSlider.Capture=true;
			dragPoint.X = e.X;
			dragPoint.Y = e.Y;
		}

		private void btnSlider_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			drag = false;
			btnSlider.Capture=false;
		}

		private void btnSlider_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if( !drag)	return;
			int	newX = (e.X - dragPoint.X) + this.btnSlider.Left;
			if( newX < 0)	newX = 0;
			if( newX > this.Width - this.btnSlider.Width)	newX = this.Width - this.btnSlider.Width;
			if( newX != this.btnSlider.Left)
			{
				this.btnSlider.Left = newX;
				if( this.Width > btnSlider.Width)
				{
					currentValue = btnSlider.Left/(float)(this.Width - btnSlider.Width);
				}
				else
				{
					currentValue = 0f;
				}
				this.Refresh();
				ValueChanged();
			}
		}
		#endregion // mouse handlers

		void ValueChanged()
		{
			if( OnValueChanged != null)
			{
				OnValueChanged( this, new EventArgs());
			}
		}

		private void slider_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			int	workWidth = this.Width - btnSlider.Width;
			Rectangle	rcDraw = new Rectangle(btnSlider.Width/2+1, 1, (int)(workWidth*currentValue), this.Height-2);
			SolidBrush	brush = new SolidBrush(Color.FromArgb(255, 232, 102));
			e.Graphics.FillRectangle(brush, rcDraw);
			brush.Dispose();
		}

		private void slider_SizeChanged(object sender, EventArgs e)
		{
			OnExternValueChanged();
		}
	}
}

