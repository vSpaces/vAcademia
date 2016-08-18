using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Nini.Config;

namespace VisualWorldEditor
{
    public partial class Options : Form
    {
        private static EditorSettings settings = new EditorSettings();

        private static byte m_currentServerNumber = 0;

        public Options()
        {
            InitializeComponent();
        }

        private void Options_Load(object sender, EventArgs e)
        {
            textBoxClientPath.Text = settings.ClientPath;
            
            if(settings.ServerPath == "")
            {
                btnServerPath.Visible = false;
            }
            else
            {
                btnServerPath.Visible = true;
            }

            //mysql
            mysqlHostTBox.Text = settings.MysqlHost;
            mysqlLoginTBox.Text = settings.MysqlLogin;
            mysqlPasswordTBox.Text = settings.MysqlPassword;

            linkToEditor.Checked = settings.LinkToEditor;
        }

        private void Save()
        {
            settings.ClientPath = textBoxClientPath.Text;

            settings.MysqlHost = mysqlHostTBox.Text;
            settings.MysqlLogin = mysqlLoginTBox.Text;
            settings.MysqlPassword = mysqlPasswordTBox.Text;

            settings.LinkToEditor = linkToEditor.Checked;

            settings.Save();
        }

        private void ButtonClientPath_Click(object sender, EventArgs e)
        {
            DialogResult dlgResult = ClientBrowserDialog.ShowDialog();
            if(dlgResult == DialogResult.OK)
            {
                textBoxClientPath.Text = ClientBrowserDialog.SelectedPath;
            }
        }

        private void ButtonSave_Click(object sender, EventArgs e)
        {
            Save();
            this.Close();
        }

        private void ButtonCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnServerPath_Click(object sender, EventArgs e)
        {
            settings.ServerPath = "";
            settings.Save();
            ServerPath.GetServerPath("");
        }

        //для доступа к параметрам используются статические методы
        public static String GetClientPath()
        {
            // Если путь еще не установлен, открыть форму
            if(settings.ClientPath == "")
            {
                FolderBrowserDialog clientPathBrowser = new FolderBrowserDialog();
                clientPathBrowser.Description = "Выберите папку:";
                clientPathBrowser.RootFolder = Environment.SpecialFolder.MyComputer;
                clientPathBrowser.SelectedPath = "C:\\Program Files\\Virtual Academia";
                clientPathBrowser.ShowNewFolderButton = false;

                MessageBox.Show("Выберите папку, куда установлен клиент.", "Настройка", MessageBoxButtons.OK, MessageBoxIcon.Information);

                DialogResult dlgResult = clientPathBrowser.ShowDialog();

                if (dlgResult == DialogResult.OK)
                {
                    settings.ClientPath = clientPathBrowser.SelectedPath;
                    settings.Save();
                }
            }

            return settings.ClientPath;
        }

        public static void SetCurrentServerNumber(byte currentServerNumber)
        {
            m_currentServerNumber = currentServerNumber;
        }

        public static byte GetCurrentServerNumber()
        {
            return m_currentServerNumber;
        }

        public static String GetHost()
        {
            lock (settings)
            {
                try
                {
                    // берем из файла настроек клиента
                    // используем Nini для чтения
                    IConfigSource playerIni = new IniConfigSource(GetClientPath() + "\\player.ini");
                    if (playerIni == null)
                    {
                        MessageBox.Show("Не могу открыть файл " + GetClientPath() + "\\player.ini", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return "";
                    }
                    settings.MysqlHost = playerIni.Configs["paths"].Get("server");
                    settings.Save();
                }
                catch (Exception e)
                {
                }
            }

            return settings.MysqlHost;
        }

        public static void SetMysqlLogin(String login)
        {
            settings.MysqlLogin = login;
            settings.Save();
        }

        public static String GetMysqlLogin()
        {
            return settings.MysqlLogin;
        }

        public static void SetMysqlPassword(String password)
        {
            settings.MysqlPassword = password;
            settings.Save();
        }

        public static String GetMysqlPassword()
        {
            return settings.MysqlPassword;
        }

        public static bool GetLinkToEditorState()
        {
            return settings.LinkToEditor;
        }

        private void linkToEditor_CheckedChanged(object sender, EventArgs e)
        {

        }

    }
}