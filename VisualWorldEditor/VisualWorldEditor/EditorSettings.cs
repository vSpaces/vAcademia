
using Nini.Config;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System;
using System.Text;
using System.IO;

namespace VisualWorldEditor {

   /* internal sealed partial*/ class EditorSettings {

        private IConfigSource m_settingsIni;
               
        [DllImport("shell32.dll")]
        static extern bool SHGetSpecialFolderPath(
	    IntPtr hwndOwner,[Out] StringBuilder lpszPath, int nFolder, bool fCreate);
        
        public EditorSettings()
        {
            StringBuilder _dirPath = new StringBuilder(260);
            SHGetSpecialFolderPath(IntPtr.Zero, _dirPath, 0x001A, false);
            string dirPath = _dirPath.ToString();
            dirPath += "\\editor";
            string path = dirPath + "\\settings.ini";

            if (File.Exists(path) == false)
            {
                System.IO.Directory.CreateDirectory(dirPath);
                System.IO.FileStream fs = System.IO.File.Create(path);
                string section = "[settings]\n";
                for (int i = 0; i < section.Length; i++)
                {
                    fs.WriteByte((byte)section[i]);
                }
                fs.Close();
            }

            try
            {
                m_settingsIni = new IniConfigSource(path);                
                if (m_settingsIni.Configs["settings"] == null)
                {
                    m_settingsIni.AddConfig("settings");
                }
            }
            catch (Exception /*e*/)
            {
                MessageBox.Show("Не могу открыть файл " + path, "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);            
            }                        
        }

       private void SetProperty(string name, string value)
       {
            m_settingsIni.Configs["settings"].Set(name, value);
       }

       private string GetProperty(string name)
       {
            string s = m_settingsIni.Configs["settings"].Get(name);
            return (s == null) ? "" : s;
       }
                
        public string ClientPath 
        {
            get 
            {
                return GetProperty("ClientPath");
            }
            set 
            {
                SetProperty("ClientPath", value);                
            }
        }
        
        public string MysqlLogin 
        {
            get 
            {
                return GetProperty("MysqlLogin");                
            }
            set 
            {
                SetProperty("MysqlLogin", value);                 
            }
        }
        
        public string MysqlPassword 
        {
            get 
            {
                return GetProperty("MysqlPassword");                
            }
            set 
            {
                SetProperty("MysqlPassword", value);                 
            }           
        }
                
        public string MysqlHost 
        {
            get 
            {
                return GetProperty("MysqlHost");                
            }
            set 
            {
                SetProperty("MysqlHost", value);                 
            } 
        }
                
        public string ServerPath 
        {
            get 
            {
                return GetProperty("ServerPath");                
            }
            set 
            {
                SetProperty("ServerPath", value);                 
            }
        }
                
        public bool LinkToEditor 
        {
            get 
            {
                // попахивает индийским погромистом, но лень тратить на это время
                return ((GetProperty("LinkToEditor") == "true") ||
                    (GetProperty("LinkToEditor") == "True") ||
                    (GetProperty("LinkToEditor") == "TRUE"));
            }
            set 
            {
                SetProperty("LinkToEditor", value.ToString());                 
            }          
        }

       public void Save()
       {
            lock (this)
            {
                m_settingsIni.Save();
            }
       }
    }
}
