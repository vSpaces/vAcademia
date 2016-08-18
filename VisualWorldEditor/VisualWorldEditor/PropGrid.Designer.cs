namespace VisualWorldEditor
{
    partial class PropGrid
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
        this.propertyGrid = new System.Windows.Forms.PropertyGrid();
        this.cameraButton = new System.Windows.Forms.Button();
        this.SuspendLayout();
        // 
        // propertyGrid
        // 
        this.propertyGrid.Location = new System.Drawing.Point(0, 0);
        this.propertyGrid.Name = "propertyGrid";
        this.propertyGrid.Size = new System.Drawing.Size(315, 315);
        this.propertyGrid.TabIndex = 1;
        this.propertyGrid.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid_PropertyValueChanged);
        // 
        // cameraButton
        // 
        this.cameraButton.Location = new System.Drawing.Point(60, 324);
        this.cameraButton.Name = "cameraButton";
        this.cameraButton.Size = new System.Drawing.Size(200, 24);
        this.cameraButton.TabIndex = 2;
        this.cameraButton.Text = "Взять текущую камеру";
        this.cameraButton.Click += new System.EventHandler(this.cameraButton_Click);
        this.cameraButton.MouseClick += new System.Windows.Forms.MouseEventHandler(this.OnGetCamera);
        // 
        // PropGrid
        // 
        this.ClientSize = new System.Drawing.Size(315, 353);
        this.Controls.Add(this.propertyGrid);
        this.Controls.Add(this.cameraButton);
        this.Name = "PropGrid";
        this.Text = "Свойства объекта";
        this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid;
        private System.Windows.Forms.Button cameraButton;

    }
}