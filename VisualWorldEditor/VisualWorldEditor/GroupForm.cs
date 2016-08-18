using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows;
using System.Collections;

namespace VisualWorldEditor
{
    public partial class GroupForm : Form
    {
        private int groupId;
        private int objectId;
        private Vector point; 

        private const int GR_LECTURE_TYPE = 5;
        private const int GR_PIVOT_TYPE = 6;

        private const String GR_LECTURE_NAME = "Аудитория";
        private const String GR_PIVOT_NAME = "Точка привязки";
        private const String GR_NONE_NAME = "Нет группы";

        private ClientMySQL m_mySql;
        private Commander m_commander;
        private HistoryProcessor m_storage;

        
        public GroupForm()
        {
            groupId = -1;
            InitializeComponent();

            m_mySql = new ClientMySQL();
            m_mySql.Connect();
            m_commander = Commander.GetCommander();
            point = new Vector();
            m_storage = HistoryProcessor.GetHProcessor();  
        }

        public void SetGroupName(String name)
        {
            groupNameTextBox.Text = name;
        }

        public String GetGroupName()
        {
            return groupNameTextBox.Text;
        }

        public void SetGroupId(int id)
        {
            groupId = id;
        }

        public int GetGroupId()
        {
            return groupId;
        }

        public void SetGroupType(int type)
        {
            int index = 0;
            if (type == GR_LECTURE_TYPE) index = groupTypeComboBox.FindString(GR_LECTURE_NAME);
            else index = groupTypeComboBox.FindString(GR_PIVOT_NAME);

            groupTypeComboBox.SelectedIndex = index;
        }

        public int GetGroupType()
        {
            if (groupTypeComboBox.SelectedItem.ToString() == GR_LECTURE_NAME) return GR_LECTURE_TYPE;
            else return GR_PIVOT_TYPE;
        }

        public void SetObjectId(int id)
        {
            objectId = id;
        }

        public Vector GetCoordinate()
        {
            return point;
        }

        public void SetCoordinate(Vector p)
        {
            point.X = p.X;
            point.Y = p.Y;
            point.Z = p.Z;
        }

        public int GetObjectId()
        {
            return objectId;
        }

        private void GroupForm_Shown(object sender, EventArgs e)
        {
              
            int index=0;
            if (GetGroupType() == GR_LECTURE_TYPE) index = groupTypeComboBox.FindString(GR_LECTURE_NAME);
            else index = groupTypeComboBox.FindString(GR_PIVOT_NAME);

            groupTypeComboBox.SelectedIndex = index;
        }

        private void OK_Click(object sender, EventArgs e)
        {
            String queryStr = null;
            ArrayList aResult = new ArrayList();

            if (GetGroupName() != "")
            {
                //если мы создаем новую группу
                if (groupId == -1)
                {
                    queryStr = "SELECT * FROM `objects_map` WHERE `name`='" + GetGroupName() + "'";
                    aResult = m_mySql.QueryA(queryStr);

                    //если такой группы еще нет
                    if (aResult == null)
                    {
                        queryStr = "SELECT * FROM `object_groups` WHERE `id_object`='" + objectId + "'";
                        aResult = m_mySql.QueryA(queryStr);

                        //если объект не имеет группы
                        if (aResult == null)
                        {
                            CreateGroupSql(GetGroupType());

                            queryStr = "INSERT INTO `object_groups` (`id_group`,`id_object`) VALUES ('" + groupId + "','" + GetObjectId() + "')";
                            m_mySql.NonQuery(queryStr);

                            SendGroupCommand();

                        }
                        else//у объекта была другая группа
                        {
                            if (MessageBox.Show("Вы действительно хотите изменить группу объекта?", "Подтверждение изменения группы объекта", MessageBoxButtons.OKCancel) == DialogResult.OK)
                            {
                                CreateGroupSql(GetGroupType());

                                queryStr = "UPDATE `object_groups` SET `id_group`='" + groupId + "' WHERE `id_object`='" + objectId + "' ";
                                m_mySql.NonQuery(queryStr);

                                SendGroupCommand();
                            }
                        }
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Такая группа уже существует", "Предупреждение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
                else//редактируем группу
                {
                    queryStr = "SELECT * FROM `objects_map`  WHERE `name`='" + GetGroupName() + "' AND `id`<> "+ GetGroupId()+" ";
                    aResult = m_mySql.QueryA(queryStr);

                    if (aResult==null)
                    {
                        queryStr = "UPDATE `objects_map` SET `type`='" + GetGroupType() + "', `name`='" + GetGroupName() + "' WHERE `id`='" + groupId + "' ";
                        m_mySql.NonQuery(queryStr);

                        SendGroupCommand();

                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Такая группа уже существует", "Предупреждение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
            }
            else
            {
                MessageBox.Show("Заполните все поля", "Предупреждение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void CreateGroupSql(int type)
        {
            String queryStr = "LOCK TABLES  `objects_map` WRITE";
            m_mySql.NonQuery(queryStr);

            // id группы
            queryStr = "SELECT MAX(id) as `max_id` FROM `objects_map` WHERE `id`<1000000";
            ArrayList aResult = m_mySql.QueryA(queryStr);
            groupId = int.Parse(((Hashtable)aResult[0])["max_id"].ToString())+1;

            ulong sqId = m_storage.GetZoneID(point.X, point.Y);

            queryStr = "INSERT INTO `objects_map` (`id`,`type`,`name`,`px`,`py`,`pz`,`sqId`) VALUES ('" + groupId + "','" + type + "','" + GetGroupName() + "','" + m_storage.WorldToZoneCoords(point.X) + "','" + m_storage.WorldToZoneCoords(point.Y) + "','" +point.Z+ "', '"+sqId+"')";
            m_mySql.NonQuery(queryStr);
            
            queryStr = "UNLOCK TABLES";
            m_mySql.NonQuery(queryStr);
        }

        private void groupNameTextBox_TextChanged(object sender, EventArgs e)
        {
            SetGroupName(GetGroupName());
        }

        private void groupTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (groupTypeComboBox.SelectedItem.ToString() == GR_LECTURE_NAME) SetGroupType(GR_LECTURE_TYPE);
            else SetGroupType(GR_PIVOT_TYPE);
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            if (groupId == -1) SetGroupName(GR_NONE_NAME);
            this.Close();
        }

        private void GroupForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (groupId == -1) SetGroupName(GR_NONE_NAME);
        }

        private void SendGroupCommand()
        {
            //привязка объекта к группе
            m_commander.SetGroup(GetGroupName(),GetGroupType());
            //добавление/изменение группы
            m_commander.SendGroupChanges(GetGroupName(), GetGroupType(),GetCoordinate());
        }


    }
}
