using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Windows;
using MySql.Data.MySqlClient;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

namespace VisualWorldEditor
{
    public partial class mainForm : Form, IMessageFilter
    {
        private String m_selectedItem = "";

        private Options optionsDialog;
        private AboutEditorBox about;
        private MaterialForm materialDialog;
        private LibraryAddingForm libAddingDialog;
        private GroupForm groupDialog;

        private PropGrid propGrid;

        private Commander m_commander;
        private ClientLoader m_client;

        private ToolTip m_toolTip;

        private static byte m_currentServer = 0;

        private ArrayList curObject;

        private int m_objectId = -1;
        private int m_groupId = -1;
        private Vector m_point;

        private const int GR_LECTURE_TYPE = 5;

        private const String GR_NONE_NAME = "Нет группы";
        private const String GR_MY_NAME = "Своя группа";
        private const int NO_GR_TYPE = 0;

        private Boolean m_undoFlag = false;

        private ClientMySQL m_mySql;

        private string m_captionBase;


        public mainForm()
        {
            this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width - this.Size.Width, 0);

            InitializeComponent();

            BindScrollsToTBox();

            optionsDialog = new Options();
            materialDialog = new MaterialForm();
            groupDialog = new GroupForm();

            libAddingDialog = new LibraryAddingForm();

            about = new AboutEditorBox();

            propGrid = new PropGrid();

            m_mySql = new ClientMySQL();
            m_mySql.Connect();

            m_point = new Vector();

            m_commander = Commander.GetCommander(this, materialDialog);
            m_client = new ClientLoader();

            m_client.ClientStopped += new ClientStoppedEventHandler(ClientExiting);

            m_commander.WorkStateChanged += new CommanderWorkStateChangedEventHandler(ChangeFormOnCommanderWorkState);
            m_commander.WorkState = Commander.WorkStates.ClientNotWorking;

            curObject = new ArrayList();

            if (CheckServerForInternetUpdate()) // в командер
            {
                this.CopyToInternetToolStripMenuItem.Enabled = true;
                this.CopyToInternetToolStripMenuItem.ToolTipText = "";
            }

            ServerPath.GetServerPath(Options.GetHost(), ref m_currentServer);
            Options.SetCurrentServerNumber(m_currentServer);
            if (m_currentServer == 0)
            {
                this.CopyTo78ToolStripMenuItem1.Enabled = true;
                this.CopyTo77ToolStripMenuItem.Enabled = true;
            }
            else if (m_currentServer == 2)
            {
                this.CopyTo78ToolStripMenuItem1.Enabled = true;
            }
            else if (m_currentServer == 78)
            {
                this.CopyTo77ToolStripMenuItem.Enabled = true;
            }

            m_toolTip = new ToolTip();
            BindToolTips(m_toolTip);

            CreateContextMenu();

            //заполняем список групп
            GroupBox_DataFill();

            Object3D.GetObject3D().ModifyLevelChanged += new ObjectEditedEventHandler(SetModifyLevelControls);

            Application.AddMessageFilter(this);

            m_captionBase = this.Text;

            UpdateCaption();
        }


        private void UpdateCaption()
        {
            this.Text = m_captionBase + " [сервер: " + m_currentServer.ToString() + "]";
        }

        public bool PreFilterMessage(ref Message m)
        {
            if (m.Msg == 0x20a)
            {
                return true;
            }

            return false;
        }

        private delegate void ActiveTextureChangedDelegate();
        public void ActiveTextureChanged()
        {
            if (this.InvokeRequired)
            {
                ActiveTextureChangedDelegate activeTextureChanged = new ActiveTextureChangedDelegate(ActiveTextureChanged);
                this.BeginInvoke(activeTextureChanged);
            }
            else
            {
                SetActiveTextureName(m_commander.GetActiveTextureName());
                materialDialog.SelectTexture(m_commander.GetActiveTextureName(), m_commander.GetActiveTextureLOD());
            }
        }

        public void SetActiveTextureName(string name)
        {
            ActiveTextureName.Text = name;
        }

        private void GetSelectedCategory(out int categoryId, out int subCategoryId)
        {
            categoryId = -1;
            subCategoryId = -1;
            EditorTreeNode node = (EditorTreeNode)CategoryComboBox.TreeDropDown.SelectedNode;
            if (node.Level == 0)
            {
                categoryId = node.GetId();
            }
            else if (node.Level == 1)
            {
                subCategoryId = node.GetId();
            }
        }

        static string m_lastErrorObjectName = "";
        static long m_lastErrorTicks = 0;

        static public void ShowObjectLockError(string name)
        {
            // Выдаем ошибку, что объект защищен от изменения свойств, если выполняется одно из 2 условий:
            // 1. имя ошибочного объекта отличается от имени прошлого ошибочного объекта
            // 2. прошло больше 20 секунд
            if ((name != m_lastErrorObjectName) || (DateTime.Now.Ticks - m_lastErrorTicks > 20 * 10000000))
            {
                m_lastErrorTicks = DateTime.Now.Ticks;

                MessageBox.Show("Объект " + name + " защищен от изменений свойств.", "Защита", MessageBoxButtons.OK, MessageBoxIcon.Information);
                m_lastErrorObjectName = name;
            }
        }

        private void CreateContextMenu()
        {
            // меню для категорий/подкатегорий
            CategoryComboBox.TreeDropDown.MouseUp += new MouseEventHandler(CategoryComboBoxTreeView_MouseUp);
            ContextMenuStrip treeNodeContextMenu = new ContextMenuStrip();
            ToolStripMenuItem deleteCategoryItem = new ToolStripMenuItem("Удалить", global::VisualWorldEditor.Properties.Resources.Delete_16x16);
            deleteCategoryItem.Click += delegate(Object sender, EventArgs e)
            {
                int categoryId, subCategoryId;
                GetSelectedCategory(out categoryId, out subCategoryId);

                m_commander.DeleteCategoryFromLib(categoryId, subCategoryId);
                CategoryDataFill();
            };

            ToolStripMenuItem copyCategoryItem = new ToolStripMenuItem("Копировать на..", global::VisualWorldEditor.Properties.Resources.Copy_16x16);
            ToolStripMenuItem copyCategoryTo77 = new ToolStripMenuItem("2-й сервер");
            copyCategoryTo77.Click += delegate(Object sender, EventArgs e)
            {
                int categoryId, subCategoryId;
                GetSelectedCategory(out categoryId, out subCategoryId);

                m_commander.CopyLibCategoryTo(categoryId, subCategoryId, 2);
            };

            ToolStripMenuItem copyCategoryTo78 = new ToolStripMenuItem("78-й сервер");
            copyCategoryTo78.Click += delegate(Object sender, EventArgs e)
            {
                int categoryId, subCategoryId;
                GetSelectedCategory(out categoryId, out subCategoryId);

                m_commander.CopyLibCategoryTo(categoryId, subCategoryId, 78);
            };

            ToolStripMenuItem copyCategoryToOwn = new ToolStripMenuItem("cвой сервер");
            copyCategoryToOwn.Click += delegate(Object sender, EventArgs e)
            {
                int categoryId, subCategoryId;
                GetSelectedCategory(out categoryId, out subCategoryId);

                m_commander.CopyLibCategoryTo(categoryId, subCategoryId, 0);
            };

            copyCategoryItem.DropDownItems.Add(copyCategoryTo77);
            copyCategoryItem.DropDownItems.Add(copyCategoryTo78);
            copyCategoryItem.DropDownItems.Add(copyCategoryToOwn);

            treeNodeContextMenu.Items.Add(copyCategoryItem);
            treeNodeContextMenu.Items.Add(deleteCategoryItem);

            CategoryComboBox.TreeDropDown.ContextMenuStrip = treeNodeContextMenu;

            // меню для объектов
            objectsList.MouseUp += new MouseEventHandler(ObjectsList_MouseUp);
            ContextMenuStrip objectsListContextMenu = new ContextMenuStrip();
            ToolStripMenuItem deleteObjectItem = new ToolStripMenuItem("Удалить", global::VisualWorldEditor.Properties.Resources.Delete_16x16);
            deleteObjectItem.Click += delegate(Object sender, EventArgs e)
            {
                EditorTreeNode objectInfo = (EditorTreeNode)objectsList.SelectedItem;
                if (objectInfo != null)
                {
                    m_commander.DeleteObjectFromLib(objectInfo.GetId());
                    CategoryDataFill();
                }
            };

            ToolStripMenuItem copyObjectItem = new ToolStripMenuItem("Копировать на..", global::VisualWorldEditor.Properties.Resources.Copy_16x16);
            ToolStripMenuItem copyObjectTo77 = new ToolStripMenuItem("2-й сервер");
            copyObjectTo77.Click += delegate(Object sender, EventArgs e)
            {
                EditorTreeNode objectInfo = (EditorTreeNode)objectsList.SelectedItem;
                if (objectInfo != null)
                {
                    m_commander.CopyLibObjectTo(objectInfo.GetId(), 2);
                }
            };

            ToolStripMenuItem copyObjectTo78 = new ToolStripMenuItem("78-й сервер");
            copyObjectTo78.Click += delegate(Object sender, EventArgs e)
            {
                EditorTreeNode objectInfo = (EditorTreeNode)objectsList.SelectedItem;
                if (objectInfo != null)
                {
                    m_commander.CopyLibObjectTo(objectInfo.GetId(), 78);
                }
            };

            ToolStripMenuItem copyObjectToOwn = new ToolStripMenuItem("cвой сервер");
            {
                EditorTreeNode objectInfo = (EditorTreeNode)objectsList.SelectedItem;
                if (objectInfo != null)
                {
                    m_commander.CopyLibObjectTo(objectInfo.GetId(), 0);
                }
            };

            copyObjectItem.DropDownItems.Add(copyObjectTo77);
            copyObjectItem.DropDownItems.Add(copyObjectTo78);
            copyObjectItem.DropDownItems.Add(copyObjectToOwn);

            objectsListContextMenu.Items.Add(copyObjectItem);
            objectsListContextMenu.Items.Add(deleteObjectItem);

            if (m_currentServer == 0)
            {
                copyCategoryToOwn.Enabled = false;
                copyObjectToOwn.Enabled = false;
            }
            else if (m_currentServer == 2)
            {
                copyCategoryTo77.Enabled = false;
                copyObjectTo77.Enabled = false;
            }
            else if (m_currentServer == 78)
            {
                copyCategoryTo78.Enabled = false;
                copyObjectTo78.Enabled = false;
            }

            objectsList.ContextMenuStrip = objectsListContextMenu;

            // меню для лодов
            ContextMenuStrip lodListContextMenu = new ContextMenuStrip();
            ToolStripMenuItem createLod = new ToolStripMenuItem("Создать лод..");
            ToolStripMenuItem editLod = new ToolStripMenuItem("Редактировать лод");
            ToolStripMenuItem viewTexturesLod = new ToolStripMenuItem("Посмотреть текстуры лода");
            ToolStripMenuItem viewInfoLod = new ToolStripMenuItem("Посмотреть информацию о лоде");
            ToolStripMenuItem deleteLod = new ToolStripMenuItem("Удалить лод");
            ToolStripMenuItem createLodAsSelected = new ToolStripMenuItem("на основе выделенного");
            ToolStripMenuItem createLodAsObject = new ToolStripMenuItem("на основе объекта");

            viewTexturesLod.Click += delegate(Object sender, EventArgs e)
            {
                CheckedListBox lodList = shadowsCheckList;
                if (lodList == null)
                {
                    return;
                }

                if (lodList.SelectedItem == null)
                {
                    return;
                }

                string path = ((Object3D.LodShadow)lodList.SelectedItem).meshName;
                path = ServerPath.GetServerPath(Options.GetHost()) + ServerPath.geometryDir + path;
                path = path.Replace("\\\\", "/");
                path = path.Replace("\\", "/");
                path = path.Replace("//", "/");
                path = "/" + path;
                path = path.Replace("mesh.bmf.zip", "");

                Process process = new Process();
                ProcessStartInfo loadInfo = new ProcessStartInfo("textures_viewer.exe");
                loadInfo.Arguments = path;
                loadInfo.UseShellExecute = true;
                loadInfo.WorkingDirectory = "";
                process = Process.Start(loadInfo);
            };


            viewInfoLod.Click += delegate(Object sender, EventArgs e)
            {
                CheckedListBox lodList = shadowsCheckList;

                if (lodList == null)
                {
                    return;
                }

                if (lodList.SelectedItem == null)
                {
                    return;
                }

                string path = ((Object3D.LodShadow)lodList.SelectedItem).meshName;

                m_commander.SendModelInfoQuery(path);
            };

            createLodAsSelected.Click += delegate(Object sender, EventArgs e)
            {
                if (Object3D.GetObject3D().ModifyLevel == 0 ||
                    Object3D.GetObject3D().ModifyLevel == 2)
                {
                    CheckedListBox lodList = shadowsCheckList;

                    if (lodList == null)
                    {
                        return;
                    }

                    if (lodList.SelectedItem == null)
                    {
                        return;
                    }

                    int lodIndex = lodList.SelectedIndex;
                    bool shadowState = lodList.GetItemChecked(lodIndex);

                    m_commander.CreateLod(((Object3D.LodShadow)lodList.SelectedItem).distance, shadowState, ((Object3D.LodShadow)lodList.SelectedItem).meshName);
                }
                else
                {
                    mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                }
            };

            deleteLod.Click += delegate(Object sender, EventArgs e)
            {
                if (Object3D.GetObject3D().ModifyLevel == 0 ||
                    Object3D.GetObject3D().ModifyLevel == 2)
                {
                    CheckedListBox lodList = shadowsCheckList;

                    if (lodList == null)
                    {
                        return;
                    }

                    if (lodList.SelectedItem == null)
                    {
                        return;
                    }

                    int lodIndex = lodList.SelectedIndex;

                    m_commander.DeleteLod(lodIndex);
                }
                else
                {
                    mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                }
            };


            editLod.Click += delegate(Object sender, EventArgs e)
            {
                if (Object3D.GetObject3D().ModifyLevel == 0 ||
                    Object3D.GetObject3D().ModifyLevel == 2)
                {
                    CheckedListBox lodList = shadowsCheckList;

                    if (lodList == null)
                    {
                        return;
                    }

                    if (lodList.SelectedItem == null)
                    {
                        return;
                    }

                    int lodIndex = lodList.SelectedIndex;
                    m_commander.EditLod(lodIndex);
                }
                else
                {
                    mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                }
            };

            createLodAsObject.Click += delegate(Object sender, EventArgs e)
            {
                if (Object3D.GetObject3D().ModifyLevel == 0 ||
                    Object3D.GetObject3D().ModifyLevel == 2)
                {
                    m_commander.AddLodLikeObject();
                }
                else
                {
                    mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                }
            };

            createLod.DropDownItems.Add(createLodAsSelected);
            createLod.DropDownItems.Add(createLodAsObject);
            lodListContextMenu.Items.Add(viewTexturesLod);
            lodListContextMenu.Items.Add(viewInfoLod);
            lodListContextMenu.Items.Add(createLod);
            lodListContextMenu.Items.Add(editLod);
            lodListContextMenu.Items.Add(deleteLod);

            shadowsCheckList.ContextMenuStrip = lodListContextMenu;
        }

        private void BindToolTips(ToolTip toolTip)
        {
            toolTip.ShowAlways = true;

            toolTip.SetToolTip(searchImage, "Поиск и выделение объектов");
            toolTip.SetToolTip(addToLibButton, "Добавить выделенный объект в библиотеку");
        }

        private bool CheckServerForInternetUpdate()
        {
            byte serverNumber = 0;
            ServerPath.GetServerPath(Options.GetHost(), ref serverNumber);

            if (serverNumber == 2)
            {
                return true;
            }
            return false;
        }

        private void BindScrollsToTBox()
        {
            EditorNumVScroll txTBoxScroll = new EditorNumVScroll(txTBox);
            txTBoxScroll.ScrollStep = 20;
            moveGroup.Controls.Add(txTBoxScroll);
            txTBoxScroll.BringToFront();
            EditorNumVScroll tyTBoxScroll = new EditorNumVScroll(tyTBox);
            tyTBoxScroll.ScrollStep = 20;
            moveGroup.Controls.Add(tyTBoxScroll);
            tyTBoxScroll.BringToFront();
            EditorNumVScroll tzTBoxScroll = new EditorNumVScroll(tzTBox);
            tzTBoxScroll.ScrollStep = 20;
            moveGroup.Controls.Add(tzTBoxScroll);
            tzTBoxScroll.BringToFront();

            EditorNumVScroll sxTBoxScroll = new EditorNumVScroll(sxTBox);
            scaleGroup.Controls.Add(sxTBoxScroll);
            sxTBoxScroll.BringToFront();
            EditorNumVScroll syTBoxScroll = new EditorNumVScroll(syTBox);
            scaleGroup.Controls.Add(syTBoxScroll);
            syTBoxScroll.BringToFront();
            EditorNumVScroll szTBoxScroll = new EditorNumVScroll(szTBox);
            scaleGroup.Controls.Add(szTBoxScroll);
            szTBoxScroll.BringToFront();

            EditorNumVScroll rxTBoxScroll = new EditorNumVScroll(rxTBox);
            rotateGroup.Controls.Add(rxTBoxScroll);
            rxTBoxScroll.BringToFront();
            EditorNumVScroll ryTBoxScroll = new EditorNumVScroll(ryTBox);
            rotateGroup.Controls.Add(ryTBoxScroll);
            ryTBoxScroll.BringToFront();
            EditorNumVScroll rzTBoxScroll = new EditorNumVScroll(rzTBox);
            rotateGroup.Controls.Add(rzTBoxScroll);
            rzTBoxScroll.BringToFront();
        }

        private void mainForm_Load(object sender, EventArgs e)
        {

        }

        private void exitProcedure()
        {
            m_client.Stop();
            m_commander.Stop();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void mainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            exitProcedure();

            Material.DeleteThreads();
        }

        private void CategoryDataFill()
        {
            objectsList.Enabled = false;
            objectsList.ClearSelected();
            objectsList.Items.Clear();
            CategoryComboBox.Value = "";
            CategoryComboBox.TreeDropDown.Nodes.Clear();

            ArrayList categoryRows = m_commander.GetCategoryList();


            if (categoryRows == null)
            {
                return;
            }


            foreach (Hashtable catRow in categoryRows)
            {
                TreeNode parentNode = new EditorTreeNode();
                parentNode.Text = (String)catRow["name"];

                EditorTreeNode categoryNode = (EditorTreeNode)parentNode;
                categoryNode.SetId((Int32)(UInt32)catRow["id"]);
                categoryNode.SetName((String)catRow["name"]);

                ArrayList subcategoryRows = m_commander.GetSubCategoryList(categoryNode.GetId());

                if (subcategoryRows != null)
                {
                    foreach (Hashtable subcatRow in subcategoryRows)
                    {
                        TreeNode childNode = new EditorTreeNode();
                        childNode.Text = (String)subcatRow["name"];
                        EditorTreeNode subcategoryNode = (EditorTreeNode)childNode;
                        subcategoryNode.SetId((Int32)(UInt32)subcatRow["id"]);
                        subcategoryNode.SetName((String)subcatRow["name"]);

                        parentNode.Nodes.Add(childNode);
                    }
                }

                CategoryComboBox.TreeDropDown.Nodes.Add(parentNode);
            }
        }

        private void GroupBox_DataFill()
        {

            GroupComboBox.Items.Clear();

            GroupComboBox.Items.Add(GR_NONE_NAME);
            GroupComboBox.Items.Add(GR_MY_NAME);

            ArrayList groupRows = m_commander.GetGroupList();

            if (groupRows == null)
            {
                return;
            }

            foreach (Hashtable grRow in groupRows)
            {
                GroupComboBox.Items.Add(grRow["name"]);

            }

        }


        private void CategoryComboBox_DataFill(object sender, UtilityLibrary.Combos.TreeCombo.EventArgsTreeDataFill e)
        {
            CategoryDataFill();
        }

        private void CategoryComboBox_ValueChanged(object sender, EventArgs e)
        {
            UtilityLibrary.Combos.TreeCombo categoryTree = (UtilityLibrary.Combos.TreeCombo)sender;

            EditorTreeNode selectedNode = (EditorTreeNode)categoryTree.TreeDropDown.SelectedNode;

            objectsList.ClearSelected();
            objectsList.Items.Clear();
            objectsList.Enabled = false;

            if (selectedNode == null)
            {
                objectsList.Items.Add("Выберите категорию");
                return;
            }
            else
            {
                m_selectedItem = selectedNode.GetName();
            }

            if (selectedNode.Level < 1)
            {
                objectsList.Items.Add("Выберите подкатегорию");
            }
            else
            {
                ArrayList objectsRows = m_commander.GetObjectsList(selectedNode.GetId());

                if (objectsRows == null)
                {
                    objectsList.Items.Add("Нет объектов");
                }
                else
                {
                    foreach (Hashtable objRow in objectsRows)
                    {
                        EditorTreeNode objectNode = new EditorTreeNode();
                        objectNode.SetId((Int32)(UInt32)objRow["id"]);
                        objectNode.SetSubId((Int32)(UInt32)objRow["id_subcategory"]);
                        //objectNode.SetName((String)objRow["name"]);
                        objectNode.SetName((String)objRow["name_display"]);
                        objectNode.SetResourse((String)objRow["res"]);
                        objectNode.Text = objectNode.GetName();

                        //objectsList.Items.Add(objectsData.GetString(2));
                        objectsList.Items.Add(objectNode);
                        //objectsList.Items.Add(selectedNode.IsSelected.ToString());
                    }

                    if (m_commander.WorkState != Commander.WorkStates.ClientNotWorking)
                    {
                        objectsList.Enabled = true;
                    }
                }
            }
        }

        private void CategoryComboBox_DropDownHided(object sender, EventArgs e)
        {
            UtilityLibrary.Combos.TreeCombo categoryTree = (UtilityLibrary.Combos.TreeCombo)sender;
            if (categoryTree.Value == "")
            {
                categoryTree.Value = m_selectedItem;
            }

            categoryTree.TreeDropDown.ContextMenuStrip.Hide();
        }

        private void CategoryComboBox_DropDownShown(object sender, EventArgs e)
        {
            UtilityLibrary.Combos.TreeCombo categoryTree = (UtilityLibrary.Combos.TreeCombo)sender;
            m_selectedItem = categoryTree.Value;
            categoryTree.Value = "";
        }

        private void objectsList_SelectedIndexChanged(object sender, EventArgs e)
        {
            /*ListBox objList = (ListBox)sender;
            EditorTreeNode selectedObject = (EditorTreeNode)objList.SelectedItem;
            if(selectedObject != null)
            {
                m_commander.AddObjectToWorld(selectedObject.GetId());               
            } */
        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            sendDeleteCommand();
        }

        private void stopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string[] cmdArgs = Environment.GetCommandLineArgs();
            bool isDebug = ((cmdArgs.Length > 1) && (cmdArgs[1] == "-debug"));
            if (isDebug)
            {
                ClientExiting(null, null);
            }
            if ((m_client.Stop()) || (isDebug))
            {
                // do some
                m_commander.WorkState = Commander.WorkStates.ClientNotWorking;
            }
            if (isDebug)
            {
                Environment.Exit(0);
            }
        }

        private void startToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.SendHelloCommand();

            m_client.Start(this.Location.X, this.Location.Y);
            m_client.AddExitedReceiver(new EventHandler(ClientExited));

            m_commander.WorkState = Commander.WorkStates.NoSelect;

            byte serverNumber = 0;
            ServerPath.GetServerPath(Options.GetHost(), ref serverNumber);
            m_currentServer = serverNumber;
            Options.SetCurrentServerNumber(m_currentServer);
            UpdateCaption();
        }

        private delegate void ClientExitedDelegate(object sender, EventArgs args);
        private void ClientExited(object sender, EventArgs args)
        {
            if (this.InvokeRequired)
            {
                ClientExitedDelegate clientExited = new ClientExitedDelegate(ClientExited);
                this.BeginInvoke(clientExited, new object[] { null, null });
            }
            else
            {
                ClientExiting(null, null);
            }
        }


        private void btnMaterial_Click(object sender, EventArgs e)
        {
            OpenMaterialDialog();
        }

        private delegate void OpenMaterialDialogDelegate();
        public void OpenMaterialDialog()
        {
            if (this.InvokeRequired)
            {
                OpenMaterialDialogDelegate openMaterialDialog = new OpenMaterialDialogDelegate(OpenMaterialDialog);
                this.BeginInvoke(openMaterialDialog);
            }
            else
            {
                materialDialog.Show();
                materialDialog.Location = new System.Drawing.Point(this.Location.X - materialDialog.Size.Width,
                    this.Location.Y);
            }
        }

        private void ClientExiting(Object sender, ClientEventArgs args)
        {
            m_commander.WorkState = Commander.WorkStates.ClientNotWorking;
            m_commander.Save(true); // перед закрытием сохранимся
            Object3D.GetObject3D().DeleteInfo();
            m_client.RemoveExitedReceivers(new EventHandler(ClientExited));
        }

        private void OptionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            optionsDialog.ShowDialog();
        }

        private void AboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            about.ShowDialog();
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.Undo();
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.Redo();
        }

        private void SaveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //сохранение
            m_commander.Save(false);
        }

        private void sendDeleteCommand()
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                m_commander.DeleteObject();
            }
            else
            {
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
            }
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            sendDeleteCommand();
        }

        private void objectNameBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                TextBox objectNameTextBox = (TextBox)sender;

                if (!(char.IsControl(e.KeyChar) || char.IsLetterOrDigit(e.KeyChar) || e.KeyChar == '_'))
                {
                    e.Handled = true;
                }

                if (e.KeyChar == (char)Keys.Return && objectNameTextBox.Text != "")
                {
                    DialogResult res = MessageBox.Show("Вы действительно хотите переименовать объект?", "Запрос подтверждения", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    if (res == DialogResult.Yes)
                    {
                        m_commander.RenameObject(objectNameTextBox.Text);
                    }
                    else
                    {
                        objectNameTextBox.Text = Object3D.GetObject3D().Names[0];
                    }
                    e.Handled = true;
                }
            }
            else
            {
                e.Handled = false;
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
            }
        }

        private void PerformSearch()
        {
            if (m_client.IsRunning())
            {
                String mask = FindBox.Text;
                if (ExactSearch.Checked == false)
                    if (!mask.EndsWith("*"))
                    {
                        mask += "*";
                    }
                m_commander.SelectByMask(mask);
            }
            else
            {
                MessageBox.Show("Клиент не запущен.", "Предупреждение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void FindBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            TextBox findObjectBox = (TextBox)sender;

            if (e.KeyChar == (char)Keys.Return && findObjectBox.Text != "")
            {
                PerformSearch();
                e.Handled = true;
            }
        }

        private void sendSelectSameCommand()
        {
            m_commander.SelectSameObjects();
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            m_commander.SendObjectCreateCommand(true);
        }

        private void btnSelectSame_Click(object sender, EventArgs e)
        {
            sendSelectSameCommand();
        }

        private void transformTBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || char.IsControl(e.KeyChar) || e.KeyChar == '.' || e.KeyChar == '-'))
            {
                e.Handled = true;
            }

            if (e.KeyChar == (char)Keys.Return)
            {
                if (sendMoveCommand())
                {
                    e.Handled = true;
                }
                else
                {
                    e.Handled = false;
                }
            }
        }

        private bool sendMoveCommand()
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                float moveX, moveY, moveZ;

                if (!float.TryParse(txTBox.Text, out moveX) ||
                    !float.TryParse(tyTBox.Text, out moveY) ||
                    !float.TryParse(tzTBox.Text, out moveZ))
                {
                    return false;
                }

                Vector move = new Vector(moveX, moveY, moveZ);
                m_commander.MoveObject(move);

                if (mDeltaLabel.Visible)
                {
                    txTBox.Text = "0";
                    tyTBox.Text = "0";
                    tzTBox.Text = "0";
                }
            }
            else
            {
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                return false;
            }

            return true;
        }

        private void rotateTBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || char.IsControl(e.KeyChar) || e.KeyChar == '.' || e.KeyChar == '-'))
            {
                e.Handled = true;
            }

            if (e.KeyChar == (char)Keys.Return)
            {
                if (sendRotateCommand())
                {
                    e.Handled = true;
                }
                else
                {
                    e.Handled = false;
                }

            }
        }

        private void rotateTBox_Leave(object sender, EventArgs e)
        {
            TextBox rotateTBox = (TextBox)sender;
            if (rotateTBox.Text == "")
            {
                rotateTBox.Text = "0";
                return;
            }

            if (rotateTBox.Text != "0")
            {
                if (!sendRotateCommand())
                {
                    rotateTBox.Text = "0";
                    return;
                }
            }
        }

        private bool sendRotateCommand()
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                float angleX, angleY, angleZ;

                if (!float.TryParse(rxTBox.Text, out angleX) ||
                    !float.TryParse(ryTBox.Text, out angleY) ||
                    !float.TryParse(rzTBox.Text, out angleZ))
                {
                    return false;
                }

                Vector rotateAngles = new Vector(angleX, angleY, angleZ);
                m_commander.RotateObject(rotateAngles);

                if (rDeltaLabel.Visible)
                {
                    rxTBox.Text = "0";
                    ryTBox.Text = "0";
                    rzTBox.Text = "0";
                }
            }
            else
            {
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                return false;
            }

            return true;
        }

        private void scaleTBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(char.IsDigit(e.KeyChar) || char.IsControl(e.KeyChar) || e.KeyChar == '.' || e.KeyChar == '-'))
            {
                e.Handled = true;
            }

            if (e.KeyChar == (char)Keys.Return)
            {
                if (sendScaleCommand())
                {
                    e.Handled = true;
                }
                else
                {
                    e.Handled = false;
                }
            }
        }

        private bool sendScaleCommand()
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                float scaleX, scaleY, scaleZ;

                if (!float.TryParse(sxTBox.Text, out scaleX) ||
                    !float.TryParse(syTBox.Text, out scaleY) ||
                    !float.TryParse(szTBox.Text, out scaleZ))
                {
                    return false;
                }

                if (scaleX <= 0 || scaleY <= 0 || scaleZ <= 0)
                {
                    if (scaleX <= 0)
                    {
                        sxTBox.Text = "1";
                    }
                    if (scaleY <= 0)
                    {
                        syTBox.Text = "1";
                    }
                    if (scaleZ <= 0)
                    {
                        szTBox.Text = "1";
                    }

                    return false;
                }

                Vector scale = new Vector(scaleX, scaleY, scaleZ);
                m_commander.ScaleObject(scale);

                if (sDeltaLabel.Visible)
                {
                    sxTBox.Text = "0";
                    syTBox.Text = "0";
                    szTBox.Text = "0";
                }
            }
            else
            {
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                return false;
            }

            return true;
        }

        private void shadowsCheckList_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                CheckedListBox checkList = (CheckedListBox)sender;

                int lodIndex = e.Index;
                int shadowState = 0;
                if (e.NewValue == CheckState.Checked)
                {
                    shadowState = 1;
                }

                CheckedListBox lodList = shadowsCheckList;

                m_commander.SetShadow(lodIndex, shadowState, ((Object3D.LodShadow)lodList.SelectedItem).meshName);
            }
            else
            {
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                e.NewValue = e.CurrentValue;
            }
        }

        private void btnAddToLib_Click(object sender, EventArgs e)
        {
            String categoryName = "";
            String subcategoryName = "";
            String nameDisplay = "";

            DialogResult result = libAddingDialog.ShowAddingDialog(
                    m_commander.GetLibStructure(),
                    ref categoryName,
                    ref subcategoryName,
                    ref nameDisplay);

            if (result == DialogResult.OK)
            {
                Console.WriteLine(categoryName + "; " + subcategoryName + "; " + nameDisplay);
                m_commander.AddSelectedObjectToLibrary(categoryName, subcategoryName, nameDisplay);
            }
        }

        private void mainForm_LocationChanged(object sender, EventArgs e)
        {
            RelinkClientWindow();

            materialDialog.Location = new System.Drawing.Point(this.Location.X - materialDialog.Size.Width,
                                this.Location.Y);
        }

        private void CopyToInternetToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenSynchPage();
        }

        private delegate void OpenSynchPageDelegate();
        public void OpenSynchPage()
        {
            if (this.InvokeRequired)
            {
                OpenSynchPageDelegate openSynchPage = new OpenSynchPageDelegate(OpenSynchPage);
                this.BeginInvoke(openSynchPage);
            }
            else
            {
                string url = @"http://www.virtuniver.loc/synch/login.php";
                Process.Start("rundll32.exe", "url.dll,FileProtocolHandler \"" + url + "\"");
            }
        }

        private delegate void ChangeFreezingDelegate();
        public void ChangeFreezing()
        {
            if (this.InvokeRequired)
            {
                ChangeFreezingDelegate changeFreezing = new ChangeFreezingDelegate(ChangeFreezing);
                this.BeginInvoke(changeFreezing);
            }
            else
            {
                if (freeCamToolStripMenuItem.Checked == false)
                {
                    freezAvatarToolStripMenuItem.Checked = !freezAvatarToolStripMenuItem.Checked;
                }
            }
        }

        private void freezAvatarToolStripMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_commander.SendFreezCommand(freezAvatarToolStripMenuItem.Checked);
        }

        private delegate void ChangeFreeCamDelegate();
        public void ChangeFreeCam()
        {
            if (this.InvokeRequired)
            {
                ChangeFreeCamDelegate changeFreeCam = new ChangeFreeCamDelegate(ChangeFreeCam);
                this.BeginInvoke(changeFreeCam);
            }
            else
            {
                freeCamToolStripMenuItem.Checked = !freeCamToolStripMenuItem.Checked;
            }
        }

        private void freeCamToolStripMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            if (freeCamToolStripMenuItem.Checked)
            {
                freezAvatarToolStripMenuItem.Checked = true;
                freezAvatarToolStripMenuItem.Enabled = false;
            }
            else
            {
                freezAvatarToolStripMenuItem.Enabled = true;
                freezAvatarToolStripMenuItem.Checked = false;
            }

            m_commander.SendFreeCamCommand(freeCamToolStripMenuItem.Checked);
        }

        private void refreshToolStripButton_Click(object sender, EventArgs e)
        {
            m_commander.RefreshObjects();
        }

        //////////////////////////////////////////////////////////////////////////
        // Form methods
        #region "Forms methods"

        bool collisionUpdatedAfterObjectSelect = false;

        private delegate void SetCollisionFlagDelegate(int collisionFlag);
        public void SetCollisionFlag(int collisionFlag)
        {
            if (this.InvokeRequired)
            {
                SetCollisionFlagDelegate setCollisionFlag = new SetCollisionFlagDelegate(SetCollisionFlag);
                this.BeginInvoke(setCollisionFlag, collisionFlag);
            }
            else
            {
                this.collisionUpdatedAfterObjectSelect = true;
                this.CollisionTypeComboBox.SelectedIndex = collisionFlag % 100;
                this.collisionUpdatedAfterObjectSelect = false;
            }
        }

        private delegate void SetObjectNameDelegate(String objectName, bool readOnly);
        public void SetObjectName(String objectName, bool readOnly)
        {
            if (this.InvokeRequired)
            {
                SetObjectNameDelegate setObjectName = new SetObjectNameDelegate(SetObjectName);
                this.BeginInvoke(setObjectName, new object[] { objectName, readOnly });
            }
            else
            {
                this.objectNameBox.Text = objectName;


                if (Object3D.GetObject3D().ModifyLevel == 1 ||
                   (Object3D.GetObject3D().ModifyLevel == 3))
                {
                    ChangeDragMode(0, global::VisualWorldEditor.Properties.Resources.select_16x16);
                }


                if (objectName == "")
                {
                    this.objectNameBox.ReadOnly = true;
                    this.rxTBox.Text = "";
                    this.ryTBox.Text = "";
                    this.rzTBox.Text = "";

                    this.rxTBox.Enabled = false;
                    this.ryTBox.Enabled = false;
                    this.rzTBox.Enabled = false;
                }
                else
                {
                    this.rxTBox.Enabled = true;
                    this.ryTBox.Enabled = true;
                    this.rzTBox.Enabled = true;

                    this.rxTBox.Text = "0";
                    this.ryTBox.Text = "0";
                    this.rzTBox.Text = "0";

                    if (readOnly)
                    {
                        this.objectNameBox.ReadOnly = true;
                    }
                    else
                    {
                        this.objectNameBox.ReadOnly = false;
                    }

                    //информация об объекте
                    ArrayList objectRows = m_commander.GetCurObjectORGroupInfo(objectName);

                    if (objectRows == null)
                    {
                        return;
                    }

                    if (int.Parse((((Hashtable)objectRows[0])["level"]).ToString()) >= 100)
                        SwitchOffPathes.Checked = true;
                    else
                        SwitchOffPathes.Checked = false;

                    m_objectId = int.Parse((((Hashtable)objectRows[0])["id"]).ToString());
                    groupDialog.SetObjectId(m_objectId);

                    //определение группы объекта
                    ArrayList grRows = m_commander.GetCurGroup(m_objectId.ToString());

                    if (grRows == null)
                    {
                        GroupComboBox.SelectedIndex = 0;
                    }
                    else
                    {
                        String curGroup = (String)(((Hashtable)grRows[0])["name"]).ToString();

                        int index = GroupComboBox.FindString(curGroup);
                        GroupComboBox.SelectedIndex = index;

                    }

                }
            }
        }

        private delegate void SetServerDelegate(String serverURL);
        public void SetServer(String serverURL)
        {
            if (this.InvokeRequired)
            {
                SetServerDelegate setServer = new SetServerDelegate(SetServer);
                this.BeginInvoke(setServer, new object[] { serverURL });
            }
            else
            {
                ServerPath.GetServerPath(serverURL, ref m_currentServer);
                Options.SetCurrentServerNumber(m_currentServer);
                SetNewConnectionSettings();
            }
        }

        public void ChangeServer(String serverURL)
        {
            SetServer(serverURL);
        }

        public void SetObjectName(String objectName)
        {
            SetObjectName(objectName, false);
        }

        private delegate String GetObjectNameDelegate();
        public String GetObjectName()
        {
            if (this.InvokeRequired)
            {
                GetObjectNameDelegate getObjectName = new GetObjectNameDelegate(GetObjectName);
                return (String)this.EndInvoke(this.BeginInvoke(getObjectName));
            }
            else
            {
                return this.objectNameBox.Text;
            }
        }

        private delegate void SetUndoMenuStateDelegate(bool state);
        public void SetUndoMenuState(bool state)
        {
            if (this.InvokeRequired)
            {
                SetUndoMenuStateDelegate setUndoMenuState = new SetUndoMenuStateDelegate(SetUndoMenuState);
                this.BeginInvoke(setUndoMenuState, new object[] { state });
            }
            else
            {
                this.undoToolStripMenuItem.Enabled = state;
            }
        }

        private delegate void SetRedoMenuStateDelegate(bool state);
        public void SetRedoMenuState(bool state)
        {
            if (this.InvokeRequired)
            {
                SetRedoMenuStateDelegate setRedoMenuState = new SetRedoMenuStateDelegate(SetRedoMenuState);
                this.BeginInvoke(setRedoMenuState, new object[] { state });
            }
            else
            {
                this.redoToolStripMenuItem.Enabled = state;
            }
        }

        private delegate void SetTransformTBoxDelegate(Vector value);
        public void SetTransformTBox(Vector value)
        {
            if (this.InvokeRequired)
            {
                SetTransformTBoxDelegate setTransformTBox = new SetTransformTBoxDelegate(SetTransformTBox);
                this.BeginInvoke(setTransformTBox, new object[] { value });
            }
            else
            {
                if (!Vector.IsNaN(value))
                {
                    this.mDeltaLabel.Visible = false;

                    this.txTBox.Enabled = true;
                    this.tyTBox.Enabled = true;
                    this.tzTBox.Enabled = true;

                    this.txTBox.Text = value.X.ToString("F3");
                    this.tyTBox.Text = value.Y.ToString("F3");
                    this.tzTBox.Text = value.Z.ToString("F3");
                }
                else
                {
                    this.txTBox.Text = "";
                    this.tyTBox.Text = "";
                    this.tzTBox.Text = "";

                    if (Object3D.GetObject3D().Names.Length == 0)
                    {
                        this.txTBox.Enabled = false;
                        this.tyTBox.Enabled = false;
                        this.tzTBox.Enabled = false;
                        this.mDeltaLabel.Visible = false;
                    }
                    else
                    {
                        this.txTBox.Enabled = true;
                        this.tyTBox.Enabled = true;
                        this.tzTBox.Enabled = true;

                        this.txTBox.Text = "0";
                        this.tyTBox.Text = "0";
                        this.tzTBox.Text = "0";

                        this.mDeltaLabel.Visible = true;
                    }
                }
            }
        }

        private delegate void SetScaleTBoxDelegate(Vector value);
        public void SetScaleTBox(Vector value)
        {
            if (this.InvokeRequired)
            {
                SetScaleTBoxDelegate setScaleTBox = new SetScaleTBoxDelegate(SetScaleTBox);
                this.BeginInvoke(setScaleTBox, new object[] { value });
            }
            else
            {
                if (!Vector.IsNaN(value))
                {
                    this.sDeltaLabel.Visible = false;
                    this.sxTBox.Enabled = true;
                    this.syTBox.Enabled = true;
                    this.szTBox.Enabled = true;

                    this.sxTBox.Text = value.X.ToString("F3");
                    this.syTBox.Text = value.Y.ToString("F3");
                    this.szTBox.Text = value.Z.ToString("F3");
                }
                else
                {
                    this.sxTBox.Text = "";
                    this.syTBox.Text = "";
                    this.szTBox.Text = "";

                    if (Object3D.GetObject3D().Names.Length == 0)
                    {
                        this.sxTBox.Enabled = false;
                        this.syTBox.Enabled = false;
                        this.szTBox.Enabled = false;
                        this.sDeltaLabel.Visible = false;
                    }
                    else
                    {
                        this.sxTBox.Enabled = true;
                        this.syTBox.Enabled = true;
                        this.szTBox.Enabled = true;

                        this.sxTBox.Text = "0";
                        this.syTBox.Text = "0";
                        this.szTBox.Text = "0";

                        this.sDeltaLabel.Visible = true;
                    }
                }
            }
        }

        private delegate void SetClientStatDelegate(String clientStat);
        public void SetClientStat(String clientStat)
        {
            if (this.InvokeRequired)
            {
                SetClientStatDelegate setClientStat = new SetClientStatDelegate(SetClientStat);
                this.BeginInvoke(setClientStat, new object[] { clientStat });
            }
            else
            {
                this.clientStatLabel.Text = clientStat;
            }
        }

        /*private delegate bool IsStatPageOpenedDelegate();
        public bool IsStatPageOpened()
        {
            if(this.InvokeRequired)
            {
                IsStatPageOpenedDelegate isStatPageOpened = new IsStatPageOpenedDelegate(IsStatPageOpened);
                return (bool)this.EndInvoke(this.BeginInvoke(isStatPageOpened));
            }
            else
            {
                return (EditorTabs.SelectedTab == PageClientStat);
            }            
        }*/

        private delegate void SetShadowsListDelegate(ArrayList shadowsList);
        public void SetShadowsList(ArrayList shadowsList)
        {
            if (this.InvokeRequired)
            {
                SetShadowsListDelegate setShadowsList = new SetShadowsListDelegate(SetShadowsList);
                this.BeginInvoke(setShadowsList, new object[] { shadowsList });
            }
            else
            {
                shadowsCheckList.ItemCheck -= new ItemCheckEventHandler(shadowsCheckList_ItemCheck);
                shadowsCheckList.ClearSelected();
                shadowsCheckList.Items.Clear();

                foreach (Object3D.LodShadow lodInfo in shadowsList)
                {
                    shadowsCheckList.Items.Add(lodInfo);
                    if (lodInfo.shadow == 1)
                    {
                        shadowsCheckList.SetItemChecked(shadowsCheckList.Items.Count - 1, true);
                    }
                }
                shadowsCheckList.ItemCheck += new ItemCheckEventHandler(shadowsCheckList_ItemCheck);
            }
        }

        private delegate void RelinkClientWindowDelegate();
        public void RelinkClientWindow()
        {
            if (this.InvokeRequired)
            {
                RelinkClientWindowDelegate relinkClientWindow = new RelinkClientWindowDelegate(RelinkClientWindow);
                this.BeginInvoke(relinkClientWindow);
            }
            else
            {
                m_client.MoveClientWindow(this.Location.X, this.Location.Y);
            }
        }

        private delegate void AddNewSegmentToPathDelegate(ArrayList pointsList);
        public void AddNewSegmentToPath(ArrayList pointsList)
        {
            if (this.InvokeRequired)
            {
                AddNewSegmentToPathDelegate addNewSegmentToPath = new AddNewSegmentToPathDelegate(AddNewSegmentToPath);
                this.BeginInvoke(addNewSegmentToPath, new object[] { pointsList });
            }
            else
            {
                CheckedListBox lodList = shadowsCheckList;

                string path = ((Object3D.LodShadow)lodList.Items[0]).meshName;
                path = ServerPath.GetServerPath(Options.GetHost()) + ServerPath.geometryDir + path;
                path = path.Replace("\\\\", "/");
                path = path.Replace("\\", "/");
                path = path.Replace("//", "/");
                path = "/" + path;
                path = path.Replace("mesh.bmf.zip", "");

                if (File.Exists(path + "pathlines.bin"))
                {
                    WriteBackupPathFiles(path);
                }

                File.Delete(path + "mesh.ebmf");

                using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bin", FileMode.Create)))
                {
                    binWriter.Write("PL1");

                    for (int i = 0; i < pointsList.Count; i++)
                    {
                        if ((i + 1) % 7 == 0)
                        {
                            binWriter.Write((byte)pointsList[i]);
                        }
                        else
                        {
                            binWriter.Write((float)pointsList[i]);
                        }
                    }
                }

                if (File.Exists(path + "pathlines.bin"))
                {
                    DeleteRepeatsSegmentsInPath(path + "pathlines.bin");

                    byte[] pathBytes = File.ReadAllBytes(path + "pathlines.bin");
                    using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bak", FileMode.Create)))
                    {
                        binWriter.Write(pathBytes);
                    }
                }

                Boolean is3DObjectExistsPathLines = false;

                if (File.Exists(path + "mesh.ini"))
                {
                    using (StreamReader meshInfo = new StreamReader(path + "mesh.ini"))
                    {
                        String meshIn;
                        while ((meshIn = meshInfo.ReadLine()) != null)
                        {
                            if (meshIn == "pathlines.bin")
                            {
                                is3DObjectExistsPathLines = true;
                            }
                        }
                    }
                }

                if (!is3DObjectExistsPathLines)
                {
                    StreamWriter meshInfoAdd = new StreamWriter(path + "mesh.ini", true);

                    meshInfoAdd.WriteLine("pathlines.bin");
                    meshInfoAdd.Close();
                }


            }
        }


        private delegate void Update3DObjectsPathDelegate(ArrayList pointsList);
        public void Update3DObjectsPath(ArrayList pointsList)
        {
            if (this.InvokeRequired)
            {
                Update3DObjectsPathDelegate update3DObjectsPath = new Update3DObjectsPathDelegate(Update3DObjectsPath);
                this.BeginInvoke(update3DObjectsPath, new object[] { pointsList });
            }
            else
            {
                CheckedListBox lodList = shadowsCheckList;

                string path = ((Object3D.LodShadow)lodList.Items[0]).meshName;
                path = ServerPath.GetServerPath(Options.GetHost()) + ServerPath.geometryDir + path;
                path = path.Replace("\\\\", "/");
                path = path.Replace("\\", "/");
                path = path.Replace("//", "/");
                path = "/" + path;
                path = path.Replace("mesh.bmf.zip", "");

                if (File.Exists(path + "pathlines.bin"))
                {
                    WriteBackupPathFiles(path);
                }

                File.Delete(path + "mesh.ebmf");

                using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bin", FileMode.Create)))
                {
                    binWriter.Write("PL1");
                    for (int i = 0; i < pointsList.Count; i++)
                    {
                        if ((i+1) % 7 == 0) 
                        {
                            binWriter.Write((byte)pointsList[i]);
                        }
                        else
                        {
                            binWriter.Write((float)pointsList[i]);
                        }
                    }
                }

                if (File.Exists(path + "pathlines.bin"))
                {
                    DeleteRepeatsSegmentsInPath(path + "pathlines.bin");

                    byte[] pathBytes = File.ReadAllBytes(path + "pathlines.bin");
                    using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bak", FileMode.Create)))
                    {
                        binWriter.Write(pathBytes);
                    }
                }
            }
        }

        private delegate void SetObjectsGroupDelegate(String objectName, String groupName, byte isUndo);
        public void SetObjectsGroup(String objectName, String groupName, byte isUndo)
        {
            if (this.InvokeRequired)
            {
                SetObjectsGroupDelegate setObjectsGroup = new SetObjectsGroupDelegate(SetObjectsGroup);
                this.BeginInvoke(setObjectsGroup, new object[] { objectName, groupName, isUndo });
            }
            else
            {
                if (isUndo == 0)
                {
                    m_undoFlag = false;
                }
                else
                {
                    m_undoFlag = true;
                }
                if (objectName == this.objectNameBox.Text)
                {
                    if (groupName == "")
                        GroupComboBox.SelectedIndex = 0;
                    else
                    {
                        int index = GroupComboBox.FindString(groupName);
                        GroupComboBox.SelectedIndex = index;
                    }
                }

            }
        }

        private delegate void UpdateLibraryDelegate();
        public void UpdateLibrary()
        {
            if (this.InvokeRequired)
            {
                UpdateLibraryDelegate updateLibrary = new UpdateLibraryDelegate(UpdateLibrary);
                this.BeginInvoke(updateLibrary);
            }
            else
            {
                CategoryDataFill();
            }
        }

        private delegate void SetModifyLevelControlsDelegate(Object sender, ObjectEditedEventArgs args);
        private void SetModifyLevelControls(Object sender, ObjectEditedEventArgs args)
        {
            if (this.InvokeRequired)
            {
                SetModifyLevelControlsDelegate setModifyLevelControls = new SetModifyLevelControlsDelegate(SetModifyLevelControls);
                this.BeginInvoke(setModifyLevelControls, new object[] { sender, args });
            }
            else
            {
                int modifyLevel = (int)args.EditParams[0];
                if (modifyLevel == 1 || modifyLevel == 3)
                {
                    this.dragToolStripDropDownButton.Image = global::VisualWorldEditor.Properties.Resources.select_16x16;
                    this.dragToolStripDropDownButton.Enabled = false;
                    m_commander.SendNewDragMode(0);
                }
                else
                {
                    this.dragToolStripDropDownButton.Enabled = true;
                }
            }
        }

        private delegate void ChangeFormOnCommanderWorkStateDelegate(Object sender, CommanderWorkStateChangedEventArgs args);
        private void ChangeFormOnCommanderWorkState(Object sender, CommanderWorkStateChangedEventArgs args)
        {
            if (this.InvokeRequired)
            {
                ChangeFormOnCommanderWorkStateDelegate changeFormOnCommanderWorkState = new ChangeFormOnCommanderWorkStateDelegate(ChangeFormOnCommanderWorkState);
                this.BeginInvoke(changeFormOnCommanderWorkState, new object[] { sender, args });
            }
            else
            {
                Commander.WorkStates workState = args.Args;

                switch (workState)
                {
                    case Commander.WorkStates.ClientNotWorking:
                        {
                            SaveToolStripMenuItem.Enabled = false;
                            undoToolStripMenuItem.Enabled = false;
                            redoToolStripMenuItem.Enabled = false;

                            CopyToToolStripMenuItem.Enabled = false;
                            copyToolStripMenuItem.Enabled = false;
                            deleteToolStripMenuItem.Enabled = false;
                            materialToolStripMenuItem.Enabled = false;

                            freezAvatarToolStripMenuItem.Enabled = false;
                            freezAvatarToolStripMenuItem.Checked = false;

                            freeCamToolStripMenuItem.Enabled = false;
                            freeCamToolStripMenuItem.Checked = false;

                            showCollisionsToolStripMenuItem.Enabled = false;
                            showCollisionsToolStripMenuItem.Checked = false;

                            showNearestPathesToolStripMenuItem.Enabled = false;
                            showNearestPathesToolStripMenuItem.Checked = false;

                            showInvisibleCollisionsToolStripMenuItem.Enabled = false;
                            showInvisibleCollisionsToolStripMenuItem.Checked = false;

                            enableWireframeToolStripMenuItem.Enabled = false;
                            enableWireframeToolStripMenuItem.Checked = false;

                            startToolStripMenuItem.Enabled = true;
                            stopToolStripMenuItem.Enabled = false;

                            this.libAddingDialog.FormOff();

                            EditorTabs.SelectedTab = PageLibrary;
                            PageProperties.Enabled = false;
                            PageClientStat.Enabled = false;

                            objectsList.Enabled = false;

                            FindBox.Enabled = false;

                            addToLibButton.Enabled = false;

                            TeleportToolStripMenuItem.Enabled = false;

                            dragToolStripDropDownButton.Image = global::VisualWorldEditor.Properties.Resources.select_16x16;


                            break;
                        }
                    case Commander.WorkStates.NoSelect:
                        {
                            SaveToolStripMenuItem.Enabled = true;


                            CopyToToolStripMenuItem.Enabled = false;
                            copyToolStripMenuItem.Enabled = false;
                            deleteToolStripMenuItem.Enabled = false;
                            materialToolStripMenuItem.Enabled = false;

                            freezAvatarToolStripMenuItem.Enabled = true;

                            freeCamToolStripMenuItem.Enabled = true;

                            TeleportToolStripMenuItem.Enabled = false;

                            showCollisionsToolStripMenuItem.Enabled = true;
                            showInvisibleCollisionsToolStripMenuItem.Enabled = true;

                            enableWireframeToolStripMenuItem.Enabled = true;

                            this.libAddingDialog.FormOff();

                            startToolStripMenuItem.Enabled = false;
                            stopToolStripMenuItem.Enabled = true;
                            PageProperties.Enabled = false;
                            PageClientStat.Enabled = true;
                            FindBox.Enabled = true;

                            addToLibButton.Enabled = false;

                            btnPropGrid.Enabled = false;

                            if (CategoryComboBox.TreeDropDown.SelectedNode != null)
                            {
                                CategoryComboBox.Value = "";
                                CategoryComboBox.Value = ((EditorTreeNode)CategoryComboBox.TreeDropDown.SelectedNode).GetName();
                            }

                            break;
                        }
                    case Commander.WorkStates.OneSelect:
                        {
                            TeleportToolStripMenuItem.Enabled = true;

                            freezAvatarToolStripMenuItem.Enabled = true;
                            freeCamToolStripMenuItem.Enabled = true;
                            showCollisionsToolStripMenuItem.Enabled = true;
                            showInvisibleCollisionsToolStripMenuItem.Enabled = true;
                            enableWireframeToolStripMenuItem.Enabled = true;
                            startToolStripMenuItem.Enabled = false;
                            stopToolStripMenuItem.Enabled = true;

                            showNearestPathesToolStripMenuItem.Enabled = true;

                            CopyToToolStripMenuItem.Enabled = true;
                            copyToolStripMenuItem.Enabled = true;

                            deleteToolStripMenuItem.Enabled = true;

                            materialToolStripMenuItem.Enabled = true;
                            materialToolStripButton.Enabled = true;

                            shadowsGroup.Enabled = true;

                            selectSameToolStripButton.Enabled = true;

                            objectNameBox.ReadOnly = false;

                            PageProperties.Enabled = true;

                            addToLibButton.Enabled = true;

                            btnPropGrid.Enabled = true;

                            FindBox.Enabled = true;

                            ActiveTextureName.Text = m_commander.GetActiveTextureName();

                            break;
                        }
                    case Commander.WorkStates.MoreSelect:
                        {
                            TeleportToolStripMenuItem.Enabled = true;

                            CopyToToolStripMenuItem.Enabled = true;
                            copyToolStripMenuItem.Enabled = true;

                            deleteToolStripMenuItem.Enabled = true;

                            materialToolStripMenuItem.Enabled = false;
                            materialToolStripButton.Enabled = false;

                            shadowsGroup.Enabled = false;

                            selectSameToolStripButton.Enabled = false;

                            this.libAddingDialog.FormOff();

                            objectNameBox.ReadOnly = true;

                            PageProperties.Enabled = true;

                            addToLibButton.Enabled = false;

                            btnPropGrid.Enabled = true;

                            ActiveTextureName.Text = m_commander.GetActiveTextureName();

                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }

        private void copyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String controlType = this.ActiveControl.GetType().Name;
            if (controlType == "TextBox" || controlType == "EditorTextBox")
            {
                // выделен ли текст
                TextBox tBox = (TextBox)this.ActiveControl;
                if (tBox.SelectedText != "")
                {
                    Clipboard.SetText(tBox.SelectedText);
                    return;
                }
            }

            m_commander.SendObjectCreateCommand(true);
        }

        private void objectNameBox_TextChanged(object sender, EventArgs e)
        {
            objectNameLibTBox.Text = ((TextBox)sender).Text;
        }

        private void TeleportToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.TeleportToNextSelectedObject();
        }

        private void CategoryComboBox_CloseDropDown(object sender, UtilityLibrary.Combos.CustomCombo.EventArgsCloseDropDown e)
        {
            UtilityLibrary.Combos.TreeCombo categoryTree = (UtilityLibrary.Combos.TreeCombo)sender;
            EditorTreeNode selectedNode = (EditorTreeNode)categoryTree.TreeDropDown.SelectedNode;

            if (selectedNode != null)
            {
                if (selectedNode.Level < 1 && selectedNode.Nodes.Count != 0)
                {
                    e.Close = false;
                }
            }
        }

        private void CategoryComboBoxTreeView_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                TreeView tree = (TreeView)sender;
                TreeNode node = tree.GetNodeAt(e.Location);
                if (node != null)
                {
                    tree.SelectedNode = node;
                    tree.ContextMenuStrip.Show(tree, e.Location);
                }
            }
        }

        private void ObjectsList_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                ListBox list = (ListBox)sender;
                int index = list.IndexFromPoint(e.Location);
                if (index != ListBox.NoMatches)
                {
                    list.SelectedIndexChanged -= new System.EventHandler(this.objectsList_SelectedIndexChanged);
                    list.SelectedIndex = index;
                    list.SelectedIndexChanged += new System.EventHandler(this.objectsList_SelectedIndexChanged);
                    list.ContextMenuStrip.Show(list, e.Location);
                }
            }
        }

        private void toolStripDropDownButton1_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            if ((Object3D.GetObject3D().ModifyLevel == 0) ||
                (Object3D.GetObject3D().ModifyLevel == 2))
            {
                ToolStripDropDownButton dragButton = (ToolStripDropDownButton)sender;
                dragButton.Image = e.ClickedItem.Image;
                Int32 dragMode = Int32.Parse((String)e.ClickedItem.Tag);

                if ((dragMode == 7) && (MessageBox.Show("Вы действительно хотите добавить обходной путь по прямоугольнику? Старый путь будет удален.", "Подтверждение генерации обходного пути по прямоугольнику", MessageBoxButtons.OKCancel) == DialogResult.Cancel))
                {
                    return;
                }

                if ((dragMode == 8) && (MessageBox.Show("Вы действительно хотите добавить обходной путь по окружности? Старый путь будет удален.", "Подтверждение генерации обходного пути по окружности", MessageBoxButtons.OKCancel) == DialogResult.Cancel))
                {
                    return;
                }

                if ((dragMode == 9) && (MessageBox.Show("Вы действительно хотите удалить путь у объекта?", "Подтверждение удаления обходного пути", MessageBoxButtons.OKCancel) == DialogResult.Cancel))
                {
                    return;
                }

                m_commander.SendNewDragMode(dragMode);

                if (dragMode == 7 || dragMode == 8 || dragMode == 9)
                {
                    CheckedListBox lodList = shadowsCheckList;

                    string path = ((Object3D.LodShadow)lodList.Items[0]).meshName;
                    path = ServerPath.GetServerPath(Options.GetHost()) + ServerPath.geometryDir + path;
                    path = path.Replace("\\\\", "/");
                    path = path.Replace("\\", "/");
                    path = path.Replace("//", "/");
                    path = "/" + path;
                    path = path.Replace("mesh.bmf.zip", "");


                    Boolean is3DObjectExistsPathLines = false;

                    if (File.Exists(path + "mesh.ini"))
                    {
                        ArrayList meshList = new ArrayList();

                        using (StreamReader meshInfo = new StreamReader(path + "mesh.ini"))
                        {
                            String meshIn;
                            while ((meshIn = meshInfo.ReadLine()) != null)
                            {
                                if (meshIn == "pathlines.bin")
                                {
                                    is3DObjectExistsPathLines = true;
                                }
                                else
                                {
                                    meshList.Add(meshIn);
                                }
                            }
                        }

                        if (dragMode == 9)
                        {
                            //if (meshList.Count != 0)
                            //{
                            //    StreamWriter meshInfoAdd = new StreamWriter(path + "mesh.ini", false);
                            //    for (int i = 0; i < meshList.Count; i++)
                            //    {
                            //        meshInfoAdd.WriteLine(meshList[i]);
                            //    }
                            //    meshInfoAdd.Close();
                            //}
                            //else
                            //{
                            //    File.Delete(path + "mesh.ini");
                            //}

                            if (is3DObjectExistsPathLines)
                            {
                                BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bin", FileMode.Create));
                                binWriter.Close();
                                //File.Create(path + "pathlines.bin",0);// Delete(path + "pathlines.bin");
                                File.Delete(path + "mesh.ebmf");
                            }
                        }
                    }


                    if (dragMode != 9)
                    {
                        if (!is3DObjectExistsPathLines)
                        {
                            StreamWriter meshInfoAdd = new StreamWriter(path + "mesh.ini", true);

                            meshInfoAdd.WriteLine("pathlines.bin");
                            meshInfoAdd.Close();
                        }

                        using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bin", FileMode.Create)))
                        {
                            if (dragMode == 7)
                            {
                                binWriter.Write("autogen:quad");
                            }
                            else
                            {
                                binWriter.Write("autogen:circle");
                            }

                        }
                    }
                }
            }
        }

        private void objectsList_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            ListBox objList = (ListBox)sender;
            EditorTreeNode selectedObject = (EditorTreeNode)objList.SelectedItem;
            if (selectedObject != null)
            {
                m_commander.AddObjectToWorld(selectedObject.GetId());
            }
        }

        private void btnPropGrid_Click(object sender, EventArgs e)
        {
            propGrid.ShowProperties();
        }


        private void btnExtendedSearch_Click(object sender, EventArgs e)
        {
           // propGrid.ShowProperties();
            String objName = FindBox.Text;
            if (objName!="")
            {
                m_commander.TeleportByCoordsToNextSelectedObject(objName, ExactSearch.Checked);
            }
        }

        private void CopyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String copyString = clientStatLabel.Text;
            copyString.Replace("\n", Environment.NewLine);
            //Clipboard.SetDataObject(copyString, true);
            Clipboard.SetText(copyString);
        }

        private void CopyTo77ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.CopyObjectsTo(2);
        }

        private void CopyToOurToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.CopyObjectsTo(0);
        }

        private void CopyTo78ToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            m_commander.CopyObjectsTo(78);
        }

        private void shadowsCheckList_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                CheckedListBox lodList = (CheckedListBox)sender;
                int index = lodList.IndexFromPoint(e.Location);
                if (index != ListBox.NoMatches)
                {
                    lodList.SelectedIndex = index;
                }
                else
                {
                    lodList.SelectedIndex = lodList.Items.Count - 1;
                }
            }
        }

        private void showCollisionsToolStripMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_commander.SendShowCollisionsCommand(showCollisionsToolStripMenuItem.Checked);
        }

        private void showInvisibleCollisionsToolStripMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_commander.SendShowInvisibleCollisionsCommand(showInvisibleCollisionsToolStripMenuItem.Checked);
        }

        private void showNearestPathesToolStripMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_commander.SendShowNearestPathesCommand(showNearestPathesToolStripMenuItem.Checked);
        }

        private void CollisionTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.collisionUpdatedAfterObjectSelect == true)
            {
                return;
            }

            bool canBeModified = (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2);
            if ((canBeModified) &&
                (MessageBox.Show("Вы действительно хотите изменить коллизии объекта " + Object3D.GetObject3D().Names[0] + "?", "Запрос подтверждения", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes))
            {
                int result = m_commander.SendChangeCollisionFlag(this.CollisionTypeComboBox.SelectedIndex);
                mainForm.ReloadServers();
                if (result != -2)
                {
                    this.CollisionTypeComboBox.SelectedIndex = result;
                }
            }
            else
            {
                if (!canBeModified)
                {
                    mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);
                }
                this.collisionUpdatedAfterObjectSelect = true;
                this.CollisionTypeComboBox.SelectedIndex = Object3D.GetObject3D().CollisionFlag;
                this.collisionUpdatedAfterObjectSelect = false;
            }
        }

        private void включитьWireframeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_commander.SendEnableWireframeCommand(enableWireframeToolStripMenuItem.Checked);
        }

        static public void ReloadServers(byte serverNumber)
        {
            try
            {
                Process process = new Process();
                ProcessStartInfo loadInfo = new ProcessStartInfo("controlservers_work.exe");
                loadInfo.Arguments = "-ip 192.168.0." + serverNumber.ToString() + " -port 11132 ResServer -reloadCache";
                loadInfo.UseShellExecute = true;
                loadInfo.CreateNoWindow = true;
                loadInfo.WindowStyle = ProcessWindowStyle.Hidden;
                process = Process.Start(loadInfo);

                Process process2 = new Process();
                loadInfo.Arguments = "-ip 192.168.0." + serverNumber.ToString() + " -port 11133 ObjectServer -rebootWorld";
                process2 = Process.Start(loadInfo);
            }
            catch (Exception e)
            {
                ClientMySQL.WriteToLog("Problem with run of controlservers_work.exe: " + e.ToString());
            }
        }

        static public void ReloadServers()
        {
            byte serverNumber = 0;
            ServerPath.GetServerPath(Options.GetHost(), ref serverNumber);
            m_currentServer = serverNumber;
            Options.SetCurrentServerNumber(m_currentServer);
            ReloadServers(serverNumber);
        }

        private void перегрузитьСервераToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ReloadServers();
        }

        private void FindBox_TextChanged(object sender, EventArgs e)
        {

        }

        private void searchImage_Click(object sender, EventArgs e)
        {

        }

        private void ExactSearch_CheckedChanged(object sender, EventArgs e)
        {
            PerformSearch();
        }

        public void SetNewConnectionSettings()
        {
            if (m_currentServer == 0)
            {
                CopyTo78ToolStripMenuItem1.Enabled = true;
                CopyTo77ToolStripMenuItem.Enabled = true;
                CopyToOurToolStripMenuItem.Enabled = false;
            }
            else if (m_currentServer == 2)
            {
                CopyTo78ToolStripMenuItem1.Enabled = true;
                CopyToOurToolStripMenuItem.Enabled = true;
                CopyTo77ToolStripMenuItem.Enabled = false;
            }
            else if (m_currentServer == 78)
            {
                CopyTo77ToolStripMenuItem.Enabled = true;
                CopyToOurToolStripMenuItem.Enabled = true;
                CopyTo78ToolStripMenuItem1.Enabled = false;
            }

            m_mySql.Connect(m_currentServer);
            m_commander.UpdateConnection();
            materialDialog.UpdateConnection();

            CategoryDataFill();
            GroupBox_DataFill();

            UpdateCaption();
        }

        private void сменитьСерверToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process process = new Process();
            ProcessStartInfo loadInfo = new ProcessStartInfo("server_switch.exe");
            loadInfo.WorkingDirectory = Options.GetClientPath();
            loadInfo.UseShellExecute = true;
            process = Process.Start(loadInfo);

            while (process.HasExited == false)
            {
                System.Threading.Thread.Sleep(1);
            }

            byte serverNumber = 0;
            ServerPath.GetServerPath(Options.GetHost(), ref  serverNumber);
            m_currentServer = serverNumber;
            Options.SetCurrentServerNumber(m_currentServer);

            SetNewConnectionSettings();
        }

        private void EditorTabs_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_commander.SetStatPageOpened(EditorTabs.SelectedTab == PageClientStat);
        }

        private void addToGrButton_Click(object sender, EventArgs e)
        {
            String groupSelected = GroupComboBox.SelectedItem.ToString();
            if (groupSelected != GR_NONE_NAME)
                OpenGroupDialog(groupSelected);
        }

        private void OpenGroupDialog(String groupSelected)
        {
            groupDialog.Text = "Редактирование группы";
            groupDialog.Location = new System.Drawing.Point(this.Location.X, this.Location.Y + this.Height);
            m_point.X = (float)Convert.ToDouble(txTBox.Text);
            m_point.Y = (float)Convert.ToDouble(tyTBox.Text);
            m_point.Z = (float)Convert.ToDouble(tzTBox.Text);


            if (groupSelected == GR_MY_NAME)
            {
                if (MessageBox.Show("Вы действительно хотите создать свою группу?", "Подтверждение создания новой группы", MessageBoxButtons.OKCancel) == DialogResult.OK)
                {
                    groupDialog.Text = "Создание новой группы";
                    groupDialog.SetGroupName(objectNameLibTBox.Text + "_group");
                    groupDialog.SetCoordinate(m_point);
                    groupDialog.SetGroupType(GR_LECTURE_TYPE);
                    groupDialog.SetGroupId(-1);
                    groupDialog.ShowDialog();
                    GroupBox_DataFill();

                    int index = GroupComboBox.FindString(groupDialog.GetGroupName());
                    GroupComboBox.SelectedIndex = index;
                }
            }
            else
            {
                groupDialog.SetGroupName(groupSelected);
                groupDialog.SetCoordinate(m_point);
                groupDialog.ShowDialog();
                GroupBox_DataFill();

                int index = GroupComboBox.FindString(groupDialog.GetGroupName());
                GroupComboBox.SelectedIndex = index;
            }
        }

        private int GetGroupId()
        {
            int id = -1;
            int type = 0;

            //id группы по имени в списке групп
            ArrayList grRows = m_commander.GetCurObjectORGroupInfo(GroupComboBox.SelectedItem.ToString());

            if (grRows == null)
            {
                return -1;
            }

            id = int.Parse((((Hashtable)grRows[0])["id"]).ToString());
            groupDialog.SetGroupId(id);

            type = int.Parse((((Hashtable)grRows[0])["type"]).ToString());
            groupDialog.SetGroupType(type);

            return id;
        }

        private void GroupComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            String queryStr = null;

            if (GroupComboBox.SelectedItem.ToString() == GR_MY_NAME)
            {
                OpenGroupDialog(GR_MY_NAME);
            }
            else if (GroupComboBox.SelectedItem.ToString() == GR_NONE_NAME)
            {
                queryStr = "DELETE FROM `object_groups` WHERE `id_object`='" + m_objectId + "'";
                m_mySql.NonQuery(queryStr);
                if (!m_undoFlag) m_commander.SetGroup("", NO_GR_TYPE);

            }
            else
            {
                //была ли у объекта группа
                ArrayList grRows = m_commander.GetCurGroup(m_objectId.ToString());
                m_groupId = GetGroupId();

                if (grRows != null)
                {
                    //объект с др группой
                    queryStr = "UPDATE `object_groups` SET `id_group`='" + m_groupId + "' WHERE `id_object`='" + m_objectId + "' ";
                    m_mySql.NonQuery(queryStr);

                    if (!m_undoFlag) m_commander.SetGroup(GroupComboBox.SelectedItem.ToString(), groupDialog.GetGroupType());
                }
                else
                {
                    //объект без группы
                    queryStr = "INSERT INTO `object_groups` (`id_group`,`id_object`) VALUES ('" + m_groupId + "','" + m_objectId + "')";
                    m_mySql.NonQuery(queryStr);

                    if (!m_undoFlag) m_commander.SetGroup(GroupComboBox.SelectedItem.ToString(), groupDialog.GetGroupType());

                }
            }
            m_undoFlag = false;
        }

        private delegate void ImitateKeyDownDelegate(int key);
        public void ImitateKeyDown(int key)
        {
            if (this.InvokeRequired)
            {
                ImitateKeyDownDelegate imitateKeyDown = new ImitateKeyDownDelegate(ImitateKeyDown);
                this.BeginInvoke(imitateKeyDown, key);
            }
            else
            {
                if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
                {
                    if (key == (int)System.Windows.Forms.Keys.F2)
                    {
                        ChangeDragMode(1, global::VisualWorldEditor.Properties.Resources.move_16x16);
                    }
                    else if (key == (int)System.Windows.Forms.Keys.F3)
                    {
                        ChangeDragMode(2, global::VisualWorldEditor.Properties.Resources.rotate_16x16);
                    }
                    else if (key == (int)System.Windows.Forms.Keys.F4)
                    {
                        ChangeDragMode(3, global::VisualWorldEditor.Properties.Resources.scale_16x16);
                    }
                    else if (key == (int)System.Windows.Forms.Keys.F5)
                    {
                        ChangeDragMode(4, global::VisualWorldEditor.Properties.Resources.AddSegment_16x16);
                    }
                    else if (key == (int)System.Windows.Forms.Keys.F6)
                    {
                        ChangeDragMode(5, global::VisualWorldEditor.Properties.Resources.DeleteSegment_16x16);
                    }
                    else if (key == (int)System.Windows.Forms.Keys.F7)
                    {
                        ChangeDragMode(6, global::VisualWorldEditor.Properties.Resources.RemovePoint_16x16);
                    }
                    else if (key == (int)System.Windows.Forms.Keys.F8)
                    {
                        ChangeDragMode(10, global::VisualWorldEditor.Properties.Resources.Move_avatar_16x16);
                    }
                }
            }
        }

        private void ChangeDragMode(int key, Image img)
        {
            this.dragToolStripDropDownButton.Image = img;
            m_commander.SendNewDragMode(key);

        }

        private void DeleteRepeatsSegmentsInPath(String path)
        {
            ArrayList vectors = new ArrayList();

            byte[] pathBytes = File.ReadAllBytes(path);
            int k = 0;

            byte[] typeOfSegment = new byte [(pathBytes.Length - 4)/25]; ;

            using (BinaryReader binReader = new BinaryReader(File.Open(path, FileMode.Open)))
            {
                byte[] version = binReader.ReadBytes(4);
                while (k < pathBytes.Length - 4)
                {
                    byte[] vec = binReader.ReadBytes(12);
                    vectors.Add(vec);
                    k = k + 12;
                    if (k % 25 == 24)
                    {
                        k++;
                        typeOfSegment[k / 25 - 1] = binReader.ReadByte();
                    }
                }
            }

            bool isNeedToDelete;

            using (BinaryWriter binWriter = new BinaryWriter(File.Open(path, FileMode.Create)))
            {
                for (int i = 0; i < vectors.Count; i += 2)
                {
                    if (i ==0 )
                    {
                        binWriter.Write("PL1");
                    }

                    isNeedToDelete = false;

                    if (EqualsBytesMassive(vectors[i], vectors[i + 1])) // сегмент - точка)
                    {
                        isNeedToDelete = true;
                    }
                    else
                    {
                        for (int j = i + 2; j < vectors.Count; j += 2)
                        {
                            if ((EqualsBytesMassive(vectors[i], vectors[j]) && EqualsBytesMassive(vectors[i + 1], vectors[j + 1])) || //совпадают сегменты
                                (EqualsBytesMassive(vectors[i], vectors[j + 1]) && EqualsBytesMassive(vectors[i + 1], vectors[j]))) //совпадают начало и конец 2-х сегментов
                            {
                                isNeedToDelete = true;
                            }
                        }
                    }

                    if (!isNeedToDelete)
                    {
                        byte[] m1 = (byte[])vectors[i];
                        byte[] m2 = (byte[])vectors[i + 1];
                        int z;
                        for (z = 0; z < 12; z++)
                        {
                            binWriter.Write(m1[z]);
                        }

                        for (z = 0; z < 12; z++)
                        {
                            binWriter.Write(m2[z]);
                        }

                        binWriter.Write(typeOfSegment[i / 2]);
                    }
                }
            }
        }

        private bool EqualsBytesMassive(object mas1, object mas2)
        {
            byte[] m1 = (byte[])mas1;
            byte[] m2 = (byte[])mas2;

            for (int i = 0; i < m1.Length; i++)
            {
                if (m1[i] != m2[i])
                {
                    return false;
                }
            }
            return true;
        }

        private void WriteBackupPathFiles(string path)
        {
            for (int i = 1; i < 10; i++)
            {
                bool isExistBak1 = false;

                if (File.Exists(path + "pathlines.bak1"))
                {
                    isExistBak1 = true;
                }

                if (i == 1)
                {
                    byte[] pathBytes = File.ReadAllBytes(path + "pathlines.bin");
                    using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bak1", FileMode.Create)))
                    {
                        binWriter.Write(pathBytes);
                    }
                }

                if ((isExistBak1) && (File.Exists(path + "pathlines.bak" + i)))
                {
                    bool isExist = true;

                    if (!File.Exists(path + "pathlines.bak" + (i + 1)))
                    {
                        isExist = false;
                    }

                    byte[] pathBytes = File.ReadAllBytes(path + "pathlines.bak" + i);
                    using (BinaryWriter binWriter = new BinaryWriter(File.Open(path + "pathlines.bak" + (i + 1), FileMode.Create)))
                    {
                        binWriter.Write(pathBytes);
                    }

                    if (!isExist)
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
            }
        }

        private void SwitchOffPathes_CheckedChanged(object sender, EventArgs e)
        {
            byte[] pathBytes = null;

            if (SwitchOffPathes.Checked)
            {
                m_commander.SetObjectsLevel(this.objectNameBox.Text, 1, pathBytes);
            }
            else
            {
                CheckedListBox lodList = shadowsCheckList;

                string path = ((Object3D.LodShadow)lodList.Items[0]).meshName;
                path = ServerPath.GetServerPath(Options.GetHost()) + ServerPath.geometryDir + path;
                path = path.Replace("\\\\", "/");
                path = path.Replace("\\", "/");
                path = path.Replace("//", "/");
                path = "/" + path;
                path = path.Replace("mesh.bmf.zip", "");

                bool isGenerated = false;

                if (File.Exists(path + "pathlines.bin"))
                {
                    using (StreamReader pathInfo = new StreamReader(path + "pathlines.bin"))
                    {
                        pathBytes = File.ReadAllBytes(path + "pathlines.bin");
                    }
                }

                bool is3DObjectExistsPathLines = false;

                if (File.Exists(path + "mesh.ini"))
                {
                    using (StreamReader meshInfo = new StreamReader(path + "mesh.ini"))
                    {
                        String meshIn;
                        while ((meshIn = meshInfo.ReadLine()) != null)
                        {
                            if (meshIn == "pathlines.bin")
                            {
                                is3DObjectExistsPathLines = true;
                            }
                        }
                    }
                }

                if (!is3DObjectExistsPathLines)
                {
                    StreamWriter meshInfoAdd = new StreamWriter(path + "mesh.ini", true);

                    meshInfoAdd.WriteLine("pathlines.bin");
                    meshInfoAdd.Close();
                }


                m_commander.SetObjectsLevel(this.objectNameBox.Text, 0, pathBytes);
            }
        }

        #endregion
    }

}