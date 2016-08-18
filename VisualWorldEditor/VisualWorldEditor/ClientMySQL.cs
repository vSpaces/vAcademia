using System;
using System.IO;
using System.Data;
using System.Collections;
using System.Text;
using MySql.Data.MySqlClient;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    class ClientMySQL
    {
        private string m_server = "localhost"; // настройки по-умолчанию
        private string m_dbName = "virtuniver";
        private string m_user = "root";
        private string m_password = "";

        private MySqlConnection m_connection = null;

        private static int m_tryConnect = 0;       

        public ClientMySQL()
        {
            // данные из настроек
            m_server = Options.GetHost();
            if (m_server.IndexOf("virtuniver.loc") == -1 && m_server.IndexOf("192.168.0.2") == -1 &&
               m_server.IndexOf("192.168.0.78") == -1 && m_tryConnect < 3)
            {
                //интернет сервер
                if(m_tryConnect == 0)
                {
                    ClientMySqlLogin loginForm = new ClientMySqlLogin();
                    while (m_connection == null || m_connection.State == ConnectionState.Closed)
                    {
                        if (m_tryConnect > 2)
                        {
                            break;
                        }

                        if (m_server.IndexOf("95") != -1)
                        {
                            MessageBox.Show("Редактор мира не работает с интернет-сервером. Используется клиент из папки " + Options.GetClientPath(), "MySQL connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            m_tryConnect = 3;
                        }
                        else
                        {
                            loginForm.ShowDialog();

                            m_user = Options.GetMysqlLogin();
                            m_password = Options.GetMysqlPassword();
                        }

                        Connect();
                        m_tryConnect++;
                    }

                    if (m_tryConnect > 2)
                    {
                        MessageBox.Show("Не удалось подключиться к серверу базы данных.", "MySQL connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }                   
                }
                else
                {
                    m_user = Options.GetMysqlLogin();
                    m_password = Options.GetMysqlPassword();
                }
            }

            WriteToLog(" == CONNECTED == ");
        }


        public ClientMySQL(byte serverNumber)
        {
            if(serverNumber != 0)
            {
                m_server = "192.168.0." + serverNumber.ToString();
            }

            WriteToLog(" == CONNECTED == ");
        }

        public static void WriteToLog(string s)
        {
            try
            {
                StreamWriter log;
                log = new StreamWriter("log.txt", true);
                log.WriteLine(s);
                log.Close();
            }
            catch (Exception /*e*/)
            {
            }
        }
        
        public bool IsConnected()
        {
            if(m_connection != null && m_connection.State == ConnectionState.Open)
            {
                return true;
            }
            else if (this.Connect() != null)
            {
                return true;
            }

            return false;
        }

        private MySqlConnection GetNewConnection()
        {
            string connectionStr = string.Format("Database={0};Data Source={1};User Id={2};Password={3};Pooling=no",
                                                    m_dbName,
                                                    m_server,
                                                    m_user,
                                                    m_password);

            MySqlConnection connection = new MySqlConnection(connectionStr);
            try
            {
               connection.Open();
            }
            catch (InvalidOperationException e)
            {
                connection = null;
                MessageBox.Show(e.Message, "MySQL connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (MySqlException e)
            {                
                connection = null;
                MessageBox.Show(e.Message, "MySQL connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            
            return connection;
        }

        public MySqlConnection Connect()
        {
            m_connection = GetNewConnection();
            return m_connection;
        }

        public MySqlConnection Connect(byte serverNumber)
        {
            if (serverNumber != 0)
            {
                m_server = "192.168.0." + serverNumber.ToString();
            }

            m_connection = GetNewConnection();
            return m_connection;
        }
        
        private MySqlDataReader MySQLQuery(string query)
        {
            lock (this)
            {
                if(!this.IsConnected())
                {
                    return null;
                }

                WriteToLog("MySQLQuery " + query);
                MySqlCommand command = m_connection.CreateCommand();
                command.CommandText = query;
                command.CommandType = CommandType.Text;

                try
                {
                    MySqlDataReader queryData = command.ExecuteReader();

                    return queryData;
                }
                catch (MySqlException e)
                {
                    MessageBox.Show(e.Message, "MySQL Query Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return null;
                }
            }
        }

        // возвращает результат запроса в виде массива
        public ArrayList QueryA(string query)
        {
            lock (this)
            {
                WriteToLog("MySQLQueryA " + query);

                MySqlDataReader queryResult = MySQLQuery(query);
            
                if (queryResult == null)
                {
                    return null;
                }

                if (queryResult.HasRows == false)
                {
                    queryResult.Close();
                    return null;
                }

                ArrayList rows = new ArrayList();

                while (queryResult.Read())
                {
                    Hashtable row = new Hashtable();

                    for (int i = 0; i < queryResult.FieldCount; i++)
                    {
                        row.Add(queryResult.GetName(i), queryResult.GetValue(i));
                    }

                    rows.Add(row);
                }

                queryResult.Close();

                return rows;
            }
        }

        public int NonQuery(string query)
        {
            lock (this)
            {
                if (!this.IsConnected())
                {
                    return 0;
                }

                WriteToLog("NonQuery " + query);
                MySqlCommand command = m_connection.CreateCommand();
                command.CommandText = query;
                command.CommandType = CommandType.Text;

                try
                {
                    return command.ExecuteNonQuery();
                }
                catch (MySqlException e)
                {
                    MessageBox.Show(e.Message, "MySQL NonQuery Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return 0;
                }
            }
        }

    }
}
