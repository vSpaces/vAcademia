using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    public partial class ClientMySqlLogin : Form
    {
        public ClientMySqlLogin()
        {
            InitializeComponent();
        }

        private void enterBtn_Click(object sender, EventArgs e)
        {
            Options.SetMysqlLogin(this.loginTBox.Text);
            Options.SetMysqlPassword(this.passwordTBox.Text);

            this.Close();
        }
    }
}