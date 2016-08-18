using System;
using System.Collections;
using System.Text;
using System.IO;
using System.IO.Pipes;
using System.Windows.Forms;
using System.Threading;

namespace VisualWorldEditor
{
    partial class PipeServer
    {
        private NamedPipeServerStream m_pipeSteam;
        private String m_pipeName = "worldeditorpipe";
        private Boolean m_isWorking = false;

        private Thread m_connectionThread;

        private Queue m_writeQueue;
        private Queue m_readedQueue;

        private DataWriter m_idleData;

        private String[] m_cmdArgs;

        public PipeServer()
        {
            m_writeQueue = new Queue();
            m_readedQueue = new Queue();

            createm_idleData(out m_idleData);

            m_cmdArgs = Environment.GetCommandLineArgs();
        }

        public bool Run()
        {
            m_isWorking = true;
            m_connectionThread = new Thread(new ThreadStart(ServerThread));
            m_connectionThread.Start();
            m_connectionThread.IsBackground = true;
            Thread.Sleep(0);

            return true;
        }

        public bool Stop()
        {
            m_isWorking = false;
            
            if(!m_pipeSteam.IsConnected)
            {
                dummyClientConnection();
            }
            else
            {
                m_pipeSteam.Dispose();
            }

            m_connectionThread.Join();

            return true;
        }

        private void dummyClientConnection()
        {
            using (NamedPipeClientStream dummyClient = new NamedPipeClientStream(".", m_pipeName))
            {
                dummyClient.Connect();
            }
        }

        private void ServerThread()
        {
            while (m_isWorking)
            {
                Thread.Sleep(10);
                using (m_pipeSteam = new NamedPipeServerStream(m_pipeName, PipeDirection.InOut, 3, PipeTransmissionMode.Byte, PipeOptions.None))                
                {
                    try
                    {
                        m_pipeSteam.WaitForConnection();                        
                        try
                        {
                            if(m_cmdArgs.Length > 1 && m_cmdArgs[1] == "-debug")
                            {                                
                                addHelloCmd();
                            }
                            else
                            {
#if DEBUG
                                addHelloCmd();
#endif
                            }
                            bool lastReadResult = false;
                            int cnt = 0;
                            while (m_isWorking)
                            {
                                if (!lastReadResult)
                                {
                                    Thread.Sleep(1);
                                }
                                // запись данных в пайп
                                writingPipe();
                                
                                // чтение и сохранение данных из пайпа
                                lastReadResult = readingPipe();

                                DataReader cmd = getNextCommand();

                                int oldCnt = cnt;

                                while (cmd != null)
                                {
                                    cnt++;
                                    
                                    // зажигание события получения новой команды
                                    CommandAddedEventArgs evargs = new CommandAddedEventArgs(cmd);
                                    OnCommandAdded(evargs);

                                    cmd = getNextCommand();                                    
                                }

                                Console.WriteLine("GetCommands: " + (cnt - oldCnt).ToString());
   
                                if (!m_pipeSteam.IsConnected)
                                {
                                    m_pipeSteam.Close();
                                    break;
                                }
                            }
                        }
                        catch (IOException e)
                        {
                            //MessageBox.Show(e.Message, "Pipe connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            Console.WriteLine("Pipe error: "+ e.Message);
                            m_pipeSteam.Close();
                            ClearQueue();
                        }                          
                    }
                    catch (InvalidOperationException e)
                    {
                        Console.WriteLine("Error: " + e.Message);
                        continue;
                    }
                }
            }
        }

        private DataReader ReadData()
        {
            //short dataSize = 0;
            int dataSize = 0;
            DataReader data = null;

            //DataReader sizeReader = new DataReader(2);
           DataReader sizeReader = new DataReader(4);

            //if(m_pipeSteam.Read(sizeReader.GetData(), 0, 2) != 2)
            if (m_pipeSteam.Read(sizeReader.GetData(), 0, 4) != 4)
            {
                return null;
            }

            if (sizeReader.Read(ref dataSize))
            {
                if (dataSize != 0)
                {
                    data = new DataReader(dataSize);
                }
                else
                {
                    return null;
                }
            }
            else
            {
                return null;
            }        

            if (m_pipeSteam.Read(data.GetData(), 0, dataSize) != dataSize)
            {
                return null;
            }

            return data;
        }
        
        private void WriteData(DataWriter data)
        {
            //short dataSize = data.GetSize();
            int dataSize = data.GetSize();
            //m_pipeSteam.Write(BitConverter.GetBytes(dataSize), 0, 2);
            m_pipeSteam.Write(BitConverter.GetBytes(dataSize), 0, 4);

            m_pipeSteam.Write(data.GetData(), 0 ,dataSize);       
        }
       
        public DataReader getNextCommand()
        {
            if(m_readedQueue.Count == 0)
            {
                return null;
            }
            return (DataReader)m_readedQueue.Dequeue();            
        }

        public void addCommand(DataWriter data)
        {
            if(data != null)
            {
                m_writeQueue.Enqueue(data);
            }            
        }

        private bool readingPipe()
        {
            int count = 0;
            DataReader sizeReader = new DataReader(4);
            
            if (m_pipeSteam.Read(sizeReader.GetData(), 0, 4) != 4)
            {
                return false;
            }

            if (sizeReader.Read(ref count))
            {
                for (int packetNum = 0; packetNum < count; packetNum++)
                {
                    DataReader data = ReadData();
                    m_readedQueue.Enqueue(data);
                }

                return true;
            }
            else
            {
                return false;
            }
        }

        private void writingPipe()
        {
            if(m_writeQueue.Count != 0)
            {
                WriteData((DataWriter)m_writeQueue.Dequeue());
            }
            else
            {
                WriteData(m_idleData);
            }
        }

        public void addHelloCmd()
        {
            DataWriter helloData = new DataWriter();
            byte helloByte = Commander.CMD_HELLO; // 2
            helloData.AddData(helloByte);
            addCommand(helloData);
        }

        private void createm_idleData(out DataWriter data)
        {
            byte idleByte = Commander.CMD_IDLE; // 1
            data = new DataWriter();
            data.AddData(idleByte);
        }

        private void ClearQueue()
        {
            m_readedQueue.Clear();
            m_writeQueue.Clear();
        }

        public bool IsClientConnected()
        {
            if (m_pipeSteam != null)
            {
                return m_pipeSteam.IsConnected;
            }
            else
            {
                return false;
            }
        }
    }
}
