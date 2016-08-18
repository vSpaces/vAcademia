using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using RecordEditor.Logic;

namespace RecordEditor
{
    static class Program
    {
        public static Context context = null;
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            context = new Context();
            context.recordEditor = new RecordEditor();
            context.recordController = new RecordController();
            context.playerSession = new PlayerSession();
            context.messageHandlerManager = new MessageHandlerManager();

            context.recordEditor.SubscribeEvents();

            Application.Run( context.recordEditor);
        }
    }
}
