using System;
using System.IO;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Threading;
using System.Globalization;

namespace VisualWorldEditor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // проверка на запуск второй копии
            bool isFirstInstance;
            copyMutex = new Mutex(false, "WorldEditorMutex", out isFirstInstance);
            if(!isFirstInstance)
            {
                return;
            }           

            CultureInfo ciCurrent = (CultureInfo)Application.CurrentCulture.Clone();
            ciCurrent.NumberFormat.NumberDecimalSeparator = ".";
            Application.CurrentCulture = ciCurrent;

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            //try
            //{
                Application.Run(new mainForm());
            //}
            //catch (Exception e)
            //{                
            //    CrashProcedure(e);
            //}
        }

        public static void CrashProcedure(Exception e)
        {   
            StreamWriter crashLog = new StreamWriter("crash.txt", true);
            crashLog.WriteLine(e.ToString());
            crashLog.Close();

            Console.WriteLine("Editor crash: " + e.ToString() + "\n");
            MessageBox.Show("ѕроизошел непредвиденный сбой в работе редактора.\n—ейчас произойдет завершение работы клиента (если запущен),\n и будет предложено сохранить изменени€ (если есть).", "—бой", MessageBoxButtons.OK, MessageBoxIcon.Error);

            try
            {
                if (ClientLoader.m_clientProcess != null)
                {
                    if (ClientLoader.m_clientProcess.HasExited == false)
                    {
                        ClientLoader.m_clientProcess.Kill();
                    }

                }
            }
            catch (System.ComponentModel.Win32Exception w)
            {
                Console.WriteLine("Can't close client: " + w.Message);
            }

            ClientMySQL mySql = new ClientMySQL();
            HistoryProcessor.GetHProcessor().ProcessHistory(mySql, true);
        }

        public static Mutex copyMutex;
    }
}