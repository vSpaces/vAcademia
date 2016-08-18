using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Windows.Forms;
using System.Threading;
using Nini.Config;

namespace VisualWorldEditor
{
    partial class ClientLoader
    {
        private ProcessStartInfo m_clientLoadInfo;
        public static Process m_clientProcess;

        [StructLayout(LayoutKind.Sequential)]
        public struct WINDOWPLACEMENT
        {
            public int length;
            public int flags;
            public int showCmd;
            public System.Drawing.Point ptMinPosition;
            public System.Drawing.Point ptMaxPosition;
            public System.Drawing.Rectangle rcNormalPosition;

            public static WINDOWPLACEMENT Default
            {
                get
                {
                    WINDOWPLACEMENT result = new WINDOWPLACEMENT();
                    result.length = Marshal.SizeOf(result);
                    return result;
                }
            }
        }

        [DllImport("user32.dll")]
        static extern bool GetWindowPlacement(IntPtr hWnd, [In] ref WINDOWPLACEMENT lpwndpl);

        [DllImport("user32.dll")]
        static extern bool SetWindowPlacement(IntPtr hWnd, [In] ref WINDOWPLACEMENT lpwndpl);

        [DllImport("user32.dll")]
        static extern bool DrawMenuBar(IntPtr hWnd);

        [DllImport("user32.dll")]
        static extern IntPtr GetSystemMenu(IntPtr hWnd, bool bRevert);

        [DllImport("user32.dll")]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll")]
        public static extern Boolean SetWindowPos(IntPtr hWnd,  IntPtr hWndInsertAfter, int x, int y,  int cx, int cy, uint uFlags );

        [DllImport("user32.dll")]
        static extern int GetMenuItemCount(IntPtr hMenu);

        [DllImport("user32.dll")]
        static extern bool RemoveMenu(IntPtr hMenu, uint uPosition, uint uFlags);

        private const Int32 MF_BYPOSITION = 0x400;
        private const Int32 MF_REMOVE = 0x1000;

        private bool m_xWasRemoved = false;

        public ClientLoader()
        {
            m_clientProcess = null;
            m_clientLoadInfo = null;
        }

        private IntPtr GetClientWindowHandle()
        {
            while(true)
            {
                foreach (Process clientProcess in Process.GetProcessesByName("player"))
                {
                    IntPtr hWnd = clientProcess.MainWindowHandle;
                    if (hWnd != IntPtr.Zero && clientProcess.Id == m_clientProcess.Id)
                    {
                        return hWnd;
                    }
                }
                Thread.Sleep(1000);
            }
        }

        private void RemoveCloseButton(IntPtr hWnd)
        {
            if(m_xWasRemoved)
            {
                return;
            }

            int n = 0;
            IntPtr hMenu = GetSystemMenu(hWnd, false);

            if (hMenu != IntPtr.Zero)
            {
                n = GetMenuItemCount(hMenu);
                if (n > 0)
                {
                    RemoveMenu(hMenu, (uint)(n - 1), MF_BYPOSITION | MF_REMOVE);
                    RemoveMenu(hMenu, (uint)(n - 2), MF_BYPOSITION | MF_REMOVE);
                    DrawMenuBar(hWnd);
                    m_xWasRemoved = true;
                }
            }
        }

        public void MoveClientWindow(int x, int y)
        {
            if(!Options.GetLinkToEditorState() || !IsRunning())
            {
                return;
            }

            IntPtr hWnd = GetClientWindowHandle();

            WINDOWPLACEMENT windowParams = new WINDOWPLACEMENT();
            GetWindowPlacement(hWnd, ref windowParams);
            int actualWidth = windowParams.rcNormalPosition.Width - windowParams.rcNormalPosition.X;
            int actualHeight = windowParams.rcNormalPosition.Height - windowParams.rcNormalPosition.Y;
            windowParams.rcNormalPosition = new System.Drawing.Rectangle(
                x - actualWidth,
                y,
                x,
                y + actualHeight);

            SetWindowPlacement(hWnd, ref windowParams);    
        }

        public void AddExitedReceiver(EventHandler receiver)
        {
            if(m_clientProcess != null && m_clientProcess.EnableRaisingEvents)
            {
                m_clientProcess.Exited += receiver;
            }
        }

        public void RemoveExitedReceivers(EventHandler receiver)
        {
            if (m_clientProcess != null && m_clientProcess.EnableRaisingEvents)
            {
                m_clientProcess.Exited -= receiver;
            }
        }

        public void Start(int posX, int posY)
        {
            IConfigSource playerIni = new IniConfigSource(Options.GetClientPath() + "\\player.ini");
            if (playerIni.Configs["run"] == null)
            {
                playerIni.AddConfig("run");
            }
            playerIni.Configs["run"].Set("fast", "true");
            playerIni.Save();
            
            m_clientLoadInfo = new ProcessStartInfo(Options.GetClientPath() + "\\player.exe");

            if(m_clientLoadInfo != null)
            {
                m_clientLoadInfo.UseShellExecute = true;
                m_clientLoadInfo.WorkingDirectory = Options.GetClientPath();
                m_clientLoadInfo.ErrorDialog = true;

                try
                {
                    if(m_clientProcess == null || m_clientProcess.HasExited)
                    {
                        m_clientProcess = Process.Start(m_clientLoadInfo);
                        m_clientProcess.EnableRaisingEvents = true;
                        RemoveCloseButton(GetClientWindowHandle());
                        MoveClientWindow(posX, posY);                        
                    }
                    else
                    {
                        MessageBox.Show("Клиент уже загружен.\nДля загрузки нового закройте старый", "Ошибка загрузки", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    
                }
                catch (Win32Exception w)
                {
                    MessageBox.Show("Клиент не может быть загружен.\n" + w.Message, "Ошибка загрузки", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                
            }
        }
 
        public bool Stop()
        {
            try
            {
                if(m_clientProcess != null)
                {
                    if(m_clientProcess.HasExited == false)
                    {
                        ClientEventArgs clientEvargs = new ClientEventArgs(null);
                        OnClientStopped(clientEvargs);

                        //m_clientProcess.CloseMainWindow();
                        m_clientProcess.Kill();
                    }
                    
                    return true;
                }
                else
                {
                    return false;
                }
            }
            catch(Win32Exception w)
            {
                Console.WriteLine("Client closing error: " + w.Message);
                return false;
            }            
        }

        public bool IsRunning()
        {
            string[] cmdArgs = Environment.GetCommandLineArgs();
            bool isDebug = ((cmdArgs.Length > 1) && (cmdArgs[1] == "-debug"));  
            if (isDebug)
            {
                return true;
            }

            if (m_clientProcess != null)
            {
                return !m_clientProcess.HasExited;
            }
            else
            {
                return false;
            }
        }
    }
}
