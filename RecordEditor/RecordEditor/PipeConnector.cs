using System;
using System.IO;
using System.Text;
using System.Linq;
using System.IO.Pipes;
using System.Threading;
using System.Collections;
using System.Collections.Generic;

namespace RecordEditor
{
    partial class PipeConnector
    {
        private NamedPipeServerStream m_pipeReadStream;
        private NamedPipeServerStream m_pipeWriteStream;
        private String m_readPipeName = "VirtualAcademiaRecordEditorPipe_read";
        private String m_writePipeName = "VirtualAcademiaRecordEditorPipe_write";

        private Thread m_connectionReadThread;
        private Thread m_connectionWriteThread;
        private Thread m_handleCommandThread;

        private Queue m_writeQueue;
        private Queue m_readedQueue;

        private bool m_isWorking = false;

        public PipeConnector()
        {
            m_writeQueue = new Queue();
            m_readedQueue = new Queue();
        }

        public void Run()
        {
            m_isWorking = true;

            m_connectionReadThread = new Thread(new ThreadStart(ConnectReadThread));
            m_connectionReadThread.Start();
            m_connectionReadThread.IsBackground = true;

            m_connectionWriteThread = new Thread(new ThreadStart(ConnectWriteThread));
            m_connectionWriteThread.Start();
            m_connectionWriteThread.IsBackground = true;

            m_handleCommandThread = new Thread(new ThreadStart(HandleCommandThread));
            m_handleCommandThread.Start();
            m_handleCommandThread.IsBackground = true;

            Thread.Sleep(0);
        }

        public bool Stop()
        {
            m_isWorking = false;

            ClearQueue();

/*            if (!m_pipeReadStream.IsConnected)
                dummyClientConnection();
            else
                m_pipeReadStream.Dispose();

            if (!m_pipeWriteStream.IsConnected)
                dummyClientConnection();
            else
                m_pipeWriteStream.Dispose();*/

            if (m_pipeReadStream.IsConnected)
                m_pipeReadStream.Dispose();

            if (m_pipeWriteStream.IsConnected)
                m_pipeWriteStream.Dispose();

            m_connectionReadThread.Join();
            m_connectionWriteThread.Join();

            if (m_handleCommandThread.ThreadState != System.Threading.ThreadState.Running)
                m_connectionWriteThread.Join();

            return true;
        }

        private void dummyClientConnection()
        {
/*            using (NamedPipeClientStream dummyClient = new NamedPipeClientStream(".", m_pipeName))
            {
                dummyClient.Connect();
            }*/
        }

        private void ProcessExeption(IOException e)
        {
            Console.WriteLine("Pipe error: " + e.Message);
            m_pipeReadStream.Close();
            m_pipeWriteStream.Close();
            ClearQueue();
        }

        private void HandleCommandThread()
        {
            while (m_isWorking)
            {
                Thread.Sleep(50);
                DataReader cmd = getInCommand();

                if (cmd == null)
                    continue;

                byte commandId = new Byte();
                if (!cmd.Read(ref commandId))
                    return;

                if (commandId == Command.CMD_ON_HANDSHAKE)
                {
                    ClearQueue();
                    continue;
                }

                Program.context.messageHandlerManager.HandleMessage(commandId, cmd);
            }

        }

        private void ConnectReadThread()
        {
            while (m_isWorking)
            {
                Thread.Sleep(50);
                using (m_pipeReadStream = new NamedPipeServerStream(m_readPipeName, PipeDirection.In, 3, PipeTransmissionMode.Byte, PipeOptions.None))
                {
                    try
                    {
                        m_pipeReadStream.WaitForConnection();

                        try
                        {
                            addHandshakeCmd();

                            while (m_isWorking)
                            {
                                Thread.Sleep(1);

                                // чтение и сохранение данных из пайпа
                                readingPipe();

                                if (!m_pipeReadStream.IsConnected)
                                {
                                    m_pipeReadStream.Close();
                                    break;
                                }
                            }
                        }
                        catch (IOException e)
                        {
                            ProcessExeption(e);
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

        private void ConnectWriteThread()
        {
            while (m_isWorking)
            {
                Thread.Sleep(50);
                using (m_pipeWriteStream = new NamedPipeServerStream(m_writePipeName, PipeDirection.Out, 3, PipeTransmissionMode.Byte, PipeOptions.None))
                {
                    try
                    {
                        m_pipeWriteStream.WaitForConnection();

                        try
                        {
                            while (m_isWorking)
                            {
                                // запись данных в пайп
                                writingPipe();

                                if (!m_pipeWriteStream.IsConnected)
                                {
                                    m_pipeWriteStream.Close();
                                    break;
                                }
                                Thread.Sleep(50);
                            }
                        }
                        catch (IOException e)
                        {
                            ProcessExeption(e);
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
            int dataSize = 0;
            DataReader data = null;

            DataReader sizeReader = new DataReader(4);
            if (m_pipeReadStream.Read(sizeReader.GetData(), 0, 4) != 4)
                return null;

            if (sizeReader.Read(ref dataSize))
            {
                if (dataSize != 0)
                    data = new DataReader(dataSize);
                else
                    return null;
            }
            else
                return null;

            if (m_pipeReadStream.Read(data.GetData(), 0, dataSize) != dataSize)
                return null;

            return data;
        }

        private void WriteData(DataWriter data)
        {
            int dataSize = data.GetSize();
            m_pipeWriteStream.Write(BitConverter.GetBytes(dataSize), 0, 4);

            m_pipeWriteStream.Write(data.GetData(), 0, dataSize);
        }

        public DataReader getInCommand()
        {
            lock (m_readedQueue)
            {
                if (m_readedQueue.Count == 0)
                {
                    return null;
                }
                return (DataReader)m_readedQueue.Dequeue();
            }
        }

        public void addOutCommand(DataWriter data)
        {
            lock (m_writeQueue)
            {
                if (data != null)
                {
                    m_writeQueue.Enqueue(data);
                }
            }
        }

        private void readingPipe()
        {
            DataReader inCommand = ReadData();
            lock (m_readedQueue)
            {
                m_readedQueue.Enqueue(inCommand);
            }
        }

        private void writingPipe()
        {
            DataWriter outCommand = null;
            lock (m_writeQueue)
            {
                if (m_writeQueue.Count != 0)
                    outCommand = (DataWriter)m_writeQueue.Dequeue();
            }
            if (outCommand != null)
                WriteData(outCommand);
        }

        private void addHandshakeCmd()
        {
            DataWriter handshakeData = new DataWriter();
            handshakeData.AddData((byte)Command.CMD_HANDSHAKE);
            handshakeData.AddData((int)0);
            addOutCommand(handshakeData);
        }

        public void ClearQueue()
        {
            lock (m_readedQueue)
            {
                m_readedQueue.Clear();
            }
            lock (m_writeQueue)
            {
                m_writeQueue.Clear();
            }
        }

        public bool IsClientConnected()
        {
            return m_pipeReadStream.IsConnected && m_pipeWriteStream.IsConnected;
        }
    }

}
