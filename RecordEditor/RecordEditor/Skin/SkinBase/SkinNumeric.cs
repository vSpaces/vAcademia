using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace FlashGenie
{
	/// <summary>
	/// Summary description for SkinNumeric.
	/// </summary>
	public class SkinNumeric : System.Windows.Forms.UserControl
	{
		[DllImport("user32.dll", CharSet=CharSet.Auto)]
		private static extern IntPtr GetForegroundWindow();

		protected	ResizableSkinController	skinController;
		public event EventHandler	ValueChanged;
		private ToggleResizableButton numeric_btnBack;
		private System.Windows.Forms.TextBox textBox1;
		private FlashGenie.seButton numeric_btnNumUp;
		private FlashGenie.seButton numeric_btnNumDown;
		private System.Windows.Forms.Timer timer1;
		private System.ComponentModel.IContainer components;
		private	bool upTimer;
		private	IntPtr	currentForeground;
		//bool	NeedValidationAfterKeyPress=false;

		public SkinNumeric()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			skinController = new ResizableSkinController(this);
			UpdateSkin();

			CheckLayout();

			this.Leave += new EventHandler(SkinNumeric_Leave);
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.numeric_btnNumUp = new FlashGenie.seButton();
			this.numeric_btnNumDown = new FlashGenie.seButton();
			this.numeric_btnBack = new FlashGenie.ToggleResizableButton();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.SuspendLayout();
			// 
			// numeric_btnNumUp
			// 
			this.numeric_btnNumUp.Active = false;
			this.numeric_btnNumUp.ActiveImage = null;
			this.numeric_btnNumUp.DisabledImage = null;
			this.numeric_btnNumUp.Location = new System.Drawing.Point(44, 0);
			this.numeric_btnNumUp.Name = "numeric_btnNumUp";
			this.numeric_btnNumUp.Pressed = false;
			this.numeric_btnNumUp.PressedImage = null;
			this.numeric_btnNumUp.Size = new System.Drawing.Size(21, 11);
			this.numeric_btnNumUp.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.numeric_btnNumUp.TabIndex = 1;
			this.numeric_btnNumUp.Click += new System.EventHandler(this.seButton1_Click);
			this.numeric_btnNumUp.MouseUp += new System.Windows.Forms.MouseEventHandler(this.numeric_btnNumDown_MouseUp);
			this.numeric_btnNumUp.MouseLeave += new System.EventHandler(this.numeric_btnNumDown_MouseLeave);
			this.numeric_btnNumUp.MouseDown += new System.Windows.Forms.MouseEventHandler(this.numeric_btnNumDown_MouseDown);
			this.numeric_btnNumUp.Leave += new System.EventHandler(this.numeric_btnNumDown_Leave);
			// 
			// numeric_btnNumDown
			// 
			this.numeric_btnNumDown.Active = false;
			this.numeric_btnNumDown.ActiveImage = null;
			this.numeric_btnNumDown.BackColor = System.Drawing.SystemColors.Control;
			this.numeric_btnNumDown.DisabledImage = null;
			this.numeric_btnNumDown.Location = new System.Drawing.Point(44, 11);
			this.numeric_btnNumDown.Name = "numeric_btnNumDown";
			this.numeric_btnNumDown.Pressed = false;
			this.numeric_btnNumDown.PressedImage = null;
			this.numeric_btnNumDown.Size = new System.Drawing.Size(21, 11);
			this.numeric_btnNumDown.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.numeric_btnNumDown.TabIndex = 2;
			this.numeric_btnNumDown.Click += new System.EventHandler(this.seButton2_Click);
			this.numeric_btnNumDown.MouseUp += new System.Windows.Forms.MouseEventHandler(this.numeric_btnNumDown_MouseUp);
			this.numeric_btnNumDown.MouseLeave += new System.EventHandler(this.numeric_btnNumDown_MouseLeave);
			this.numeric_btnNumDown.MouseDown += new System.Windows.Forms.MouseEventHandler(this.numeric_btnNumDown_MouseDown);
			this.numeric_btnNumDown.Leave += new System.EventHandler(this.numeric_btnNumDown_Leave);
			// 
			// numeric_btnBack
			// 
			this.numeric_btnBack.Active = false;
			this.numeric_btnBack.ActiveImage = null;
			this.numeric_btnBack.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(231)), ((System.Byte)(230)), ((System.Byte)(251)));
			this.numeric_btnBack.DisabledImage = null;
			this.numeric_btnBack.EastWidth = 6;
			this.numeric_btnBack.Location = new System.Drawing.Point(0, 0);
			this.numeric_btnBack.Name = "numeric_btnBack";
			this.numeric_btnBack.Pressed = false;
			this.numeric_btnBack.PressedImage = null;
			this.numeric_btnBack.Size = new System.Drawing.Size(43, 22);
			this.numeric_btnBack.Style = FlashGenie.FlatToggleButtonStyles.Normal;
			this.numeric_btnBack.TabIndex = 0;
			this.numeric_btnBack.WestWidth = 3;
			// 
			// textBox1
			// 
			this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.textBox1.Location = new System.Drawing.Point(6, 5);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(35, 13);
			this.textBox1.TabIndex = 3;
			this.textBox1.Text = "0";
			this.textBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox1_KeyPress);
			this.textBox1.Validating += new System.ComponentModel.CancelEventHandler(this.textBox1_Validating);
			this.textBox1.Leave += new System.EventHandler(this.textBox1_TextChanged);
			this.textBox1.KeyUp += new System.Windows.Forms.KeyEventHandler(this.textBox1_KeyUp);
			// 
			// timer1
			// 
			this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// SkinNumeric
			// 
			this.Controls.Add(this.textBox1);
			this.Controls.Add(this.numeric_btnNumDown);
			this.Controls.Add(this.numeric_btnNumUp);
			this.Controls.Add(this.numeric_btnBack);
			this.Name = "SkinNumeric";
			this.Size = new System.Drawing.Size(65, 22);
			this.SizeChanged += new System.EventHandler(this.SkinNumeric_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		double	numValue=0.0;
		public double Value
		{
			get
			{
				return numValue;
			}
			set
			{
				double	nv = value;
				if( value > MaxValue)	nv = MaxValue;
				else	if( value < MinValue)	nv = MinValue;
				if( numValue != nv)
				{
					numValue= nv;
					OnNumEditChanged();
				}
			}
		}

		double	step=0.1;
		public double Step
		{
			get
			{
				return step;
			}
			set
			{
				step = value;
			}
		}

		double	min=-10000000;
		public double MinValue
		{
			get
			{
				return min;
			}
			set
			{
				min = value;
			}
		}

		double	max=10000000;
		public double MaxValue
		{
			get
			{
				return max;
			}
			set
			{
				max = value;
			}
		}

		string	format=@"{0:F2}";
		public string Format
		{
			get
			{
				return format;
			}
			set
			{
				format = value;
				OnNumEditChanged();
			}
		}

		public void OnNumEditChanged()
		{
			if( !noTextChanges)
			{
				string	val = "";
				val = string.Format(format, Value);
				textBox1.Text = val;
			}
			if(ValueChanged != null)
				ValueChanged(this, new EventArgs());
		}


		private void seButton1_Click(object sender, System.EventArgs e)
		{
			Value += Step;
		}

		private void seButton2_Click(object sender, System.EventArgs e)
		{
			Value -= Step;
		}

		string	sLastText = "";
		private void textBox1_TextChanged(object sender, System.EventArgs e)
		{
			Validate();
		}

		bool noTextChanges = false;
		private void textBox1_Validating(object sender, System.ComponentModel.CancelEventArgs e)
		{
			try
			{
				noTextChanges = true;
				Value = Double.Parse(textBox1.Text);
				noTextChanges = false;
				sLastText = textBox1.Text;
			}
			catch(Exception)
			{
				e.Cancel = true;
				//textBox1.Text = Value.ToString();
				textBox1.Text = sLastText;
			}			
		}

		private void SkinNumeric_SizeChanged(object sender, System.EventArgs e)
		{
			CheckLayout();
		}

		void CheckLayout()
		{
			numeric_btnNumUp.Left = this.Width - numeric_btnNumUp.Width;
			numeric_btnNumDown.Left = this.Width - numeric_btnNumDown.Width;
			numeric_btnBack.Width = this.Width - numeric_btnNumDown.Width;
			textBox1.Width = this.Width-29;
		}

		#region	Skinning
		public bool UpdateSkin()
		{
			if(skinController==null)	return false;
			skinController.SkinFile = SkinSettings.ControlsSkinFilename;
			return true;
		}
		#endregion	//Skinning

		private void textBox1_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			bool	handled = false;
			//if( e.KeyChar == 13)
			if( e.KeyChar == '-')
			{
				if( textBox1.Text.IndexOf('-') != -1)
						handled = true;
//				else
//					NeedValidationAfterKeyPress = true;
			}
			else	if( e.KeyChar == ',')
			{
				if( textBox1.Text.IndexOf(',') != -1)
					handled = true;
			}
			else	if( e.KeyChar < '0' || e.KeyChar > '9')	handled = true;

			if( handled)
				e.Handled = true;
			else
				textBox1_Validating(sender, new CancelEventArgs(true));
		}

		private void numeric_btnNumDown_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			upTimer =  sender==numeric_btnNumDown?false:true;
			timer1.Enabled = true;
			((seButton)sender).Capture = true;
			currentForeground = GetForegroundWindow();
		}

		private void numeric_btnNumDown_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			timer1.Enabled = false;
			((seButton)sender).Capture = false;
		}

		private void timer1_Tick(object sender, System.EventArgs e)
		{
			if( currentForeground != GetForegroundWindow())
				timer1.Enabled = false;
			if( upTimer)	Value += Step;
			else	Value -= Step;
		}

		private void numeric_btnNumDown_Leave(object sender, System.EventArgs e)
		{
			timer1.Enabled = false;
		}

		private void numeric_btnNumDown_MouseLeave(object sender, System.EventArgs e)
		{
			timer1.Enabled = false;
		}

		private void textBox1_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			textBox1_Validating(sender, new CancelEventArgs(true));
		}

		private void SkinNumeric_Leave(object sender, EventArgs e)
		{
			noTextChanges = true;
			this.textBox1.Text = Value.ToString();
			noTextChanges = false;			
		}
	}
}
