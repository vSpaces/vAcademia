using System;
using System.Collections;
using System.Text;
using System.Timers;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Xml;
using System.Text.RegularExpressions;
using System.IO;

namespace VisualWorldEditor
{

    // разнести по разным файлам
    partial class Commander
    {
        //command codes
        public const byte CMD_IDLE = 1;
        public const byte CMD_HELLO = 2;
        public const byte CMD_DELETE = 3;
        public const byte CMD_RENAME = 4;
        public const byte CMD_MOVE = 5;
        public const byte CMD_ROTATE = 6;
        public const byte CMD_SCALE = 7;
        public const byte CMD_SELECT = 8;
        public const byte CMD_SELECT_SAME = 9;
        public const byte CMD_MATERIAL = 10;
        public const byte CMD_SELECT_MASK = 11;
        public const byte CMD_SHADOW = 12;
        public const byte CMD_OBJECT_COPY = 13;
        public const byte CMD_OBJECT_CREATE = 14;
        public const byte CMD_CLASS = 15;
        public const byte CMD_LOCK = 16;
        public const byte CMD_PARAMS = 17;
        public const byte CMD_CHANGE_COLLISION = 18; // изменить тип коллизий
        public const byte CMD_GET_MODEL_INFO = 19;
        public const byte CMD_SET_GROUP = 20;
        public const byte CMD_CHANGE_GROUP = 21;

        public const byte CMD_DRAGMODE = 193; // роль активных плоскостей
        public const byte CMD_TELEPORT = 194; // телепорт к объекту
        public const byte CMD_REFRESH = 195; // обновить объекты
        public const byte CMD_FREE_CAMERA = 196; // свободная камера
        public const byte CMD_RELINK_WINDOW = 197; // перепривязать окно клиента при изменении размера
        public const byte CMD_RENAME_ANSWER = 198; // ответ об переименовывании
        public const byte CMD_ROTATE_ANSWER = 199; // ответ об новом повороте
        public const byte CMD_STAT = 200; // статистика от клиента
        public const byte CMD_FREEZ = 201; // заморозка/отморозка аватара

        public const byte CMD_KEY_CODE = 202; // клавиша

        public const byte CMD_CURRENT_CAMERA = 203; // уведомление о позиции/look-at текущей камеры

        public const byte CMD_SHOW_COLLISIONS = 204; // показать коллизионные боксы
        public const byte CMD_SHOW_INVISIBLE_COLLISIONS = 205; // показать невидимую коллизионную геометрию
        public const byte CMD_SHOW_NEAREST_PATHES = 206; // показать ближние пути
        public const byte CMD_SWITCH_OFF_PATHES = 207; // отключить путь у объекта

        public const byte CMD_ENABLE_WIREFRAME = 210; // включить wireframe
        public const byte CMD_SELECT_TEXTURE = 211; // сменить выделенную текстуру
        public const byte CMD_UPDATE_LODS = 212; // сапдейтить структуру лодов

        public const byte CMD_TELEPORT_BY_COORDS = 213; // телепорт к объекту по координатам из бд

        // etc.

        //forms instances
        private static mainForm m_mainFormInstance = null;
        private static MaterialForm m_materialFormInstance = null;

        // singltone instance
        private static readonly Commander instance = new Commander();

        private PipeServer m_pipeServer;
        private ClientMySQL m_mySql;
        private HistoryProcessor m_history;

        private string m_activeTextureName;
        private int m_activeTextureLOD;
        
        
        private Object3D m_object;

        private System.Timers.Timer m_statTimer;

        private bool m_waitRotateAnswer = false;
        private bool m_waitRenameAnswer = false;

        private float m_eyeX, m_eyeY, m_eyeZ;
        private float m_lookAtX, m_lookAtY, m_lookAtZ;

        private bool m_isStatPageOpened;

        private Hashtable m_usedNames;

        private Commander()
        {
            m_mySql = new ClientMySQL();
            m_mySql.Connect();

            m_history  = HistoryProcessor.GetHProcessor();
            m_object = Object3D.GetObject3D();
            m_pipeServer = new PipeServer();
            m_statTimer = new System.Timers.Timer(2000);
            
            OnEventConnect(); // подключаем обработчики событий
            m_pipeServer.Run();
            m_statTimer.Enabled = true;

            m_eyeX = 0;
            m_eyeY = 0;
            m_eyeZ = 0;

            m_lookAtX = 0;
            m_lookAtY = 0;
            m_lookAtZ = 0;

            m_activeTextureLOD = 0;

            m_isStatPageOpened = false;

            m_usedNames = new Hashtable();
        }

        string MySqlEscape(string usString)
        {
            if (usString == null)
            {
                return null;
            }
    
            return Regex.Replace(usString, @"[\r\n\x00\x1a\\'""]", @"\$0");
        }

        public void AddUserLog(int objectID, string res, string content)
        {
            string query = "INSERT INTO world_editor_changes(id_object, username, res, content) VALUES ('" + objectID.ToString() + "', '" + Environment.UserName + "', '"+MySqlEscape(res)+"', '"+MySqlEscape(content)+"')";            
            m_mySql.NonQuery(query);            
        }

        public static Commander GetCommander(mainForm _mainForm, MaterialForm _materialForm)
        {
            m_mainFormInstance = _mainForm;
            m_materialFormInstance = _materialForm;

            return instance;
        }

        public static Commander GetCommander()
        {
            return instance;
        }

        public void OnEventConnect()
        {
            m_pipeServer.CommandAdded += new CommandAddedEventHandler(ProcessPipeCommand);
            m_history.GetStorage().RedoAllowed += new RedoAllowedEventHandler(SetRedoState);
            m_history.GetStorage().UndoAllowed += new UndoAllowedEventHandler(SetUndoState);

            m_object.NamesChanged += new ObjectEditedEventHandler(SetObjectName);
            m_object.CollisionFlagChanged += new ObjectEditedEventHandler(SetCollisionFlag);
            m_object.TransformationChanged += new ObjectEditedEventHandler(SetObjectTransformation);
            m_object.ScaleChanged += new ObjectEditedEventHandler(SetObjectScale);
            m_object.MaterialChanged += new ObjectEditedEventHandler(SendMaterialChanges);
            m_object.ShadowsListChanged += new ObjectEditedEventHandler(SetObjectShadows);

            m_statTimer.Elapsed += new ElapsedEventHandler(OnTimedStat);

        }

        public void OffEventConnect()
        {
            m_pipeServer.CommandAdded -= new CommandAddedEventHandler(ProcessPipeCommand);
            m_history.GetStorage().RedoAllowed -= new RedoAllowedEventHandler(SetRedoState);
            m_history.GetStorage().UndoAllowed -= new UndoAllowedEventHandler(SetUndoState);

            m_object.NamesChanged -= new ObjectEditedEventHandler(SetObjectName);
            m_object.TransformationChanged -= new ObjectEditedEventHandler(SetObjectTransformation);
            m_object.ScaleChanged -= new ObjectEditedEventHandler(SetObjectScale);
            m_object.MaterialChanged -= new ObjectEditedEventHandler(SendMaterialChanges);
            m_object.ShadowsListChanged -= new ObjectEditedEventHandler(SetObjectShadows);
            
            m_statTimer.Elapsed -= new ElapsedEventHandler(OnTimedStat);
        }

        #region "Library loading from server"
        public ArrayList GetCategoryList()
        {
            String queryStr = "SELECT * FROM objects_lib_category";
            return m_mySql.QueryA(queryStr);
        }

        //группа для текущего объекта
        public ArrayList GetCurGroup(String id)
        {
            String queryStr = "SELECT `object_groups`.`id_group`,`objects_map`.* FROM `object_groups` LEFT JOIN `objects_map` ON `object_groups`.`id_group`=`objects_map`.`id` WHERE `object_groups`.`id_object`='" + id + "'";
            return m_mySql.QueryA(queryStr);
        }

        public ArrayList GetCurObjectORGroupInfo(String name)
        {
            String queryStr = "SELECT * FROM `objects_map` WHERE `name`='" + name + "'";
            return m_mySql.QueryA(queryStr);
        }

        public ArrayList GetCurObjectNearlyInfo(String name)
        {
            String queryStr = "SELECT * FROM `objects_map` WHERE `name` LIKE '%" + name + "%'";
            return m_mySql.QueryA(queryStr);
        }

        public ArrayList GetGroupList()
        {
            String queryStr = "SELECT * FROM `objects_map` WHERE `type`IN ('5','6')";
            return m_mySql.QueryA(queryStr);
        }

        public ArrayList GetSubCategoryList(int categoryID)
        {
            String queryStr = "SELECT * FROM `objects_lib_subcategory` WHERE id_category=" + categoryID.ToString();
            return m_mySql.QueryA(queryStr);
        }

        public ArrayList GetObjectsList(int subcategoryID)
        {
            String queryStr = "SELECT * FROM objects_lib WHERE id_subcategory=" + subcategoryID.ToString();
            return m_mySql.QueryA(queryStr);
        }
        #endregion

        public ArrayList GetShadowList(String res)
        {
            ArrayList lods = new ArrayList();

            string[] parts = res.Split(';');

            if (((int)(parts.Length / 3) * 3 != parts.Length - 1) &&
                ((int)(parts.Length / 3) * 3 != parts.Length))
            {
                return lods;
            }

            for (int i = 0; i < (int)(parts.Length / 3); i++)
            {
                string meshName = parts[i * 3 + 0];
                int lodDistance, lodShadow;
                try
                {
                    lodDistance = int.Parse(parts[i * 3 + 1]);
                    lodShadow = int.Parse(parts[i * 3 + 2]);
                }
                catch (Exception /*e*/)
                {
                    return lods;
                }
                Object3D.LodShadow lodLevel = new Object3D.LodShadow(lodDistance, lodShadow, meshName);
                lods.Add(lodLevel);
            }

            return lods;
        }

        public ArrayList GetObjectInfo()
        {
            lock (this)
            {
                if (m_object.Names.Length == 0)
                {
                    return null;
                }

                String queryStr = "SELECT * FROM `objects_map` WHERE `name` IN (";

                int cnt = 0;

                for (int nameIndex = 0; nameIndex < m_object.Names.Length; nameIndex++)
                {
                    if (cnt != 0)
                    {
                        queryStr += ", ";
                    }

                    if (m_object.Names[nameIndex] != "")
                    {
                        queryStr += "'" + m_object.Names[nameIndex] + "'";
                        cnt++;
                    }
                }

                queryStr += ") AND (deleteTime IS NULL)";

                ArrayList objectsInfo = m_mySql.QueryA(queryStr);
                if (objectsInfo == null)
                {
                    return null;
                }

                // просмотреть историю на наличие изменений
                // если свойство было изменено, то установим новое значение
                for (int objectIndex = 0; objectIndex < objectsInfo.Count; objectIndex++)
                {
                    Hashtable info = (Hashtable)objectsInfo[objectIndex];
                    Object className = m_history.FindChangedProp((String)info["name"], Commander.CMD_CLASS);
                    if (className != null)
                    {
                        info["class"] = className;
                    }
                    Object paramsStr = m_history.FindChangedProp((String)info["name"], Commander.CMD_PARAMS);
                    if (paramsStr != null)
                    {
                        info["params"] = paramsStr;
                    }

                    Object lockLevel = m_history.FindChangedProp((String)info["name"], Commander.CMD_LOCK);
                    if (lockLevel != null)
                    {
                        info.Add("lock_level", lockLevel);
                    }
                    else
                    {
                        info.Add("lock_level", GetLockLevel((String)info["name"]));
                    }

                    Object level = m_history.FindChangedProp((String)info["name"], Commander.CMD_CHANGE_COLLISION);
                    if (level != null)
                    {
                        info["level"] = (int)level;
                    }
                }

                return objectsInfo;
            }
        }

        public void Stop()
        {
            m_pipeServer.Stop();
        }

        // обработчик изменения состояния команды отмена
        private void SetUndoState(Object sender, HistoryAllowedEventArgs args)
        {
            m_mainFormInstance.SetUndoMenuState(args.AllowedState);
        }

        // обработчик изменения состояния команды повтор
        private void SetRedoState(Object sender, HistoryAllowedEventArgs args)
        {
            m_mainFormInstance.SetRedoMenuState(args.AllowedState);
        }

        // обработчик изменения имени объекта
        private void SetCollisionFlag(Object sender, ObjectEditedEventArgs args)
        {
            int collisionFlag = (int)args.EditParams[0];
            m_mainFormInstance.SetCollisionFlag(collisionFlag);         
        }

        // обработчик изменения имени объекта
        private void SetObjectName(Object sender, ObjectEditedEventArgs args)
        {
            m_mainFormInstance.SetObjectName((String)args.EditParams[0], (bool)args.EditParams[1]);
            m_object.ModifyLevel = GetModifyLevel((String)args.EditParams[0]);
        }

        // обработчик изменения положения объекта
        private void SetObjectTransformation(Object sender, ObjectEditedEventArgs args)
        {
            Vector transformation = (Vector)args.EditParams[0];
            m_mainFormInstance.SetTransformTBox(transformation);
        }

        // обработчик изменения масштаба
        private void SetObjectScale(Object sender, ObjectEditedEventArgs args)
        {
            Vector scale = (Vector)args.EditParams[0];
            m_mainFormInstance.SetScaleTBox(scale);
        }

        private void SetObjectShadows(Object sender, ObjectEditedEventArgs args)
        {
            ArrayList shadowsList = (ArrayList)args.EditParams[0];
            m_mainFormInstance.SetShadowsList(shadowsList);
        }

        // обработчик события CommandAdded
        private void ProcessPipeCommand(Object sender, CommandAddedEventArgs args)
        {
            try
            {
                DataReader pipeData = args.CmdData;

                if (pipeData == null)
                {
                    return;
                }

                byte commandId = new Byte();
                if (!pipeData.Read(ref commandId))
                {
                    return;
                }

                switch (commandId)
                {
                    case CMD_IDLE:
                        {
                            break;
                        }
                    case CMD_DELETE:
                        {
                            Console.WriteLine("Delete command " + commandId.ToString());
                            break;
                        }
                    case CMD_MOVE:
                        {
                            Console.WriteLine("Move command " + commandId.ToString());
                            SaveMoveResult(pipeData);
                            break;
                        }
                    case CMD_HELLO:
                        {
                            Console.WriteLine("Hello command " + commandId.ToString());
                            SetServer(pipeData);
                            break;
                        }
                    case CMD_SELECT:
                        {
                            Console.WriteLine("Select command " + commandId.ToString());
                            SelectObject(pipeData);
                            m_waitRenameAnswer = false; // произошло выделение вместо переименовывания
                            break;
                        }
                    case CMD_SELECT_TEXTURE:
                        {
                            Console.WriteLine("Select texture command " + commandId.ToString());
                            SelectTexture(pipeData);
                            m_waitRenameAnswer = false; // произошло выделение вместо переименовывания
                            break;
                        }
                    case CMD_STAT:
                        {
                            Console.WriteLine("Stat command " + commandId.ToString());
                            SetStatText(pipeData);
                            break;
                        }
                    case CMD_ROTATE:
                        {
                            SaveNewRotate(pipeData);
                            break;
                        }
                    case CMD_ROTATE_ANSWER:
                        {
                            if (m_waitRotateAnswer)
                            {
                                Console.WriteLine("Rotate answer command " + commandId.ToString());
                                SaveNewRotate(pipeData);
                                m_waitRotateAnswer = false;
                            }
                            break;
                        }
                    case CMD_RENAME_ANSWER:
                        {
                            Console.WriteLine("Rename answer command " + commandId.ToString());
                            //SaveNewName(pipeData);
                            break;
                        }
                    case CMD_OBJECT_COPY:
                        {
                            Console.WriteLine("Object copy command " + commandId.ToString());
                            AddCreatingToHistory(pipeData);
                            break;
                        }
                    case CMD_OBJECT_CREATE:
                        {
                            Console.WriteLine("New object create command " + commandId.ToString());
                            AddNewObjectToHistory(pipeData);
                            break;
                        }
                    case CMD_RELINK_WINDOW:
                        {
                            Console.WriteLine("Relink client window command " + commandId.ToString());
                            m_mainFormInstance.RelinkClientWindow();
                            break;
                        }
                    case CMD_KEY_CODE:
                        {
                            Console.WriteLine("Key press command " + commandId.ToString());
                            ProcessKey(pipeData);
                            break;
                        }
                    case CMD_CURRENT_CAMERA:
                        {
                            Console.WriteLine("Set camera command " + commandId.ToString());
                            SetCurrentCamera(pipeData);
                            break;
                        }
                    case CMD_SET_GROUP:
                        {
                            Console.WriteLine("Group command " + commandId.ToString());
                            SetObjectGroup(pipeData);
                            break;
                        }
                    case CMD_DRAGMODE:
                        {
                            Console.WriteLine("Dragmode command " + commandId.ToString());
                            int dragMode = -1;
                            pipeData.Read(ref dragMode);
                            if (dragMode == 4)
                            {
                              AddNewSegmentToPath(pipeData);
                            }
                            else if((dragMode == 5) || (dragMode == 6) || (dragMode == 11))
                            {
                              Update3DObjectsPath(pipeData);
                            }

                            break;
                        }
                    default:
                        {
                            Console.WriteLine("Unknown command '" + commandId.ToString() + "'");
                            break;
                        }
                }
            }
            catch (Exception e)
            {
                Program.CrashProcedure(e);
            }
        }

        private void SelectTexture(DataReader objectInfo)
        {
            string textureName = "";

            if (!objectInfo.Read(ref textureName))
            {
                return;
            }

            int lod = 0;
            objectInfo.Read(ref lod);

            m_activeTextureName = textureName;
            m_activeTextureLOD = lod;

            if (m_mainFormInstance != null)
            {
                m_mainFormInstance.ActiveTextureChanged();
            }
        }

        public string GetActiveTextureName()
        {
            return m_activeTextureName;
        }

        public int GetActiveTextureLOD()
        {
            return m_activeTextureLOD;
        }

        private void SelectObject(DataReader objectInfo)
        {
            short selectedObjects = 0;

            if (!objectInfo.Read(ref selectedObjects))
            {
                return;
            }

            //m_object.DeleteInfo(); // очищаем инфу о предыдущем объекте

            String[] namesArray = new String[] { };
            Vector translation = Vector.GetNaN();
            Vector scale = Vector.GetNaN();
            ArrayList shadowsList = new ArrayList();
            int collisionFlag = -1;

            if (1 == selectedObjects)
            {
                // имя объекта
                String objectName = "";
                if (!objectInfo.Read(ref objectName))
                {
                    return;
                }             
                
                // положение объекта
                translation = new Vector();

                float xyz = 0;
                objectInfo.Read(ref xyz);
                translation.X = xyz;
                objectInfo.Read(ref xyz);
                translation.Y = xyz;
                objectInfo.Read(ref xyz);
                translation.Z = xyz;

                // Масштаб объекта
                scale = new Vector();
                objectInfo.Read(ref xyz);
                scale.X = xyz;
                objectInfo.Read(ref xyz);
                scale.Y = xyz;
                objectInfo.Read(ref xyz);
                scale.Z = xyz;

                // инфа о тенях?
                // количество лодов * дистанция, включена ли тень
                byte lodsCount = 0;
                // чтение
                objectInfo.Read(ref lodsCount);

                shadowsList = new ArrayList();
                for(int lodIndex = 0; lodIndex < lodsCount; lodIndex ++)
                {
                    // чтение
                    float lodDistance = 0;
                    objectInfo.Read(ref lodDistance);
                    int lodShadow = 0;
                    objectInfo.Read(ref lodShadow);
                    
                    Object3D.LodShadow lodLevel = new Object3D.LodShadow(lodDistance, lodShadow, "");
                    shadowsList.Add(lodLevel);
                }

                namesArray = new String[] { objectName };

                m_object.Names = namesArray;
                ArrayList objectsInfo = GetObjectInfo();
                if (objectsInfo != null)
                {
                    Hashtable objInfo = (Hashtable)objectsInfo[0];
                    collisionFlag = (int)objInfo["level"];
                    shadowsList = GetShadowList((string)objInfo["res"]);
                }

                this.WorkState = WorkStates.OneSelect;
            }
            else if (selectedObjects > 1)
            {
                // только имена, все действия относительные
                namesArray = new String[selectedObjects];                
                for (int objectIndex = 0; objectIndex < selectedObjects; objectIndex ++)
                {
                    String objectName = "";
                    if (!objectInfo.Read(ref objectName))
                    {
                        break;
                    }
                    namesArray[objectIndex] = objectName;
                }

                bool isOnlyOneCollisionFlagForAllObjects = true;
                m_object.Names = namesArray;
                ArrayList objectsInfo = GetObjectInfo();
                if (objectsInfo == null)
                {
                    return;
                }
                for (int i = 0; i < objectsInfo.Count; i++)
                {
                    Hashtable objInfo = (Hashtable)objectsInfo[i];
                    int currentCollisionFlag = (int)objInfo["level"];
                    if (-1 == collisionFlag)
                    {
                        collisionFlag = currentCollisionFlag;
                    }
                    else if (collisionFlag != currentCollisionFlag)
                    {
                        isOnlyOneCollisionFlagForAllObjects = false;
                    }
                }

                if (false == isOnlyOneCollisionFlagForAllObjects)
                {
                    collisionFlag = -1;
                }

                this.WorkState = WorkStates.MoreSelect;             
            }

            m_object.Transformation = translation;
            m_object.Scale = scale;
            m_object.ShadowsList = shadowsList;
            m_object.CollisionFlag = collisionFlag;
            // добавление в историю если это новое выделение, а не результат операции "отмена" или "повтор"
            byte isNewSelect = 0;
            objectInfo.Read(ref isNewSelect);
            if (isNewSelect == 1)
            {
                m_history.AddSelect(namesArray);
            }                
        }

        public void DeleteObject()
        {
            DataWriter deleteCommand = new DataWriter();

            deleteCommand.AddData(Commander.CMD_DELETE);

            String[] names = m_object.Names;

            if(names.Length > 0)
            {
                deleteCommand.AddData((short)names.Length);
                if(names.Length > 1)
                {                    
                    for (int nameIndex = 0; nameIndex < names.Length; nameIndex++)
                    {
                        deleteCommand.AddData(names[nameIndex]);
                    }
                }
                else
                {
                    deleteCommand.AddData(Object3D.GetObject3D().Names[0]);
                }                
            }

            m_pipeServer.addCommand(deleteCommand);

            m_history.AddDelete(names);
            m_object.DeleteInfo();

            m_workState = WorkStates.NoSelect;
        }

        public void SaveNewRotate(DataReader rotateInfo)
        {
            Quaternion rotate = new Quaternion();
            float xyz = 0;
            Vector angles = new Vector();
            rotateInfo.Read(ref xyz);
            angles.X = xyz;
            rotateInfo.Read(ref xyz);
            angles.Y = xyz;
            rotateInfo.Read(ref xyz);
            angles.Z = xyz;

            short rotatedObjects = 0;
            rotateInfo.Read(ref rotatedObjects);

            String[] objectsNames = new String[rotatedObjects];
            float x = 0, y = 0, z = 0, w = 0;
            String objName = "";

            Hashtable rotates = new Hashtable();

            for(int objectIndex = 0; objectIndex < rotatedObjects; objectIndex ++)
            {                
                rotateInfo.Read(ref objName);
                objectsNames[objectIndex] = objName;

                rotateInfo.Read(ref x);
                rotateInfo.Read(ref y);
                rotateInfo.Read(ref z);
                rotateInfo.Read(ref w);

                rotates.Add(objName, new Quaternion(x, y, z, w));
            }                      

            m_history.AddRotate(objectsNames, angles, rotates);
        }

        public void RenameObject(String newName)
        {
            if (newName == Object3D.GetObject3D().Names[0])
                return;

            DataWriter renameCommand = new DataWriter();
            renameCommand.AddData(Commander.CMD_RENAME);
            renameCommand.AddData(Object3D.GetObject3D().Names[0]);
            renameCommand.AddData(newName);

            m_waitRenameAnswer = true;
            m_pipeServer.addCommand(renameCommand);

            m_history.AddRename(Object3D.GetObject3D().Names[0], newName, m_mySql);
        }

        public void SendGroupChanges(String groupName, int groupType, Vector point)
        {
            DataWriter sendGroupCommand = new DataWriter();
            sendGroupCommand.AddData(Commander.CMD_CHANGE_GROUP);

            sendGroupCommand.AddData(groupName);
            sendGroupCommand.AddData(groupType);
            sendGroupCommand.AddData(point.X);
            sendGroupCommand.AddData(point.Y);
            sendGroupCommand.AddData(point.Z);

            m_pipeServer.addCommand(sendGroupCommand);

         }

        public void SetGroup(String groupName, int groupType)
        {
            DataWriter groupCommand = new DataWriter();
            byte isUndo = 0;

            groupCommand.AddData(Commander.CMD_SET_GROUP);

            groupCommand.AddData(Object3D.GetObject3D().Names[0]);
            groupCommand.AddData(groupName);
            groupCommand.AddData(groupType);
            groupCommand.AddData(isUndo);

            string groupOldName = "";

            Object groupOld = m_history.FindChangedProp(Object3D.GetObject3D().Names[0], Commander.CMD_SET_GROUP);
            if (groupOld != null)
            {
               groupOldName = (string)groupOld;
            }

      
             m_history.AddGroup(Object3D.GetObject3D().Names[0], groupName, groupOldName, groupType);
             m_pipeServer.addCommand(groupCommand);
          

        }

        public void SendModelInfoQuery(String path)
        {
            DataWriter modelInfoCommand = new DataWriter();
            modelInfoCommand.AddData(Commander.CMD_GET_MODEL_INFO);
            modelInfoCommand.AddData(path);

            m_pipeServer.addCommand(modelInfoCommand);

            //m_history.AddRename(Object3D.GetObject3D().Names[0], newName);
        }


        private void SaveNewName(DataReader renameInfo)
        {
            String oldName = "";
            renameInfo.Read(ref oldName);

            String newName = "";
            renameInfo.Read(ref newName);

            if(m_waitRenameAnswer)
            {
                m_history.AddRename(oldName, newName, m_mySql);
                m_waitRenameAnswer = false;
            }           

            m_mainFormInstance.SetObjectName(newName);
        }

        public void SelectSameObjects()
        {
            if(m_object.Names.Length == 1)
            {
                DataWriter selectSameCommand = new DataWriter();

                selectSameCommand.AddData(Commander.CMD_SELECT_SAME);
                selectSameCommand.AddData(Object3D.GetObject3D().Names[0]);

                m_pipeServer.addCommand(selectSameCommand);

                m_workState = WorkStates.MoreSelect;
            }
        }

        public void SelectByMask(String mask)
        {
            DataWriter selectByMask = new DataWriter();

            selectByMask.AddData(Commander.CMD_SELECT_MASK);
            selectByMask.AddData(mask);

            m_pipeServer.addCommand(selectByMask);
        }

        public void MoveObject(Vector newTransform)
        {
            DataWriter moveCommand = new DataWriter();

            moveCommand.AddData(Commander.CMD_MOVE);

            Vector moveDelta;

            if (!Vector.IsNaN(m_object.Transformation))
            {
                moveDelta = new Vector(newTransform.X - m_object.Transformation.X,
                                        newTransform.Y - m_object.Transformation.Y,
                                        newTransform.Z - m_object.Transformation.Z);
            }
            else
            {
                moveDelta = new Vector(newTransform.X,
                                        newTransform.Y,
                                        newTransform.Z);
            }

            moveCommand.AddData(moveDelta.X);
            moveCommand.AddData(moveDelta.Y);
            moveCommand.AddData(moveDelta.Z);

            m_history.AddMove(m_object.Names, moveDelta);

            m_pipeServer.addCommand(moveCommand);        
        }

        public void RotateObject(Vector rotateAngles)
        {
            DataWriter rotateCommand = new DataWriter();

            rotateCommand.AddData(Commander.CMD_ROTATE);

            rotateCommand.AddData(rotateAngles.X);
            rotateCommand.AddData(rotateAngles.Y);
            rotateCommand.AddData(rotateAngles.Z);
            
            //m_history.AddRotate(m_object.Names, rotateAngles);
            m_waitRotateAnswer = true;

            m_pipeServer.addCommand(rotateCommand);            
        }

        public void ScaleObject(Vector newScale)
        {
            DataWriter scaleCommand = new DataWriter();

            scaleCommand.AddData(Commander.CMD_SCALE);

            Vector scaleDelta;
            if(!Vector.IsNaN(m_object.Scale))
            {
                scaleDelta = new Vector(newScale.X - m_object.Scale.X,
                                        newScale.Y - m_object.Scale.Y,
                                        newScale.Z - m_object.Scale.Z);
            }
            else
            {
                scaleDelta = new Vector(newScale.X,
                                        newScale.Y,
                                        newScale.Z);
            }

            scaleCommand.AddData(scaleDelta.X);
            scaleCommand.AddData(scaleDelta.Y);
            scaleCommand.AddData(scaleDelta.Z);

            m_history.AddScale(m_object.Names, scaleDelta);

            m_pipeServer.addCommand(scaleCommand);            
        }

        public void SetShadow(int lodIndex, int state, string meshName)
        {
            DataWriter shadowCommand = new DataWriter();

            shadowCommand.AddData(Commander.CMD_SHADOW);

            shadowCommand.AddData((byte)lodIndex);
            shadowCommand.AddData((byte)state);

            m_history.AddShadow(m_object.Names, new Object3D.LodShadow(lodIndex, state, meshName));

            m_pipeServer.addCommand(shadowCommand);
        }

        public void SendUpdateLod(string name, string newRes)
        {
            DataWriter lodCommand = new DataWriter();

            lodCommand.AddData(Commander.CMD_UPDATE_LODS);

            lodCommand.AddData(m_object.Names[0]);
            lodCommand.AddData(newRes);

            m_pipeServer.addCommand(lodCommand);
        }

        public bool CreateLod(float parentLodDistance, bool _state, string mesh)
        {
            ArrayList lodList = Object3D.GetObject3D().ShadowsList;

            // новая дистанция для лода
            LodInfo lodDialog = new LodInfo(mesh == "" ? LodInfo.LOD_INFO_ADD_SIMILAR_LOD : LodInfo.LOD_INFO_ADD_LIKE_OBJECT);
            lodDialog.SetShadow(_state);
            if (mesh != "")
            {
                lodDialog.SetDistance((int)parentLodDistance);
                parentLodDistance = 0;
            }
            DialogResult createResult = lodDialog.ShowDialog();
            if(createResult == DialogResult.OK)
            {
                int lodDistance = lodDialog.GetDistance();
                bool state = lodDialog.GetShadow();
                if(lodDistance != -1)
                {
                    String objectName = m_object.Names[0];

                    String queryStr = "SELECT `res` FROM `objects_map` WHERE `name`='" + objectName + "'";                    
                    ArrayList aResult = m_mySql.QueryA(queryStr);

                    if (aResult == null)
                        return false;

                    Hashtable row = (Hashtable)aResult[0];
                    String oldRes = (String)row["res"];
                    oldRes = oldRes.Replace("\\", "\\\\");

                    int searchStrPos = -1;
                    int sourcePathPos = 0;
                    int distancePos = 0;
                    int shadowPos = 0;

                    bool addLod = false;
                    float preDistance = 0;
                    float postDistance = 0;

                    while(searchStrPos < oldRes.Length - 1)
                    {
                        sourcePathPos = oldRes.IndexOf(";", searchStrPos + 1);
                        distancePos = oldRes.IndexOf(";", sourcePathPos + 1);
                        shadowPos = oldRes.IndexOf(";", distancePos + 1);

                        String distanceStr = oldRes.Substring(sourcePathPos + 1, distancePos - (sourcePathPos + 1));

                        if(float.Parse(distanceStr) == parentLodDistance)
                        {
                            addLod = true;

                            if(shadowPos < oldRes.Length - 1)
                            {
                                int nextSrcPos = oldRes.IndexOf(";", shadowPos + 1);
                                int nextDistPos = oldRes.IndexOf(";", nextSrcPos + 1);
                                postDistance = float.Parse(oldRes.Substring(nextSrcPos + 1, nextDistPos - (nextSrcPos + 1)));
                            }                            

                            break;
                        }

                        if (float.Parse(distanceStr) == lodDistance)
                        {
                            MessageBox.Show("Для дистанции '" + lodDistance.ToString() + "' необходимо выбрать другой лод в качестве исходного или поменяйте дистанцию.", "Ошибка создания лода", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return false;
                        }

                        if (float.Parse(distanceStr) < lodDistance)
                        {
                            preDistance = float.Parse(distanceStr);
                            searchStrPos = shadowPos;
                        }

                        if (float.Parse(distanceStr) > lodDistance)
                        {
                            postDistance = float.Parse(distanceStr);                            
                            break;
                        }
                    }

                    if ((addLod) || (mesh != ""))
                    {
                        if (((lodDistance >= postDistance)
                            || (lodDistance <= preDistance)
                            || (lodDistance == parentLodDistance)) && (postDistance != 0))
                        {
                            MessageBox.Show("Для дистанции '"+ lodDistance.ToString() +"' необходимо выбрать другой лод в качестве исходного\nили поменяйте дистанцию.", "Ошибка создания лода", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return false;
                        }
                        else
                        {
                            int newLodShadow = 0;
                            if (state)
                            {
                                newLodShadow = 1;
                            }

                            String newRes = "";
                            string newMesh = "";
                            if (mesh != "")
                            {
                                newMesh = mesh + ";";
                            }
                            else
                            {
                                oldRes.Substring(searchStrPos + 1, sourcePathPos - searchStrPos);
                            }
                            if ((lodDistance >= parentLodDistance) && (parentLodDistance != 0))
                            {
                                newRes = oldRes.Substring(0, shadowPos + 1) +
                                    newMesh + ((int)lodDistance).ToString() + ";" + newLodShadow.ToString() +
                                    oldRes.Substring(shadowPos);
                            }
                            else
                            {
                                newRes = oldRes.Substring(0, searchStrPos + 1) +
                                    newMesh + ((int)lodDistance).ToString() + ";" + newLodShadow.ToString() + ";" +
                                    oldRes.Substring(searchStrPos + 1);
                            }

                            queryStr = "UPDATE `objects_map` SET `res`='" + newRes + "' WHERE `res`='" + oldRes + "'";
                            Console.WriteLine(queryStr);
                            int iResult = m_mySql.NonQuery(queryStr);

                            SendUpdateLod(m_object.Names[0], newRes);

                            m_object.ShadowsList = GetShadowList(newRes);           
                        }
                    }
                }
                else
                {
                    MessageBox.Show("Введена некорректная дистанция для лода.", "Ошибка создания лода", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return false;
                }
            }

            mainForm.ReloadServers();

            return true;
        }

        public void DeleteLod(int lodIndex)
        {
            String objectName = m_object.Names[0];

            String queryStr = "SELECT `res` FROM `objects_map` WHERE `name`='" + objectName + "'";
            ArrayList aResult = m_mySql.QueryA(queryStr);

            if (aResult == null)
            {
                return;
            }

            Hashtable row = (Hashtable)aResult[0];
            String oldRes = (String)row["res"];
            String res = "";
            oldRes = oldRes.Replace("\\", "\\\\");

            string[] parts = oldRes.Split(';');
            int i = 0;
            foreach (string part in parts)
            {
                int num = (i - i % 3) / 3;
                if ((num != lodIndex) && (part != ""))
                {
                    res += part;
                    res += ";";
                }
                i++;
            }

            queryStr = "UPDATE `objects_map` SET `res`='" + res + "' WHERE `res`='" + oldRes + "'";
            Console.WriteLine(queryStr);

            SendUpdateLod(m_object.Names[0], res);
                          
            int iResult = m_mySql.NonQuery(queryStr);

            m_object.ShadowsList = GetShadowList(res);

            mainForm.ReloadServers();
        }

        public void EditLod(int lodIndex)
        {
            ArrayList lodList = Object3D.GetObject3D().ShadowsList;

            if (lodIndex >= lodList.Count)
            {
                return;
            }

            // новая дистанция для лода
            LodInfo lodDialog = new LodInfo(LodInfo.LOD_INFO_EDIT);
            lodDialog.SetShadow(((Object3D.LodShadow)lodList[lodIndex]).shadow == 1);
            lodDialog.SetDistance((int)(((Object3D.LodShadow)lodList[lodIndex]).distance));
            DialogResult createResult = lodDialog.ShowDialog();
            if (createResult == DialogResult.OK)
            {
                String objectName = m_object.Names[0];

                String queryStr = "SELECT `res` FROM `objects_map` WHERE `name`='" + objectName + "'";
                ArrayList aResult = m_mySql.QueryA(queryStr);

                if (aResult == null)
                {
                    return;
                }

                Hashtable row = (Hashtable)aResult[0];
                String oldRes = (String)row["res"];
                String res = "";
                oldRes = oldRes.Replace("\\", "\\\\");

                string[] parts = oldRes.Split(';');
                int i = 0;
                foreach (string part in parts)
                {
                    int num = (i - i % 3) / 3;
                    if (part != "")
                    {
                        if (num != lodIndex)
                        {
                            res += part;
                            res += ";";
                        }
                        else
                        {
                            switch (i % 3)
                            {
                            case 0:
                                    res += part;
                                    break;

                            case 1:
                                    res += ((int)lodDialog.GetDistance()).ToString();
                                    break;

                            case 2:
                                    res += lodDialog.GetShadow() ? "1" : "0";
                                    break;
                            }
                            res += ";";
                        }
                    }
                    i++;
                }

                queryStr = "UPDATE `objects_map` SET `res`='" + res + "' WHERE `res`='" + oldRes + "'";
                SendUpdateLod(m_object.Names[0], res);
                Console.WriteLine(queryStr);

                int iResult = m_mySql.NonQuery(queryStr);

                m_object.ShadowsList = GetShadowList(res);    
            }

            mainForm.ReloadServers();
        }

        public void AddLodLikeObject()
        {
            string objectName = "";
            if (MyInputBox.InputBox("Добавление лода", "Введите имя объекта, на основе которого вы хотите добавить лод:", ref objectName) == DialogResult.OK)
            {
                String queryStr = "SELECT `res` FROM `objects_map` WHERE `name`='" + objectName + "'";
                ArrayList aResult = m_mySql.QueryA(queryStr);

                if (aResult == null)
                {
                    MessageBox.Show("Объект с таким именем не существует", "Ошибка создания лода", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                Hashtable row = (Hashtable)aResult[0];
                String res = (String)row["res"];
                res = res.Replace("\\", "\\\\");
                string[] parts = res.Split(';');

                if (parts.Length != 4)
                {
                    MessageBox.Show("Объект-образец не должен иметь лодов!", "Ошибка создания лода", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                if (CreateLod(float.Parse((string)parts[1]), (string)parts[2] == "1", (string)parts[0]) == true)
                {
                    queryStr = "DELETE FROM `objects_map` WHERE `name`='" + objectName + "'";
                    int iResult = m_mySql.NonQuery(queryStr);

                    DataWriter deleteCommand = new DataWriter();

                    deleteCommand.AddData(Commander.CMD_DELETE);

                    short count = 1;
                    deleteCommand.AddData(count);         
                    deleteCommand.AddData(objectName);
                    
                    m_pipeServer.addCommand(deleteCommand);
                }
            }

            mainForm.ReloadServers();
        }

        private void SetCurrentCamera(DataReader cameraData)
        {
            cameraData.Read(ref m_eyeX);
            cameraData.Read(ref m_eyeY);
            cameraData.Read(ref m_eyeZ);
            cameraData.Read(ref m_lookAtX);
            cameraData.Read(ref m_lookAtY);
            cameraData.Read(ref m_lookAtZ);
        }

        private void SetObjectGroup(DataReader groupData)
        {
            String objectName = "";
            groupData.Read(ref objectName);
            String groupName = "";
            groupData.Read(ref groupName);
            byte isUndo = 0;
            groupData.Read(ref isUndo);
            m_mainFormInstance.SetObjectsGroup(objectName, groupName, isUndo);
        }

        private void AddNewSegmentToPath(DataReader segmentData)
        {
            ArrayList pointsList = new ArrayList();

            float xyz = 0;

            byte type = 0;

            while (segmentData.Read(ref xyz))
            {
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref type);
                pointsList.Add(type);
            }

            m_mainFormInstance.AddNewSegmentToPath(pointsList);

        }

        private void Update3DObjectsPath(DataReader segmentData)
        {
            ArrayList pointsList = new ArrayList();

            float xyz = 0;

            byte type = 0;

            while (segmentData.Read(ref xyz))
            {
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref xyz);
                pointsList.Add(xyz);
                segmentData.Read(ref type);
                pointsList.Add(type);
            }

            m_mainFormInstance.Update3DObjectsPath(pointsList);
        }

        private void SetStatText(DataReader statData)
        {
            String statString = "";
            statData.Read(ref statString);

            statString += "\nКоординаты аватара: ";
            float avatarXYZ = 0;
            statData.Read(ref avatarXYZ);
            statString += "x=" + avatarXYZ.ToString("F1") + ", ";
            statData.Read(ref avatarXYZ);
            statString += "y=" + avatarXYZ.ToString("F1") + ", ";
            statData.Read(ref avatarXYZ);
            statString += "z=" + avatarXYZ.ToString("F1");

            statString += "\nПоворот аватара: ";
            avatarXYZ = 0;
            statData.Read(ref avatarXYZ);
            statString += "a=" + avatarXYZ.ToString("F3") + ", ";
            statData.Read(ref avatarXYZ);
            statString += "x=" + avatarXYZ.ToString("F3") + ", ";
            statData.Read(ref avatarXYZ);
            statString += "y=" + avatarXYZ.ToString("F3") + ", ";
            statData.Read(ref avatarXYZ);
            statString += "z=" + avatarXYZ.ToString("F3");

            m_mainFormInstance.SetClientStat(statString);
        }

        public void Undo()
        {
            m_pipeServer.addCommand(m_history.GetUndoCommand());         
        }

        public void Redo()
        {
            m_pipeServer.addCommand(m_history.GetRedoCommand());
        }

        public void Save(bool clearHistoryOnCancel)
        {
            m_history.ProcessHistory(m_mySql, clearHistoryOnCancel);
        }

        public void SendHelloCommand()
        {
            m_pipeServer.addHelloCmd();
        }

        public void SetStatPageOpened(bool isStatPageOpened)
        {
            m_isStatPageOpened = isStatPageOpened;
        }

        private void OnTimedStat(object source, ElapsedEventArgs e)
        {
            if (m_pipeServer.IsClientConnected() && m_isStatPageOpened)
            {
                DataWriter statCommand = new DataWriter();
                statCommand.AddData(Commander.CMD_STAT);
                m_pipeServer.addCommand(statCommand);
            }            
        }

        public void SendFreezCommand(bool _state)
        {
            DataWriter freezCommand = new DataWriter();
            freezCommand.AddData(Commander.CMD_FREEZ);
            byte state;
            if(_state)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            freezCommand.AddData(state);
            m_pipeServer.addCommand(freezCommand);
        }

        public int SendChangeCollisionFlag(int collisionFlag)
        {
            if (m_object.CollisionFlag == collisionFlag)
            {
                return -2;
            }

            DialogResult confirmResult = DialogResult.Yes;
            if (m_object.Names.Length != 1)
            {
                confirmResult = MessageBox.Show("Выделено несколько объектов. Применить настройки коллизий ко всем?", "Коллизии", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            }

            if (confirmResult == DialogResult.Yes)
            {
                DataWriter collisionCommand = new DataWriter();
                collisionCommand.AddData(Commander.CMD_CHANGE_COLLISION);
                collisionCommand.AddData(collisionFlag);
                m_pipeServer.addCommand(collisionCommand);

                m_history.AddChangeCollision(collisionFlag);

                return -2;
            }
            else
            {
                return m_object.CollisionFlag;
            }
        }

        public void SendShowCollisionsCommand(bool _state)
        {
            DataWriter scCommand = new DataWriter();
            scCommand.AddData(Commander.CMD_SHOW_COLLISIONS);
            byte state;
            if (_state)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            scCommand.AddData(state);
            m_pipeServer.addCommand(scCommand);
        }

        public void SendShowInvisibleCollisionsCommand(bool _state)
        {
            DataWriter scCommand = new DataWriter();
            scCommand.AddData(Commander.CMD_SHOW_INVISIBLE_COLLISIONS);
            byte state;
            if (_state)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            scCommand.AddData(state);
            m_pipeServer.addCommand(scCommand);
        }

        public void SendShowNearestPathesCommand(bool _state)
        {
            DataWriter scCommand = new DataWriter();
            scCommand.AddData(Commander.CMD_SHOW_NEAREST_PATHES);
            byte state;
            if (_state)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            scCommand.AddData(state);
            m_pipeServer.addCommand(scCommand);
        }

        public void SendEnableWireframeCommand(bool _state)
        {
            DataWriter wireframeCommand = new DataWriter();
            wireframeCommand.AddData(Commander.CMD_ENABLE_WIREFRAME);
            byte state;
            if (_state)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            wireframeCommand.AddData(state);
            m_pipeServer.addCommand(wireframeCommand);
        }
        
        public void SendFreeCamCommand(bool _state)
        {
            DataWriter freeCamCommand = new DataWriter();
            freeCamCommand.AddData(Commander.CMD_FREE_CAMERA);
            byte state;
            if(_state)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            freeCamCommand.AddData(state);
            m_pipeServer.addCommand(freeCamCommand);
        }

        public void SendObjectCreateCommand(bool isCopy)
        {
            DataWriter createCommand = new DataWriter();
            createCommand.AddData(Commander.CMD_OBJECT_COPY);

            // копирование или создание нового объекта
            byte copyState = 0;
            if(isCopy)
            {
                copyState = 1;
            }
            createCommand.AddData(copyState);
            byte redoFlag = 0;
            createCommand.AddData(redoFlag);

            if(!isCopy)
            {
                
                createCommand.AddData((short)m_object.Names.Length);
                // для каждого выделенного объекта
                for(int objIndex = 0; objIndex < m_object.Names.Length; objIndex ++)
                {
                    // имя копируемого объекта
                    createCommand.AddData(m_object.Names[objIndex]);

                    String queryStr = "SELECT `res`,`level` FROM `objects_map` WHERE `name`='" + m_object.Names[objIndex] + "'";

                    ArrayList aResult = m_mySql.QueryA(queryStr);

                    if (aResult == null)
                        continue;

                    Hashtable row = (Hashtable)aResult[0];
                    String objectUrl = "geometry\\" + (String)row["res"];
                    createCommand.AddData(objectUrl);
                    String objectName = m_object.Names[objIndex] + "c";
                    createCommand.AddData(objectName);
                    createCommand.AddData((Int32)row["level"]);
                }
            }
            else
            {
                for(int objIndex = 0; objIndex < m_object.Names.Length; objIndex ++)
                {
                    // имя копируемого объекта
                    createCommand.AddData(GetNextObjectName(m_object.Names[objIndex]));
                }
            }

            m_pipeServer.addCommand(createCommand);
        }

        private void SendMaterialChanges(Object sender, ObjectEditedEventArgs args)
        {
            Hashtable materialParam = (Hashtable)args.EditParams[0];

            DataWriter materialCommand = new DataWriter();
            materialCommand.AddData(Commander.CMD_MATERIAL);

            byte updateFlag = (byte)((Int32)materialParam["updateFlag"]); // 0 - изменение параметра, 1 - создание материала
            materialCommand.AddData(updateFlag);

            String materialName = (String)materialParam["materialName"];// имя материала
            materialCommand.AddData(materialName);
            
            if(updateFlag == 0)
            {
                materialCommand.AddData((String)materialParam["paramType"]);
                materialCommand.AddData((String)materialParam["paramName"]);
                materialCommand.AddData((String)materialParam["value"]);
                if(materialParam["valueType"] != null)
                {
                    materialCommand.AddData((String)materialParam["valueType"]);
                }
                else
                {
                    materialCommand.AddData("");
                }
                
            }
            else if (updateFlag == 1)
            {
                materialCommand.AddData((String)materialParam["materialContent"]);
            }
            else if (updateFlag == 2)
            {
                materialCommand.AddData((String)materialParam["state"]);

                if(materialParam.ContainsKey("materialContent"))
                {
                    materialCommand.AddData((String)materialParam["materialContent"]);
                }
            }

            m_pipeServer.addCommand(materialCommand);
        }

        private void AddCreatingToHistory(DataReader creatingInfo)
        {
            short objCount = 0;
            creatingInfo.Read(ref objCount);

            byte redoFlag = 0;
            creatingInfo.Read(ref redoFlag);
            if(redoFlag == 1)
            {
                return;
            }

            String[] objects = new String[objCount];
            ArrayList objCopyInfo = new ArrayList();
            String[] objectsForSelect = new String[objCount];
            for (int objIndex = 0; objIndex < objCount; objIndex++)
            {
                // имя оригинального объекта
                String objName = "";
                creatingInfo.Read(ref objName);

                // имя копии
                String objCopyName = "";
                creatingInfo.Read(ref objCopyName);

                // координаты нового объекта
                float x = 0, y = 0, z = 0;
                creatingInfo.Read(ref x);
                creatingInfo.Read(ref y);
                creatingInfo.Read(ref z);

                Vector objCoords = new Vector(x, y, z);

                objects[objIndex] = objName;
                objectsForSelect[objIndex] = objCopyName;
                objCopyInfo.Add(objCopyName);
                objCopyInfo.Add(objCoords);
            }

            if(objCount > 1)
            {
                m_object.DeleteInfo();

                m_object.Names = objectsForSelect;

                m_workState = WorkStates.MoreSelect;
            }

            m_history.AddCopy(objects, objCopyInfo);
        }

        public void AddSelectedObjectToLibrary(String categoryName, String subcategoryName, String nameDisplay)
        {
            String queryStr = "SELECT `id`, `name` FROM `objects_lib_category` WHERE `name`='" + categoryName + "'";
            ArrayList aResult = m_mySql.QueryA(queryStr);
            Int64 subcategoryId = -1;
            Int32 categoryId = -1;
            if(aResult != null)
            {
                // данная категория есть
                categoryId = (Int32)(UInt32)(((Hashtable)aResult[0])["id"]);

                queryStr = "SELECT `id`, `name` FROM `objects_lib_subcategory` WHERE `name`='" + subcategoryName + "' AND `id_category`='" + categoryId.ToString() + "'" ;
                aResult = m_mySql.QueryA(queryStr);

                if(aResult != null)
                {
                    subcategoryId = (Int32)(UInt32)(((Hashtable)aResult[0])["id"]);
                }
                else
                {
                    subcategoryId = 0;
                }
            }
            else
            {
               // категория отсутствует, вставляем новую
                queryStr = "INSERT INTO `objects_lib_category` SET `name`='" + categoryName + "'";
                m_mySql.NonQuery(queryStr);

                // id категории
                //queryStr = "SELECT LAST_INSERT_ID() as `last_id` FROM `objects_lib_category`";
                queryStr = "SELECT LAST_INSERT_ID() as `last_id`";
                aResult = m_mySql.QueryA(queryStr);
                categoryId = (Int32)(Int64)(((Hashtable)aResult[0])["last_id"]);

                subcategoryId = 0;
            }

            if(subcategoryId == 0)
            {
                // вставляем новую подкатегорию
                queryStr = "INSERT INTO `objects_lib_subcategory` SET `id_category`='" + categoryId.ToString() + "', `name`='" + subcategoryName + "'";
                m_mySql.NonQuery(queryStr);

                // id подкатегории
                //queryStr = "SELECT LAST_INSERT_ID() as `last_id` FROM `objects_lib_subcategory`";
                queryStr = "SELECT LAST_INSERT_ID() as `last_id`";
                aResult = m_mySql.QueryA(queryStr);
                subcategoryId = (Int64)(((Hashtable)aResult[0])["last_id"]);
            }

            // вставка объекта
            queryStr = "SELECT `res`, `level`, `class`, `params`, `bx`, `by`, `bz` FROM `objects_map` WHERE `name`='" + m_object.Names[0] + "'";
            aResult = m_mySql.QueryA(queryStr);
            Hashtable objectInfo = (Hashtable)aResult[0];

            queryStr = "INSERT INTO `objects_lib` SET `id_subcategory`='" + subcategoryId.ToString() + "', " +
                "`name`='" + m_object.Names[0] + "', " + 
                "`name_display`='" + nameDisplay + "', " + 
                "`res`='" + ((String)objectInfo["res"]).Replace("\\", "\\\\") + "', " + 
                "`level`='" + objectInfo["level"] + "', " + 
                "`class`='" + objectInfo["class"] + "', " +
                "`params`='" + objectInfo["params"] + "', " +
                "`bx`='" + objectInfo["bx"] + "', " +
                "`by`='" + objectInfo["by"] + "', " +
                "`bz`='" + objectInfo["bz"] + "'";
            m_mySql.NonQuery(queryStr);

            //обновление вкладки библиотеки
            m_mainFormInstance.UpdateLibrary();
        }

        public void DeleteCategoryFromLib(int categoryId, int subCategoryId)
        {
            if(categoryId != -1)
            {
                if (m_history.ProcessHistory(m_mySql, false) == false)
                {
                    System.Windows.Forms.MessageBox.Show("Удаление категории из библиотеки доступно только после сохранения.", "Предупреждение", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
                }
                else
                {
                    // удаляем объекты подкатегорий
                    String queryStr = "DELETE FROM `objects_lib` WHERE `id_subcategory` IN (SELECT `id` FROM `objects_lib_subcategory` WHERE `id_category`='" + categoryId.ToString() + "')";
                    m_mySql.NonQuery(queryStr);
                    // удаляем сами подкатегории
                    queryStr = "DELETE FROM `objects_lib_subcategory` WHERE `id_category`='" + categoryId.ToString() + "'";
                    m_mySql.NonQuery(queryStr);
                    // удаляем категорию
                    queryStr = "DELETE FROM `objects_lib_category` WHERE `id`='" + categoryId.ToString() + "'";
                    m_mySql.NonQuery(queryStr);
                }
            }
            else if (subCategoryId != -1)
            {
                if (m_history.ProcessHistory(m_mySql, false) == false)
                {
                    System.Windows.Forms.MessageBox.Show("Удаление подкатегории из библиотеки доступно только после сохранения.", "Предупреждение", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
                }
                else
                {
                    // удаляем объекты в подкатерогии
                    String queryStr = "DELETE FROM `objects_lib` WHERE `id_subcategory`='" + subCategoryId.ToString() + "'";
                    m_mySql.NonQuery(queryStr);
                    // удаляем подкатегорию
                    queryStr = "DELETE FROM `objects_lib_subcategory` WHERE `id`='" + subCategoryId.ToString() + "'";
                    m_mySql.NonQuery(queryStr);
                }
            }
        }

        public void DeleteObjectFromLib(int objectId)
        {
            if(m_history.ProcessHistory(m_mySql, false) == false)
            {
                System.Windows.Forms.MessageBox.Show("Удаление объектов из библиотеки доступно только после сохранения.", "Предупреждение", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
            }
            else
            {
                String queryStr = "DELETE FROM `objects_lib` WHERE `id`='" + objectId.ToString() + "'";
                m_mySql.NonQuery(queryStr);
            }
        }

        public ArrayList GetLibStructure()
        {
            ArrayList categoryStruct = null;

            ArrayList categoryList = GetCategoryList();

            if(categoryList == null)
            {
                categoryList = new ArrayList();
            }

            categoryStruct = new ArrayList();
            foreach (Hashtable categoryInfo in categoryList)
            {
                ArrayList subcategoryStruct = new ArrayList();

                String categoryName = (String)categoryInfo["name"];
                int categoryId = (Int32)(UInt32)categoryInfo["id"];

                subcategoryStruct.Add(categoryName);

                ArrayList subcategoryList = GetSubCategoryList(categoryId);
                if (subcategoryList != null)
                {
                    foreach (Hashtable subcategoryInfo in subcategoryList)
                    {
                        subcategoryStruct.Add(subcategoryInfo["name"]);
                    }
                }

                categoryStruct.Add(subcategoryStruct);
            }

            return categoryStruct;
        }

        public void AddObjectToWorld(int objectId)
        {
            AddObjectToWorld(objectId, null);
        }

        // подбор нового имени для размещаемого объекта
        string GetNextObjectName(string objName)
        {
            string nextObjName = objName + "_";

            int number = 1;
            string newName = "";
            while (true)
            {
                newName = nextObjName + number.ToString();
                if (m_usedNames[newName] == null)
                {
                    String queryStr = "SELECT id FROM `objects_map` WHERE `name`= '" + newName + "'";
                    ArrayList aResult = m_mySql.QueryA(queryStr);
                    if (aResult == null)
                    {
                        break;
                    }
                }

                number++;
            }

            nextObjName += number.ToString();
            m_usedNames[nextObjName] = 1;
    
	        return nextObjName;        
	    }

        public void AddObjectToWorld(int objectId, Vector coords)
        {
            String queryStr = "SELECT * FROM `objects_lib` WHERE `id`=" + objectId.ToString();
            ArrayList aResult = m_mySql.QueryA(queryStr);

            if ((aResult == null) || (aResult.Count == 0))
            {
                MessageBox.Show("Ошибка", "Не найден объект c id = " + objectId.ToString() + " в библиотеке. Код ошибки 1");
                return;
            }

            Hashtable objectInfo = (Hashtable)aResult[0];

            if (objectInfo == null)
            {
                MessageBox.Show("Ошибка", "Не найден объект c id = " + objectId.ToString() + " в библиотеке. Код ошибки 1");
                return;
            }

            DataWriter addObjectCommand = new DataWriter();

            addObjectCommand.AddData(CMD_OBJECT_CREATE);

            byte createFlag = 1;
            if(coords != null)
            {
                createFlag = 3;
            }
            addObjectCommand.AddData(createFlag);

            string name = (String)objectInfo["name"];

            addObjectCommand.AddData(name);
            // на будущее можно дописать поддержку коллективной работы с одним и тем же типом объекта
            // сейчас имена могут пересечься
            // или делать проверку при сохранении
            addObjectCommand.AddData(GetNextObjectName(name));
            addObjectCommand.AddData((String)objectInfo["res"]);
            addObjectCommand.AddData((Int32)objectInfo["level"]);
            addObjectCommand.AddData((String)objectInfo["class"]);
            addObjectCommand.AddData((String)objectInfo["params"]);
            addObjectCommand.AddData(objectId);

            if(coords != null)
            {
                addObjectCommand.AddData(coords.X);
                addObjectCommand.AddData(coords.Y);
                addObjectCommand.AddData(coords.Z);
            }           

            m_pipeServer.addCommand(addObjectCommand);
        }

        private void AddNewObjectToHistory(DataReader newObjectInfo)
        {
            // id, имя, координаты
            int objectLibId = 0;
            newObjectInfo.Read(ref objectLibId);

            String objectName = "";
            newObjectInfo.Read(ref objectName);

            Vector objectCoords = new Vector();
            float xyz = 0;
            newObjectInfo.Read(ref xyz);
            objectCoords.X = xyz;
            newObjectInfo.Read(ref xyz);
            objectCoords.Y = xyz;
            newObjectInfo.Read(ref xyz);
            objectCoords.Z = xyz;

            // предыдущее выделение
            short objectsCount = 0;
            newObjectInfo.Read(ref objectsCount);
            String[] strParams = new String[objectsCount + 1];
            strParams[0] = objectLibId.ToString();

            for (int objIndex = 1; objIndex < objectsCount + 1;  objIndex++)
            {
                String prevObjName = "";
                newObjectInfo.Read(ref prevObjName);
                strParams[objIndex] = prevObjName;
            }

            m_history.AddCreate(objectName, strParams, objectCoords);
        }

        public void RefreshObjects()
        {
            m_materialFormInstance.SaveMaterial();

            DataWriter refreshCommand = new DataWriter();

            refreshCommand.AddData(Commander.CMD_REFRESH);

            m_pipeServer.addCommand(refreshCommand);
        }

        public void TeleportToNextSelectedObject()
        {
            DataWriter teleportCommand = new DataWriter();

            teleportCommand.AddData(Commander.CMD_TELEPORT);

            m_pipeServer.addCommand(teleportCommand);
        }

        public void TeleportByCoordsToNextSelectedObject(String objName, bool isExactSearch)
        {
            ArrayList resRows;

            if (isExactSearch)
                resRows = GetCurObjectORGroupInfo(objName);
            else
                resRows = GetCurObjectNearlyInfo(objName);

            if (resRows!=null)
            {
                // надо отослать клиенту координаты для телепортации аватара к объекту
                DataWriter teleportCommand = new DataWriter();

                teleportCommand.AddData(Commander.CMD_TELEPORT_BY_COORDS);

                Hashtable obj = (Hashtable)resRows[0];

                int coordX = (int)(float.Parse(obj["px"].ToString()) - float.Parse(obj["bx"].ToString()) / 2 - 100);

                int zoneX, zoneY;

                m_history.GetZoneCenter((Int64)obj["sqId"], out zoneX, out zoneY);

                int newX = (int)(coordX + zoneX);
                int newY = (int)(float.Parse(obj["py"].ToString()) + zoneY);
                int newZ = (int)(float.Parse(obj["pz"].ToString()));

                teleportCommand.AddData(newX);
                teleportCommand.AddData(newY);
                teleportCommand.AddData(newZ);

                m_pipeServer.addCommand(teleportCommand);
            }

        }

        private void SaveMoveResult(DataReader moveResult)
        {
            Vector moveDelta = new Vector();
            float xyz = 0;

            moveResult.Read(ref xyz);
            moveDelta.X = xyz;

            moveResult.Read(ref xyz);
            moveDelta.Y = xyz;

            moveResult.Read(ref xyz);
            moveDelta.Z = xyz;

            m_history.AddMove(m_object.Names, moveDelta);
        }

        private void SetServer(DataReader helloInfo)
        {
            String serverURL = "";
            helloInfo.Read(ref serverURL);
            if (serverURL != "192.168.0." + Options.GetCurrentServerNumber().ToString())
            {   
                m_mainFormInstance.ChangeServer(serverURL);
            }
        }

        public Vector GetCurrentCameraEye()
        {
            Vector res = new Vector();
            res.X = m_eyeX;
            res.Y = m_eyeY;
            res.Z = m_eyeZ;

            return res;
        }

        public void UpdateConnection()
        {
            m_mySql.Connect();
        }

        public Vector GetCurrentCameraLookAt()
        {
            Vector res = new Vector();
            res.X = m_lookAtX;
            res.Y = m_lookAtY;
            res.Z = m_lookAtZ;

            return res;
        }

        public void SendNewDragMode(int dragMode)
        {
            DataWriter dragModeCommand = new DataWriter();
            dragModeCommand.AddData(Commander.CMD_DRAGMODE);
            dragModeCommand.AddData(dragMode);

            m_pipeServer.addCommand(dragModeCommand);
        }

        public void SendChangeClass(String oldClass, String newClass)
        {
            m_history.AddClass(oldClass, newClass);
        }

        public void SendChangeParams(String oldParams, String newParams)
        {
            m_history.AddParams(oldParams, newParams);
        }

        public void SendChangeLockLevel(Int32 oldLockLevel, Int32 newLockLevel)
        {
            if ((1 == newLockLevel) || (3 == newLockLevel))
            {
                SendNewDragMode(0);
            }
            m_history.AddLockLevel(oldLockLevel, newLockLevel);
            m_object.ModifyLevel = newLockLevel;
        }

        public int GetModifyLevel(String objName)
        {
            Object lockLevel = m_history.FindChangedProp(objName, Commander.CMD_LOCK);
            if (lockLevel != null)
            {
                return (int)lockLevel;
            }
            else
            {
                return GetLockLevel(objName);
            }                
        }

        public void SetObjectsLevel(String objName, int isCheck, byte[] pathBytes)
        {
            if (objName == "")
            {
                return;
            }

            String queryStr = "SELECT `level` FROM `objects_map` WHERE `name`='"+objName+"'";

            ArrayList result = m_mySql.QueryA(queryStr);  

            if (result != null)
            {
                int level = (int)(((Hashtable)result[0])["level"]);

                if ((level < 100)&& (isCheck == 1)) level = level + 100;
                else if ((level >= 100)&& (isCheck == 0)) level = level - 100;

                queryStr = "UPDATE `objects_map` SET `level`='" + level + "' WHERE `name`='" + objName + "'";
                m_mySql.NonQuery(queryStr);


                DataWriter switchOffPathesCommand = new DataWriter();
                switchOffPathesCommand.AddData(Commander.CMD_SWITCH_OFF_PATHES);
                switchOffPathesCommand.AddData(Object3D.GetObject3D().Names[0]);
                switchOffPathesCommand.AddData(isCheck);

                if ((isCheck == 0) && (pathBytes != null))
                 for (int i=0; i< pathBytes.Length; i++)
                    switchOffPathesCommand.AddData((byte)pathBytes[i]);

                m_pipeServer.addCommand(switchOffPathesCommand);
            }
        }

        private int GetLockLevel(String objName)
        {
            if (objName == "")
            {
                return 0;
            }

            String queryStr = "SELECT * FROM `objects_map_lock` WHERE `object_name`='"+objName+"'";

            ArrayList result = m_mySql.QueryA(queryStr);
            if (result != null)
            {
                if (result.Count == 0)
                {
                    return 0;
                }

                Hashtable lockInfo = (Hashtable)result[0];
                if (lockInfo == null)
                {
                    return 0;
                }

                if (lockInfo["lock_level"] == null)
                {
                    return 0;
                }

                return (int)lockInfo["lock_level"];
            }            

            return 0;
        }

        public int CopyLibCategoryTo(int categoryId, int subcategoryId, int serverId)
        {
            ArrayList objectsArray = null;
            String queryStr = "";

            if(categoryId != -1)
            {
                queryStr = "SELECT `id` FROM `objects_lib` WHERE `id_subcategory` IN (SELECT `id` FROM `objects_lib_subcategory` WHERE `id_category`='" + categoryId.ToString() + "')";
                objectsArray = m_mySql.QueryA(queryStr);
            }
            else if (subcategoryId != -1)
            {
                queryStr = "SELECT `id` FROM `objects_lib` WHERE `id_subcategory`='" + subcategoryId.ToString() + "'";
                objectsArray = m_mySql.QueryA(queryStr);               
            }

            foreach(Hashtable objectInfo in objectsArray)
            {
                int objectId = (Int32)(UInt32)objectInfo["id"];
                CopyLibObjectTo(objectId, serverId);
            }

            return 0;
        }

        public int CopyLibObjectTo(int objectId, int serverId)
        {            
            byte servNum = 0;
            String copyPath = ServerPath.GetServerPath(serverId.ToString(), ref servNum);

            if(copyPath != "")
            {
                copyPath += ServerPath.geometryDir;
                String queryStr = "SELECT * FROM `objects_lib` WHERE `id`='" + objectId.ToString() + "'";
                ArrayList aResult = m_mySql.QueryA(queryStr);
                if(aResult == null)
                {
                    return 0;
                }
                Hashtable libObjectInfo = new Hashtable((Hashtable)aResult[0]);

                queryStr = "SELECT * FROM `objects_lib_subcategory` WHERE `id`='" + libObjectInfo["id_subcategory"].ToString() + "'";
                aResult = m_mySql.QueryA(queryStr);

                if(aResult == null)
                {
                    return 0;
                }
                Hashtable libSubcategoryInfo = new Hashtable((Hashtable)aResult[0]);

                queryStr = "SELECT * FROM `objects_lib_category` WHERE `id`='" + libSubcategoryInfo["id_category"].ToString() + "'";
                aResult = m_mySql.QueryA(queryStr);

                if (aResult == null)
                {
                    return 0;
                }

                Hashtable libCategoryInfo = new Hashtable((Hashtable)aResult[0]);
                
                ClientMySQL copyMySql = new ClientMySQL(servNum);
                copyMySql.Connect();

                // создать категорию и подкатегорию для объекта, если их нет
                queryStr = "SELECT * FROM `objects_lib_category` WHERE `name`='" + libCategoryInfo["name"].ToString() + "'";
                aResult = copyMySql.QueryA(queryStr);
                int copyCategoryId = -1;
                int copySubcategoryId = -1;
                if(aResult == null)
                {
                    queryStr = "INSERT INTO `objects_lib_category` SET `name`='" + libCategoryInfo["name"].ToString() + "'";
                    copyMySql.QueryA(queryStr);
                    // id категории
                    //queryStr = "SELECT LAST_INSERT_ID() as `last_id` FROM `objects_lib_category`";
                    queryStr = "SELECT LAST_INSERT_ID() as `last_id`";
                    aResult = copyMySql.QueryA(queryStr);
                    copyCategoryId = (Int32)(Int64)(((Hashtable)aResult[0])["last_id"]);
                }
                else
                {
                    copyCategoryId = (Int32)(UInt32)((Hashtable)aResult[0])["id"];

                    queryStr = "SELECT * FROM `objects_lib_subcategory` WHERE `name`='" + libSubcategoryInfo["name"].ToString() + "' AND `id_category`='" + copyCategoryId.ToString() + "'";
                    aResult = copyMySql.QueryA(queryStr);
                    if(aResult != null)
                    {
                        copySubcategoryId = (Int32)(UInt32)((Hashtable)aResult[0])["id"];
                    }
                }

                int overwrite = 1;
                String queryBeginStr = "INSERT INTO ";
                String queryEndStr = "";

                if(copySubcategoryId == -1)
                {
                    queryStr = "INSERT INTO `objects_lib_subcategory` SET `name`='" + libSubcategoryInfo["name"].ToString() + "', `id_category`='" + copyCategoryId.ToString() + "'";
                    copyMySql.QueryA(queryStr);
                    // id подкатегории
                    //queryStr = "SELECT LAST_INSERT_ID() as `last_id` FROM `objects_lib_subcategory`";
                    queryStr = "SELECT LAST_INSERT_ID() as `last_id`";
                    aResult = copyMySql.QueryA(queryStr);
                    copySubcategoryId = (Int32)(Int64)(((Hashtable)aResult[0])["last_id"]);
                }
                else
                {
                    // проверить наличие объекта в подкатегории
                    queryStr = "SELECT * FROM `objects_lib` WHERE `res`='" + libObjectInfo["res"].ToString().Replace("\\", "\\\\") + "' AND `id_subcategory`='" + copySubcategoryId.ToString() + "'";
                    aResult = copyMySql.QueryA(queryStr);
                    if (aResult != null)
                    {
                        DialogResult answer = MessageBox.Show("Объект '"+ libObjectInfo["name"].ToString() +"' уже есть в библиотеке. Заменить?", "Копирование в библиотеку", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                        if (answer == DialogResult.No)
                        {
                            overwrite = 0;
                        }
                        else if (answer == DialogResult.Yes)
                        {
                            queryBeginStr = "UPDATE ";
                            queryEndStr = " WHERE `id`='" + ((Hashtable)aResult[0])["id"].ToString() + "'";
                        }
                    }
                } 
              
                if(overwrite == 1)
                {
                    String objRes = ((String)libObjectInfo["res"]).Replace("\\", "\\\\");
                    queryStr = queryBeginStr + "`objects_lib` SET " +
                        "`id_subcategory`='" + copySubcategoryId.ToString() + "', " +
                        "`name`='" + libObjectInfo["name"].ToString() + "', " +
                        "`name_display`='" + libObjectInfo["name_display"].ToString() + "', " +
                        "`res`='" + objRes + "', " +
                        "`level`='" + libObjectInfo["level"].ToString() + "', " +
                        "`class`='" + libObjectInfo["class"].ToString() + "', " +
                        "`params`='" + libObjectInfo["params"].ToString() + "', " +
                        "`bx`='" + libObjectInfo["bx"].ToString() + "', " +
                        "`by`='" + libObjectInfo["by"].ToString() + "', " +
                        "`bz`='" + libObjectInfo["bz"].ToString() + "'" +                  
                        //"``='" + obj[""].ToString() + "', " +
                        queryEndStr;

                    Console.WriteLine(queryStr);
                    copyMySql.NonQuery(queryStr);

                    ArrayList processedRes = new ArrayList();

                    // копирование ресурсов объекта
                    String srcPath = ServerPath.GetServerPath(Options.GetHost());
                    srcPath += ServerPath.geometryDir;

                    FileSystem fs = new FileSystem();

                    String resPath = (String)libObjectInfo["res"];

                    // пути к ресурсам
                    ArrayList resPaths = new ArrayList();
                    String[] splittedRes = resPath.Split(';');

                    for (int splitItemIndex = 0; splitItemIndex < splittedRes.Length; splitItemIndex++)
                    {
                        if (splitItemIndex % 3 == 0)
                        {
                            if (splittedRes[splitItemIndex] == "")
                            {
                                break;
                            }

                            resPaths.Add(splittedRes[splitItemIndex].Substring(0, splittedRes[splitItemIndex].IndexOf("\\mesh.bmf.zip")));
                        }
                    }

                    for (int resIndex = 0; resIndex < resPaths.Count; resIndex++)
                    {
                        String srcResPath = srcPath + resPaths[resIndex] + "/";
                        String copyResPath = copyPath + resPaths[resIndex] + "/";

                        if (!processedRes.Contains(srcResPath))
                        {
                            fs.CopyDir(srcResPath, copyResPath);
                            processedRes.Add(srcResPath);

                            try
                            {
                                using (XmlReader texturesList = XmlReader.Create(srcResPath + "materials.xml"))
                                {
                                    texturesList.MoveToContent();
                                    while (texturesList.Read())
                                    {
                                        if ((texturesList.Name == "texture") || (texturesList.Name == "cubemap"))
                                        {
                                            String textureName = texturesList.ReadString();
                                            String textureFilePath = srcResPath + "../textures/" + textureName;
                                            if (!processedRes.Contains(textureFilePath))
                                            {
                                                fs.CopyFile(textureFilePath, copyResPath + "../textures/" + textureName);
                                                processedRes.Add(textureFilePath);
                                            }
                                        }
                                    }
                                }
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine(e.Message);
                            }
                        }
                    }
                }
            }

            return 0;
        }

        public int CopyObjectsTo(int dest)
        {
            // сохранение изменений перед копированием
            if (m_history.ProcessHistory(m_mySql, false) == false)
            {
                System.Windows.Forms.MessageBox.Show("Объекты скопируются без сделанных изменений.", "Предупреждение", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Information);
            }

            byte serverNumber = 0;
            String copyPath = ServerPath.GetServerPath(dest.ToString(), ref serverNumber);
 
            if(copyPath != "")
            {
                copyPath += ServerPath.geometryDir; // путь до геометрии

                String queryStr = "SELECT * FROM `objects_map` WHERE `name` IN (";

                for (int nameIndex = 0; nameIndex < m_object.Names.Length; nameIndex++)
                {
                    if (nameIndex != 0)
                    {
                        queryStr += ", ";
                    }
                    queryStr += "'" + m_object.Names[nameIndex] + "'";
                }

                queryStr += ")";

                ArrayList objectsInfo = m_mySql.QueryA(queryStr);

                // подключение к базе на целевом сервере
                ClientMySQL copyMySql = new ClientMySQL(serverNumber);
                copyMySql.Connect();

                int overwrite = 0;
                ArrayList processedRes = new ArrayList();

                for (int objIndex = 0; objIndex < objectsInfo.Count; objIndex++)
                {
                    Hashtable obj = (Hashtable)objectsInfo[objIndex];
                    // значение из objects_map_lock
                    obj["lock_level"] = GetLockLevel((String)obj["name"]);

                    queryStr = "SELECT `name` FROM `objects_map` WHERE `name`='" + (String)obj["name"] + "'";
                    String queryBeginStr = "";
                    String queryEndStr = "";
                    String newId = "";
                    if(copyMySql.QueryA(queryStr) != null)
                    {
                        // замена, не замена
                        if(overwrite != 2 && overwrite != -2)
                        {
                            overwrite = OverwriteDialog((String)obj["name"]);
                        }

                        if (overwrite == 0) // отмена
                        {
                            break;
                        }                                              

                        if(overwrite == -1 || overwrite == -2) // пропустить
                        {
                            continue;
                        }

                        // перезаписать при 1 или 2
                        queryBeginStr = "UPDATE ";
                        queryEndStr = " WHERE `name`='" + (String)obj["name"] + "'";
                    }
                    else
                    {
                        // простое копирование
                        queryBeginStr = "INSERT INTO ";
                        //queryStr = "SELECT max(id) as `id` FROM `objects_map` WHERE id < 100000000";
					    queryStr = "SELECT max(t.id_max) as `id` FROM (" +
					               "SELECT max(id) as `id_max` FROM `objects_map` WHERE `id`<'100000000' union all " +
					               "SELECT max(id_object) as `id_max` FROM `objects_map_changelog_items` WHERE `id_object`<'100000000') as `t`";

                        ArrayList aResult = copyMySql.QueryA(queryStr);
                        UInt32 id = 0;
                        if(aResult != null)
                        {
                            Hashtable row = (Hashtable)(aResult[0]);
                            id = (UInt32)row["id"] + 1;
                        }                        
                        newId = "`id`='" + id.ToString() + "', ";
                    }

                    String objRes = ((String)obj["res"]).Replace("\\", "\\\\");
                    queryStr = queryBeginStr + "`objects_map` SET " + newId +
                        "`type`='"+obj["type"].ToString()+ "', " +
                        "`res`='" + objRes + "', " +
                        "`name`='"+obj["name"].ToString()+"', "+
                        //"`reference`='"+obj["reference"].ToString()+"', "+
                        "`level`='"+obj["level"].ToString()+"', "+
                        //"`lod`='"+obj["lod"].ToString()+"', "+
                        "`class`='"+obj["class"].ToString()+"', "+
                        //"`version`='"+obj["version"].ToString()+"', "+
                        "`px`='"+obj["px"].ToString()+"', "+
                        "`py`='"+obj["py"].ToString()+"', "+
                        "`pz`='"+obj["pz"].ToString()+"', "+
                        "`rx`='"+obj["rx"].ToString()+"', "+
                        "`ry`='"+obj["ry"].ToString()+"', "+
                        "`rz`='"+obj["rz"].ToString()+"', "+
                        "`rw`='"+obj["rw"].ToString()+"', "+
                        "`bx`='"+obj["bx"].ToString()+"', "+
                        "`by`='"+obj["by"].ToString()+"', "+
                        "`bz`='"+obj["bz"].ToString()+"', "+
                        "`sqId`='"+obj["sqId"].ToString()+"', "+
                        "`params`='" + obj["params"].ToString() + "', " +
                        //"`grass`='" + obj["grass"].ToString() + "', " +
                        "`scalex`='" + obj["scalex"].ToString() + "', " +
                        "`scaley`='" + obj["scaley"].ToString() + "', " +
                        "`scalez`='" + obj["scalez"].ToString() + "' " +
                        //"``='" + obj[""].ToString() + "', " +
                        queryEndStr;

                    Console.WriteLine(queryStr);
                    copyMySql.NonQuery(queryStr);

                    queryStr = "DELETE FROM `objects_map_lock` WHERE `object_name`='" + obj["name"].ToString() + "'";
                    copyMySql.NonQuery(queryStr);
                    queryStr = "INSERT INTO `objects_map_lock` SET `object_name`='" + obj["name"].ToString() + "', `lock_level`='" + obj["lock_level"].ToString() + "'";
                    copyMySql.NonQuery(queryStr);

                    //копирование файлов с заменой
                    String srcPath = ServerPath.GetServerPath(Options.GetHost());
                    srcPath += ServerPath.geometryDir;

                    FileSystem fs = new FileSystem();

                    String resPath = (String)obj["res"];

                    // пути к ресурсам
                    ArrayList resPaths = new ArrayList();
                    String[] splittedRes = resPath.Split(';');

                    for (int splitItemIndex = 0; splitItemIndex < splittedRes.Length; splitItemIndex++)
                    {
                        if (splitItemIndex % 3 == 0)
                        {
                            if (splittedRes[splitItemIndex] == "")
                            {
                                break;
                            }

                            resPaths.Add(splittedRes[splitItemIndex].Substring(0, splittedRes[splitItemIndex].IndexOf("\\mesh.bmf.zip")));
                        }
                    }

                    for (int resIndex = 0; resIndex < resPaths.Count; resIndex++ )
                    {
                        String srcResPath = srcPath + resPaths[resIndex] + "/";
                        String copyResPath = copyPath + resPaths[resIndex] + "/";

                        if (!processedRes.Contains(srcResPath))
                        {
                            fs.CopyDir(srcResPath, copyResPath);
                            processedRes.Add(srcResPath);

                            try
                            {
                                using (XmlReader texturesList = XmlReader.Create(srcResPath + "materials.xml"))
                                {
                                    texturesList.MoveToContent();
                                    while (texturesList.Read())
                                    {
                                        if ((texturesList.Name == "texture") || (texturesList.Name == "cubemap"))
                                        {
                                            String textureName = texturesList.ReadString();
                                            String textureFilePath = srcResPath + "../textures/" + textureName;
                                            if (!processedRes.Contains(textureFilePath))
                                            {
                                                fs.CopyFile(textureFilePath, copyResPath + "../textures/" + textureName);
                                                processedRes.Add(textureFilePath);
                                            }
                                        }
                                    }
                                }
                            }
                            catch(Exception e)
                            {
                                Console.WriteLine(e.Message);
                            }

                            // чтение списка всех материалов в mesh.ini
                            try
                            {
                                ArrayList materialsList = new ArrayList();
                                String materialName;

                                using (StreamReader materialsAdditionalList = new StreamReader(srcResPath + "mesh.ini"))
                                {
                                    while ((materialName = materialsAdditionalList.ReadLine()) != null && materialName != "pathlines.bin")
                                    {
                                        materialsList.Add(materialName);
                                    }
                                }
                                

                                    // чтение файлов дополнительных материалов и копирование текстур
                                    try
                                    {
                                        foreach (String i in materialsList)
                                        {
                                            using (XmlReader texturesAdditionalList = XmlReader.Create(srcResPath + i))
                                            {

                                                texturesAdditionalList.MoveToContent();
                                                while (texturesAdditionalList.Read())
                                                {
                                                    if ((texturesAdditionalList.Name == "texture") || (texturesAdditionalList.Name == "cubemap"))
                                                    {
                                                        texturesAdditionalList.MoveToAttribute("src");

                                                        // если присутствует возвращаемый узел
                                                        if (texturesAdditionalList.ReadAttributeValue())
                                                        {
                                                            String textureAdditionalName = texturesAdditionalList.Value;
                                                            String textureAdditionalFilePath = srcResPath + "../textures/" + textureAdditionalName;
                                                            if (!processedRes.Contains(textureAdditionalFilePath))
                                                            {
                                                                fs.CopyFile(textureAdditionalFilePath, copyResPath + "../textures/" + textureAdditionalName);
                                                                processedRes.Add(textureAdditionalFilePath);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    catch (Exception e)
                                    {
                                        Console.WriteLine(e.Message);
                                    }
                            }
                            catch (IOException e)
                            {
                                Console.WriteLine(e.Message);
                            }
                        }                                              
                    }            
 
                }

                if(overwrite != 0)
                {                    
                    MessageBox.Show("Операция выполнена успешно.", "Копирование объектов", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }

                mainForm.ReloadServers((byte)dest);
            }
            else
            {
                MessageBox.Show("Не выбрана папка для копирования.", "Копирование объектов", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            return 0;
        }

        private int OverwriteDialog(String objName)
        {
            OverwriteMessage message = new OverwriteMessage();
            DialogResult result = message.ShowDialog();

            if(result == DialogResult.Cancel)
            {
                return 0;
            }

            if(result == DialogResult.OK)
            {
                return 1;
            }

            if(result == DialogResult.No)
            {
                return -1;
            }

            if(result == DialogResult.Ignore)
            {
                return 2;
            }

            if(result == DialogResult.Abort)
            {
                return -2;
            }

            return 0;
        }

        private void ProcessKey(DataReader keyInfo)
        {
            int key = 0;
            keyInfo.Read(ref key);

            byte ctrl = 0;
            keyInfo.Read(ref ctrl);

            byte alt = 0;
            keyInfo.Read(ref alt);

            byte shift = 0;
            keyInfo.Read(ref shift);

            if ((key >= (int)System.Windows.Forms.Keys.F2) && (key <= (int)System.Windows.Forms.Keys.F8))
            {                
                m_mainFormInstance.ImitateKeyDown(key);
            }            

            switch(key)
            {
                case (int)System.Windows.Forms.Keys.Delete:
                    {
                        DeleteObject();                        
                        break;
                    }
                case (int)'S':
                    {
                        if(ctrl == 1)
                        {
                            this.Save(false);
                        }
                        break;
                    }
                case (int)'I':
                    {
                        if (ctrl == 1)
                        {
                            m_mainFormInstance.OpenSynchPage();
                        }
                        break;
                    }
                case (int)'Z':
                    {
                        if (ctrl == 1)
                        {
                            this.Undo();
                        }
                        break;
                    }
                case (int)'Y':
                    {
                        if (ctrl == 1)
                        {
                            this.Redo();
                        }
                        break;
                    }
                case (int)'C':
                    {                      
                        if (ctrl == 1)
                        {
                            this.SendObjectCreateCommand(true);
                        }
                        break;
                    }
                case (int)'M':
                    {
                        if (alt == 1)
                        {
                            m_mainFormInstance.OpenMaterialDialog();
                        }
                        break;
                    }
                case (int)System.Windows.Forms.Keys.Tab:
                    {
                        if(ctrl == 1)
                        {
                            this.TeleportToNextSelectedObject();
                        }
                        break;
                    }
                case (int)'F':
                    {
                        if(ctrl == 1)
                        {
                            if(shift == 1)
                            {
                                m_mainFormInstance.ChangeFreeCam();
                                break;
                            }

                            m_mainFormInstance.ChangeFreezing();
                        }
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }
    }
}
