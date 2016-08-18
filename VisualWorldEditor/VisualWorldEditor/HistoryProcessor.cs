using System;
using System.Collections;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    class HistoryProcessor
    {
        private HistoryStorage m_storage;

        private static readonly HistoryProcessor instance = new HistoryProcessor();

        public static HistoryProcessor GetHProcessor()
        {
            return instance;
        }

        private HistoryProcessor()
        {
            m_storage = new HistoryStorage(500);
        }

        public HistoryStorage GetStorage()
        {
            return m_storage;
        }

        public void AddDelete(String[] objects)
        {
            HistoryElement deleteAction = new HistoryElement(Commander.CMD_DELETE, objects);
            m_storage.AddAction(deleteAction);
        }

        public void AddSelect(String[] objects)
        {
            HistoryElement selectAction = new HistoryElement(Commander.CMD_SELECT, objects);
            m_storage.AddAction(selectAction);
        }

        public void AddRename(String oldName, String newName, ClientMySQL mySql)
        {
            String queryStr = "SELECT * FROM `objects_map` WHERE `name`='" + newName + "'";
            ArrayList aResult = mySql.QueryA(queryStr);

            if (((aResult != null) && (aResult.Count != 0) && !m_storage.IsAlreadyRenamed(newName)) || (m_storage.CheckDuplicateName(newName)))
            {
                MessageBox.Show("Не удалось переименовать объект, так как объект с таким именем уже существует.", "Ошибка переименования объекта", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            HistoryElement renameAction = new HistoryElement(Commander.CMD_RENAME, new String[] { oldName }, new String[] { newName });

           
            m_storage.AddAction(renameAction);
        }

        public void AddMove(String[] objects, Vector moveDelta)
        {
            HistoryElement moveAction = new HistoryElement(Commander.CMD_MOVE, objects, moveDelta);
            m_storage.AddAction(moveAction);
        }

        public void AddRotate(String[] objects, Vector angles, Hashtable rotates)
        {
            ArrayList paramsArray = new ArrayList();
            paramsArray.Add(angles);
            paramsArray.Add(rotates);
            HistoryElement rotateAction = new HistoryElement(Commander.CMD_ROTATE, objects, paramsArray);
            m_storage.AddAction(rotateAction);
        }

        public void AddChangeCollision(int collisionFlag)
        {
            HistoryElement changeCollisionAction = new HistoryElement(Commander.CMD_CHANGE_COLLISION, Object3D.GetObject3D().Names, collisionFlag);
            m_storage.AddAction(changeCollisionAction);
        }

        public void AddGroup(String objectName, String groupName, String groupOldName, int groupType)
        {
            HistoryElement groupAction = new HistoryElement(Commander.CMD_SET_GROUP,
                new String[] { objectName, groupName, groupOldName },
                groupType);
            m_storage.AddAction(groupAction);
        }

        public void AddScale(String[] objects, Vector scaleDelta)
        {
            HistoryElement scaleAction = new HistoryElement(Commander.CMD_SCALE, objects, scaleDelta);
            m_storage.AddAction(scaleAction);
        }

        public void AddShadow(String[] objects, Object3D.LodShadow shadowState)
        {
            HistoryElement shadowAction = new HistoryElement(Commander.CMD_SHADOW, objects, shadowState);
            m_storage.AddAction(shadowAction);
        }

        public void AddCopy(String[] objects, ArrayList newObjects)
        {
            HistoryElement copyAction = new HistoryElement(Commander.CMD_OBJECT_COPY, objects, newObjects);
            m_storage.AddAction(copyAction);
        }

        public void AddCreate(String objectName, String[] strParams, Vector coords)
        {
            HistoryElement createAction = new HistoryElement(Commander.CMD_OBJECT_CREATE,
                new String[]{objectName},
                coords,
                strParams);
            m_storage.AddAction(createAction);
        }

        public void AddClass(String oldClass, String newClass)
        {
            HistoryElement classAction = new HistoryElement(Commander.CMD_CLASS,
                Object3D.GetObject3D().Names,
                new String[]{oldClass, newClass});

            m_storage.AddAction(classAction);
        }

        public void AddParams(String oldParams, String newParams)
        {
            HistoryElement paramsAction = new HistoryElement(Commander.CMD_PARAMS,
                Object3D.GetObject3D().Names,
                new String[] { oldParams, newParams });

            m_storage.AddAction(paramsAction);
        }

        public void AddLockLevel(Int32 oldLockLevel, Int32 newLockLevel)
        {
            HistoryElement lockAction = new HistoryElement(Commander.CMD_LOCK,
                Object3D.GetObject3D().Names,
                new Int32[] { oldLockLevel, newLockLevel });

            m_storage.AddAction(lockAction);
        }

        public void AddMaterial()
        {
            HistoryElement materialAction = new HistoryElement(Commander.CMD_MATERIAL, Object3D.GetObject3D().Names);
            m_storage.AddAction(materialAction);
        }

        public DataWriter GetUndoCommand()
        {
            DataWriter undoCommand = null;

            HistoryElement undoAction = m_storage.Undo();
            if(undoAction != null)
            {
                undoCommand = new DataWriter();

                undoCommand.AddData(undoAction.cmdId);

                switch(undoAction.cmdId)
                {
                    case Commander.CMD_SELECT:
                        {
                            // поиск предыдущего выделения, количество выделенных объектов и их имена
                            bool prevSelectFinded = false;
                            for (int storageIndex = m_storage.GetHistoryPosition(); storageIndex >= 0; storageIndex--)
                            {
                                undoAction = m_storage.GetHistoryElement(storageIndex);
                                if(undoAction != null)
                                {
                                    if (undoAction.cmdId == Commander.CMD_OBJECT_COPY || undoAction.cmdId == Commander.CMD_OBJECT_CREATE)
                                    {
                                        prevSelectFinded = true;
                                        break;
                                    }

                                    if (undoAction.cmdId == Commander.CMD_SELECT || undoAction.cmdId == Commander.CMD_RENAME)
                                    {
                                        prevSelectFinded = true;
                                        break;
                                    }
                                    else if (undoAction.cmdId == Commander.CMD_DELETE)
                                    {
                                        Object3D.GetObject3D().DeleteInfo();
                                    }
                                }                                
                            }

                            if(prevSelectFinded)
                            {                               
                                undoCommand.AddData((short)undoAction.objNames.Length);
                                for (int nameIndex = 0; nameIndex < undoAction.objNames.Length; nameIndex++)
                                {
                                    if(undoAction.cmdId == Commander.CMD_RENAME)
                                    {
                                        undoCommand.AddData(undoAction.cmdParams[nameIndex]);
                                    }
                                    else if (undoAction.cmdId == Commander.CMD_OBJECT_COPY)
                                    {
                                        undoCommand.AddData((String)((ArrayList)undoAction.cmdParam)[nameIndex*2]);
                                    }
                                    else if (undoAction.cmdId == Commander.CMD_OBJECT_CREATE)
                                    {
                                        undoCommand.AddData(undoAction.objNames[nameIndex]);
                                    }
                                    else
                                    {
                                        undoCommand.AddData(undoAction.objNames[nameIndex]);
                                    }                                    
                                }
                            }
                            else
                            {
                                Object3D.GetObject3D().DeleteInfo();

                                undoCommand.AddData((short)1);
                                String emptyString = "";
                                undoCommand.AddData(emptyString);
                            }

                            break;
                        }
                    case Commander.CMD_DELETE:
                        {
                            // количество удаленных объетов и их имена
                            undoCommand.AddData((short)undoAction.objNames.Length);
                            for (int nameIndex = 0; nameIndex < undoAction.objNames.Length; nameIndex++)
                            {
                                undoCommand.AddData(undoAction.objNames[nameIndex]);
                            }
                            break;
                        }
                    case Commander.CMD_RENAME:
                        {
                            // передача текущего и нового имен
                            undoCommand.AddData(undoAction.cmdParams[0]);
                            undoCommand.AddData(undoAction.objNames[0]);
                            break;
                        }
                    case Commander.CMD_MOVE:
                        {
                            // объект уже активен, шлем изменения координат
                            Vector deltaMove = (Vector)undoAction.cmdParam;
                            undoCommand.AddData(-deltaMove.X);
                            undoCommand.AddData(-deltaMove.Y);
                            undoCommand.AddData(-deltaMove.Z);
                            break;
                        }
                    case Commander.CMD_ROTATE:
                        {
                            ArrayList rotateParams = (ArrayList)undoAction.cmdParam;
                            Vector rotateDelta = (Vector)rotateParams[0];
                            undoCommand.AddData(-rotateDelta.X);
                            undoCommand.AddData(-rotateDelta.Y);
                            undoCommand.AddData(-rotateDelta.Z);
                            break;
                        }
                    case Commander.CMD_SCALE:
                        {
                            Vector deltaScale = (Vector)undoAction.cmdParam;
                            undoCommand.AddData(-deltaScale.X);
                            undoCommand.AddData(-deltaScale.Y);
                            undoCommand.AddData(-deltaScale.Z);
                            break;
                        }
                    case Commander.CMD_SHADOW:
                        {
                            Object3D.LodShadow lodLevelInfo = (Object3D.LodShadow)undoAction.cmdParam;
                            byte lodLevel = (byte)lodLevelInfo.distance;
                            undoCommand.AddData(lodLevel);
                            byte shadowState = 1;
                            if (lodLevelInfo.shadow == 1)
                            {
                                shadowState = 0;
                            }
                            undoCommand.AddData(shadowState);

                            ArrayList shadowList = Object3D.GetObject3D().ShadowsList;
                            float distance = ((Object3D.LodShadow)shadowList[lodLevel]).distance;
                            string meshName = ((Object3D.LodShadow)shadowList[lodLevel]).meshName;
                            shadowList[lodLevel] = new Object3D.LodShadow(distance, shadowState, meshName);
                            Object3D.GetObject3D().ShadowsList = shadowList;
                            break;
                        }
                    case Commander.CMD_OBJECT_COPY:
                        {
                            byte undoCreateFlag = 2;
                            undoCommand.AddData(undoCreateFlag);

                            //undoCommand.AddData((short)undoAction.objNames.Length);
                            for (int objectIndex = 0; objectIndex < undoAction.objNames.Length; objectIndex++)
                            {
                                undoCommand.AddData(undoAction.objNames[objectIndex]);
                            }
                            Object3D.GetObject3D().DeleteInfo();
                            break;
                        }
                    case Commander.CMD_OBJECT_CREATE:
                        {
                            byte undoCreateFlag = 2;
                            undoCommand.AddData(undoCreateFlag);
                            undoCommand.AddData(undoAction.objNames[0]);

                            // объекты предыдущего выделения
                            undoCommand.AddData((short)(undoAction.cmdParams.Length - 1));
                            for (int objectIndex = 1; objectIndex < undoAction.cmdParams.Length; objectIndex++)
                            {
                                undoCommand.AddData(undoAction.cmdParams[objectIndex]);
                            }
                            Object3D.GetObject3D().DeleteInfo();                            
                            break;
                        }
                    case Commander.CMD_SET_GROUP:
                        {
                            byte isUndo = 1;
                            undoCommand.AddData(undoAction.objNames[0]);
                            undoCommand.AddData(undoAction.objNames[2]);
                            undoCommand.AddData((int)undoAction.cmdParam);
                            undoCommand.AddData(isUndo);
                            break;
                        }
                    case Commander.CMD_CLASS:
                        {
                            break;
                        }
                    case Commander.CMD_PARAMS:
                        {
                            break;
                        }
                    case Commander.CMD_LOCK:
                        {
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }

            return undoCommand;
        }

        public DataWriter GetRedoCommand()
        {
            DataWriter redoCommand = null;

            HistoryElement redoAction = m_storage.Redo();
            if(redoAction != null)
            {
                redoCommand = new DataWriter();

                redoCommand.AddData(redoAction.cmdId);

                switch(redoAction.cmdId)
                {
                    case Commander.CMD_SELECT:
                        {
                            redoCommand.AddData((short)redoAction.objNames.Length);
                            for (int nameIndex = 0; nameIndex < redoAction.objNames.Length; nameIndex++)
                            {
                                redoCommand.AddData(redoAction.objNames[nameIndex]);
                            }
                            break;
                        }
                    case Commander.CMD_DELETE:
                        {
                            redoCommand.AddData((short)redoAction.objNames.Length);
                            for (int nameIndex = 0; nameIndex < redoAction.objNames.Length; nameIndex++)
                            {
                                redoCommand.AddData(redoAction.objNames[nameIndex]);
                            }
                            Object3D.GetObject3D().DeleteInfo();
                            break;
                        }
                    case Commander.CMD_RENAME:
                        {
                            redoCommand.AddData(redoAction.objNames[0]);
                            redoCommand.AddData(redoAction.cmdParams[0]);
                            break;
                        }
                    case Commander.CMD_MOVE:
                        {
                            // объект уже активен, шлем изменения координат
                            Vector deltaMove = (Vector)redoAction.cmdParam;
                            redoCommand.AddData(deltaMove.X);
                            redoCommand.AddData(deltaMove.Y);
                            redoCommand.AddData(deltaMove.Z);
                            break;
                        }
                    case Commander.CMD_ROTATE:
                        {                            
                            ArrayList rotateParams = (ArrayList)redoAction.cmdParam;
                            Vector rotateDelta = (Vector)rotateParams[0];
                            redoCommand.AddData(rotateDelta.X);
                            redoCommand.AddData(rotateDelta.Y);
                            redoCommand.AddData(rotateDelta.Z);
                            break;
                        }
                    case Commander.CMD_SCALE:
                        {
                            Vector deltaScale = (Vector)redoAction.cmdParam;
                            redoCommand.AddData(deltaScale.X);
                            redoCommand.AddData(deltaScale.Y);
                            redoCommand.AddData(deltaScale.Z);
                            break;
                        }
                    case Commander.CMD_SHADOW:
                        {
                            Object3D.LodShadow lodLevelInfo = (Object3D.LodShadow)redoAction.cmdParam;
                            redoCommand.AddData((byte)lodLevelInfo.distance);
                            redoCommand.AddData((byte)lodLevelInfo.shadow);

                            ArrayList shadowList = Object3D.GetObject3D().ShadowsList;
                            float distance = ((Object3D.LodShadow)shadowList[(int)lodLevelInfo.distance]).distance;
                            string meshName = ((Object3D.LodShadow)shadowList[(int)lodLevelInfo.distance]).meshName;
                            shadowList[(int)lodLevelInfo.distance] = new Object3D.LodShadow(distance, lodLevelInfo.shadow, meshName);
                            Object3D.GetObject3D().ShadowsList = shadowList;
                            break;
                        }
                    case Commander.CMD_OBJECT_COPY:
                        {
                            byte copyState = 1;
                            redoCommand.AddData(copyState);
                            byte redoFlag = 1;
                            redoCommand.AddData(redoFlag);
                            break;
                        }
                    case Commander.CMD_OBJECT_CREATE:
                        {
                            Int32 libId = Int32.Parse(redoAction.cmdParams[0]);
                            Commander.GetCommander().AddObjectToWorld(libId, (Vector)redoAction.cmdParam);
                            redoCommand = null;
                            break;
                        }
                    case Commander.CMD_SET_GROUP:
                        {
                            byte isRedo = 1;
                            redoCommand.AddData(redoAction.objNames[0]);
                            redoCommand.AddData(redoAction.objNames[1]);
                            redoCommand.AddData((int)redoAction.cmdParam);
                            redoCommand.AddData(isRedo);
                            break;
                        }
                    case Commander.CMD_CLASS:
                        {
                            break;
                        }
                    case Commander.CMD_PARAMS:
                        {
                            break;
                        }
                    case Commander.CMD_LOCK:
                        {
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }

            return redoCommand;
        }

        public String FindObjectFirstName(String currentName)
        {
            if(!m_storage.isUndo())
            {
                return currentName;
            }

            String firstName = currentName;

            for (int i = m_storage.GetHistoryPosition(); i >= 0 ; i-- )
            {
                HistoryElement he = m_storage.GetHistoryElement(i);
                if(he.cmdId == Commander.CMD_RENAME)
                {
                    if(he.cmdParams[0] == firstName)
                    {
                        firstName = he.objNames[0];
                    }
                }
            }

            return firstName;
        }

        public String FindObjectLibId(String objectName)
        {
            if (!m_storage.isUndo())
            {
                return null;
            }

            String libId = null;

            for (int i = m_storage.GetHistoryPosition(); i >= 0; i--)
            {
                HistoryElement he = m_storage.GetHistoryElement(i);
                if (he.cmdId == Commander.CMD_OBJECT_CREATE)
                {
                    if (he.objNames[0] == objectName)
                    {
                        libId = he.cmdParams[0];
                        break;
                    }
                }
            }

            return libId;
        }

        public String FindObjectParent(String objectName, out byte libFlag)
        {
            libFlag = 0;
            if (!m_storage.isUndo())
            {
                return objectName;
            }

            String parent = objectName;

            for (int i = m_storage.GetHistoryPosition(); i >= 0; i--)
            {
                HistoryElement he = m_storage.GetHistoryElement(i);
                if (he.cmdId == Commander.CMD_RENAME) // переименовывание
                {
                    if (he.cmdParams[0] == parent)
                    {
                        parent = he.objNames[0];
                    }
                }
                if (he.cmdId == Commander.CMD_OBJECT_CREATE) // создание
                {
                    if (he.objNames[0] == parent)
                    {
                        parent = he.cmdParams[0];
                        libFlag = 1;
                        break;
                    }
                }
                if(he.cmdId == Commander.CMD_OBJECT_COPY) // копирование
                {
                    ArrayList newObjects = (ArrayList)he.cmdParam;
                    for (int newObjIndex = 0; newObjIndex < newObjects.Count; newObjIndex += 2)
                    {
                        if((String)newObjects[newObjIndex] == parent)
                        {
                            parent = he.objNames[newObjIndex/2];
                            break;
                        }
                    }                   
                }
            }

            return parent;
        }

        public Object FindChangedProp(String objectName, byte cmd)
        {
            if (!m_storage.isUndo())
            {
                return null;
            }

            Object result = null;

            for (int i = m_storage.GetHistoryPosition(); i >= 0; i--)
            {
                HistoryElement he = m_storage.GetHistoryElement(i);
                if (he.cmdId == Commander.CMD_CLASS && cmd == Commander.CMD_CLASS)
                {
                    ArrayList objNames = new ArrayList(he.objNames);
                    if (objNames.Contains(objectName))
                    {
                        result = he.cmdParams[1];
                        break;
                    }
                }
                if (he.cmdId == Commander.CMD_PARAMS && cmd == Commander.CMD_PARAMS)
                {
                    ArrayList objNames = new ArrayList(he.objNames);
                    if (objNames.Contains(objectName))
                    {
                        result = he.cmdParams[1];
                        break;
                    }
                }
                if(he.cmdId == Commander.CMD_LOCK && cmd == Commander.CMD_LOCK)
                {
                    ArrayList objNames = new ArrayList(he.objNames);
                    if (objNames.Contains(objectName))
                    {
                        result = ((Int32[])he.cmdParam)[1];
                        break;
                    }
                }
                if (he.cmdId == Commander.CMD_CHANGE_COLLISION && cmd == Commander.CMD_CHANGE_COLLISION)
                {
                    ArrayList objNames = new ArrayList(he.objNames);
                    if (objNames.Contains(objectName))
                    {
                        result = he.cmdParam;
                        break;
                    }
                }
                if (he.cmdId == Commander.CMD_SET_GROUP && cmd == Commander.CMD_SET_GROUP)
                {
                    ArrayList objNames = new ArrayList(he.objNames);
                    if (objNames.Contains(objectName))
                    {
                        result = he.objNames[1];
                        break;
                    }
                }

            }

            return result;
        }

        public bool ProcessHistory(ClientMySQL _mySql, bool clearHistoryOnCancel)
        {
            if (_mySql == null)
            {
                MessageBox.Show("Соединение с базой данных не может быть установлено!", "Ошибка",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            ClientMySQL.WriteToLog("Try to save history");

            // если история пуста или одни селекты, то выходим
            if(!m_storage.isUndo())
            {
                ClientMySQL.WriteToLog("history is empty");
                return true;
            }
            else
            {
                bool allSelectAction = true;
                for(int i = 0; i <= m_storage.GetHistoryPosition(); i++)
                {
                    if(m_storage.GetHistoryElement(i).cmdId != Commander.CMD_SELECT)
                    {
                        allSelectAction = false;
                        break;
                    }
                }

                if(allSelectAction)
                {
                    ClientMySQL.WriteToLog("there are only selects in history");
                    return true;
                }
            }

            ClientMySQL.WriteToLog("there are some important elements in history");

            ClientMySQL mySql = _mySql;
            HistoryElement action = null;

            // подтверждение            
            DialogResult confirmResult = MessageBox.Show("Сохранить изменения на сервер?", "Сохранение", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (confirmResult == DialogResult.Yes)
            {
                ClientMySQL.WriteToLog("Save is confirmed");
                // информация changelog
                String userName = Environment.UserName;
                String changeQuery = "INSERT INTO `objects_map_changelog` SET `user_name`='" + userName + "'";
                mySql.NonQuery(changeQuery);
                //changeQuery = "SELECT LAST_INSERT_ID() as `last_id` FROM `objects_map_changelog`";
                changeQuery = "SELECT LAST_INSERT_ID() as `last_id`";

                Hashtable lastIdRes = (Hashtable)mySql.QueryA(changeQuery)[0];
                if (lastIdRes == null)
                {
                    return false;
                }

                if (lastIdRes["last_id"] == null)
                {
                    return false;
                }

                Int64 changeId = (Int64)lastIdRes["last_id"]; // id изменений                             

                ArrayList changedObjectsId = new ArrayList();

                ClientMySQL.WriteToLog("Count of history elements: " + m_storage.GetHistoryPosition().ToString());

                for (int actionIndex = 0; actionIndex <= m_storage.GetHistoryPosition(); actionIndex++)
                {
                    action = m_storage.GetHistoryElement(actionIndex);
                    ClientMySQL.WriteToLog("Current element has type " + action.cmdId.ToString());

                    switch(action.cmdId)
                    {
                        case Commander.CMD_DELETE:
                            {
                                ClientMySQL.WriteToLog("History CMD_DELETE start");
                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));

                                for(int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex ++)
                                if (objectIndex < changedObjectsId.Count)
                                {
                                    // Удаление из таблицы связей групп с объектами
                                    String queryStr = "DELETE FROM `object_groups` WHERE `id_object`='" + changedObjectsId[objectIndex] + "'";
                                    int result = mySql.NonQuery(queryStr);
                                    queryStr = "DELETE FROM `objects_map` WHERE `name`='" + action.objNames[objectIndex] + "'";
                                    result = mySql.NonQuery(queryStr);                                
                                    // Удаление из таблицы блокировок
                                    queryStr = "DELETE FROM `objects_map_lock` WHERE `object_name`='" + action.objNames[objectIndex] + "'";
                                    result = mySql.NonQuery(queryStr);
                                  }
                                ClientMySQL.WriteToLog("History CMD_DELETE end");
                                break;
                            }
                        case Commander.CMD_RENAME:
                            {
                                ClientMySQL.WriteToLog("History CMD_RENAME start");
                                changedObjectsId.AddRange(GetObjectsId(mySql, new String[] { action.cmdParams[0] }, changedObjectsId));

                                String queryStr = "UPDATE `objects_map` SET `name`='" + action.cmdParams[0] + "' WHERE `name`='" + action.objNames[0] + "'";
                                int result = mySql.NonQuery(queryStr);
                                ClientMySQL.WriteToLog("History CMD_RENAME end");
                                
                                break;
                            }
                        case Commander.CMD_MOVE:
                            {
                                ClientMySQL.WriteToLog("History CMD_MOVE start");
                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));

                                for(int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex++)
                                {
                                    // получить из базы текущие координаты
                                    String queryStr = "SELECT `px`,`py`,`pz`,`sqId` FROM `objects_map` WHERE `name`='" + action.objNames[objectIndex] + "'";
                                    ArrayList aResult = mySql.QueryA(queryStr);

                                    if ((aResult == null) || (aResult.Count == 0))
                                    {
                                        ClientMySQL.WriteToLog("[ERROR] attempt to move non-existing object");
                                        ClientMySQL.WriteToLog("History CMD_MOVE end");
                                        continue;
                                    }

                                    Hashtable transformation = (Hashtable)aResult[0];
                                    Vector deltaMove = (Vector)action.cmdParam;
                                    // 1.вычисление мировых координат
                                    int zoneX, zoneY;
                                    GetZoneCenter((Int64)transformation["sqId"], out zoneX, out zoneY);
                                    // 2.применение изменений
                                    float newX = (Single)transformation["px"] + deltaMove.X + zoneX;
                                    float newY = (Single)transformation["py"] + deltaMove.Y + zoneY;
                                    float newZ = (Single)transformation["pz"] + deltaMove.Z;
                                    // 3.расчет новой зоны
                                    ulong newSqId = GetZoneID(newX, newY);
                                    // 4.расчет новых координат в зоне
                                    newX = WorldToZoneCoords(newX);
                                    newY = WorldToZoneCoords(newY);

                                    // 5.установка новых координат
/*                                    queryStr = "UPDATE `objects_map` SET `px`='" + ((Single)transformation["px"] + deltaMove.X).ToString("F6") + 
                                                "', `py`='" + ((Single)transformation["py"] + deltaMove.Y).ToString("F6") + 
                                                "', `pz`='" + ((Single)transformation["pz"] + deltaMove.Z).ToString("F6") +
                                                "' WHERE `name`='" + action.objNames[objectIndex] + "'";*/
                                    queryStr = "UPDATE `objects_map` SET `px`='" + newX.ToString("F6") +
                                                "', `py`='" + newY.ToString("F6") +
                                                "', `pz`='" + newZ.ToString("F6") +
                                                "', `sqId`='" + newSqId.ToString() +
                                                "' WHERE `name`='" + action.objNames[objectIndex] + "'";

                                    int iResult = mySql.NonQuery(queryStr);
                                }
                                ClientMySQL.WriteToLog("History CMD_MOVE end");
                                break;
                            }
                        case Commander.CMD_ROTATE:
                            {
                                ClientMySQL.WriteToLog("History CMD_ROTATE start");
                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));

                                for (int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex++)
                                {
                                    String objectName = action.objNames[objectIndex];
                                    Quaternion objectRotate = (Quaternion)((Hashtable)((ArrayList)action.cmdParam)[1])[objectName];
                                 
                                    // установка нового поворота
                                    String queryStr = "UPDATE `objects_map` SET `rx`='" + objectRotate.X.ToString("F6") + "', `ry`='" + objectRotate.Y.ToString("F6") + "', `rz`='" + objectRotate.Z.ToString("F6") + "', `rw`='" + objectRotate.W.ToString("F6") + "' WHERE `name`='" + objectName + "'";
                                    //Console.WriteLine(queryStr);
                                    int iResult = mySql.NonQuery(queryStr);
                                }
                                ClientMySQL.WriteToLog("History CMD_ROTATE end");
                                break;
                            }
                        case Commander.CMD_SCALE:
                            {
                                ClientMySQL.WriteToLog("History CMD_SCALE start");
                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));

                                for (int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex++)
                                {
                                    // вытянуть из базы текущий масштаб и установить новый
                                    String queryStr = "SELECT `scalex`,`scaley`,`scalez` FROM `objects_map` WHERE `name`='" + action.objNames[objectIndex] + "'";
                                    ArrayList aResult = mySql.QueryA(queryStr);

                                    if ((aResult == null) || (aResult.Count == 0))
                                    {
                                        ClientMySQL.WriteToLog("[ERROR] attempt to scale non-existing object");
                                        ClientMySQL.WriteToLog("History CMD_SCALE end");
                                        continue;
                                    }

                                    Hashtable scale = (Hashtable)aResult[0];
                                    Vector vScale = new Vector((Single)scale["scalex"], (Single)scale["scaley"], (Single)scale["scalez"]);
                                    Vector deltaScale = (Vector)action.cmdParam;

                                    vScale.X += deltaScale.X;
                                    vScale.Y += deltaScale.Y;
                                    vScale.Z += deltaScale.Z;

                                    queryStr = "UPDATE `objects_map` SET `scalex`='" + vScale.X.ToString("F6") + "', `scaley`='" + vScale.Y.ToString("F6") + "', `scalez`='" + vScale.Z.ToString("F6") + "' WHERE `name`='" + action.objNames[objectIndex] + "'";
                                    //Console.WriteLine(queryStr);
                                    int iResult = mySql.NonQuery(queryStr);
                                }
                                ClientMySQL.WriteToLog("History CMD_SCALE end");
                                break;
                            }
                        case Commander.CMD_CHANGE_COLLISION:
                            {
                                ClientMySQL.WriteToLog("History CMD_CHANGE_COLLISION start");
                                int collisionFlag = (int)action.cmdParam;
                                for (int objIndex = 0; objIndex < action.objNames.Length; objIndex++)
                                {
                                    string queryStr = "UPDATE `objects_map` SET `level`='" + collisionFlag + "' WHERE `name`='" + action.objNames[objIndex] + "'";
                                    int iResult = mySql.NonQuery(queryStr);
                                }
                                ClientMySQL.WriteToLog("History CMD_CHANGE_COLLISION end");
                            }
                            break;

                        case Commander.CMD_SET_GROUP:
                            {
                                ClientMySQL.WriteToLog("History CMD_SET_GROUP start");
                                String objectName = action.objNames[0];
                                String groupName = action.objNames[1];

                                String queryStr = "SELECT `id` FROM `objects_map` WHERE `name`='" + objectName + "'";
                                ArrayList aResult = mySql.QueryA(queryStr);

                                if ((aResult == null) || (aResult.Count == 0))
                                {
                                    ClientMySQL.WriteToLog("[ERROR] object with name " + objectName + " not found!");
                                    ClientMySQL.WriteToLog("History CMD_SET_GROUP end");
                                    continue;
                                }
                                
                                int objectId = int.Parse((((Hashtable)aResult[0])["id"]).ToString());

                                queryStr = "SELECT `id` FROM `objects_map` WHERE `name`='" + groupName + "'";
                                aResult = mySql.QueryA(queryStr);

                                if ((aResult == null) || (aResult.Count == 0))
                                {
                                    ClientMySQL.WriteToLog("[ERROR] group with name " + groupName + " not found!");
                                    ClientMySQL.WriteToLog("History CMD_SET_GROUP end");
                                    continue;
                                }

                                int groupId = int.Parse((((Hashtable)aResult[0])["id"]).ToString());

                                queryStr = "SELECT * FROM `object_groups` WHERE `id_object`='" + objectId + "'";
                                aResult = mySql.QueryA(queryStr);

                                //если объект не имеет группы
                                if (aResult == null)
                                {
                                     queryStr = "INSERT INTO `object_groups` (`id_group`,`id_object`) VALUES ('" + groupId + "','" + objectId + "')";
                                     mySql.NonQuery(queryStr);
                                }
                                else
                                {
                                     queryStr = "UPDATE `object_groups` SET `id_group`='" + groupId + "' WHERE `id_object`='" + objectId + "' ";
                                     mySql.NonQuery(queryStr);
                                }

                                ClientMySQL.WriteToLog("History CMD_SET_GROUP end");
                                break;
                            }                            
                         case Commander.CMD_SHADOW:
                            {                                
                                ClientMySQL.WriteToLog("History CMD_SHADOW start");

                                String objectName = action.objNames[0];
                                Object3D.LodShadow lodLevelInfo = (Object3D.LodShadow)action.cmdParam;

                                String queryStr = "SELECT `res` FROM `objects_map` WHERE `name`='" + objectName + "'";
                                //String queryStr = "SELECT `res` FROM `objects_map` WHERE `res` IN (SELECT `res` FROM `objects_map` WHERE `name`='"+ objectName +"')";
                                ArrayList aResult = mySql.QueryA(queryStr);

                                if ((aResult == null) || (aResult.Count == 0))
                                {
                                    ClientMySQL.WriteToLog("[ERROR] object with name " + objectName + " not found!");
                                    ClientMySQL.WriteToLog("History CMD_SHADOW end");
                                    continue;
                                }

                                Hashtable row = (Hashtable)aResult[0];
                                String oldRes = (String)row["res"];
                                oldRes = oldRes.Replace("\\", "\\\\");

                                int shadowStrPos = -1;
                                for(int lodIndex = 0; lodIndex <= (int)lodLevelInfo.distance; lodIndex++)
                                {
                                    shadowStrPos = oldRes.IndexOf(";", shadowStrPos + 1);
                                    shadowStrPos = oldRes.IndexOf(";", shadowStrPos + 1);

                                    if(lodIndex != (int)lodLevelInfo.distance)
                                    {
                                        shadowStrPos = oldRes.IndexOf(";", shadowStrPos + 1);
                                    }
                                }
                                String newRes = oldRes.Substring(0, shadowStrPos + 1) + 
                                                lodLevelInfo.shadow.ToString() + 
                                                oldRes.Substring(shadowStrPos + 1 + lodLevelInfo.shadow.ToString().Length);

                                queryStr = "SELECT `name` FROM `objects_map` WHERE `res`='"+ oldRes +"'";
                                aResult = mySql.QueryA(queryStr);

                                ArrayList names = new ArrayList();
                                foreach(Hashtable nameRow in aResult)
                                {
                                    names.Add(nameRow["name"]);
                                }
                                
                                queryStr = "UPDATE `objects_map` SET `res`='" + newRes + "' WHERE `res`='" + oldRes + "'";                                
                                Console.WriteLine(queryStr);
                                //queryStr = "SELECT `res` FROM `objects_map` WHERE `res`='" + oldRes + "'";
                                //aResult = mySql.QueryA(queryStr);
                                int iResult = mySql.NonQuery(queryStr);

                                String[] namesArr = new String[names.Count];
                                Array.Copy(names.ToArray(), namesArr, names.Count);

                                changedObjectsId.AddRange(GetObjectsId(mySql, namesArr, changedObjectsId));
                                ClientMySQL.WriteToLog("History CMD_SHADOW end");

                                break;
                            }
                        case Commander.CMD_OBJECT_COPY:
                            {                              
                                ClientMySQL.WriteToLog("History CMD_OBJECT_COPY start");
                                for (int objIndex = 0; objIndex < action.objNames.Length; objIndex++ )
                                {
                                    String objectName = action.objNames[objIndex];
                                    String queryStr = "SELECT * FROM `objects_map` WHERE `name`='" + objectName + "'";

                                    ArrayList aResult = mySql.QueryA(queryStr);
                                    if ((aResult == null) || (aResult.Count == 0))
                                    {
                                        ClientMySQL.WriteToLog("[ERROR] object with name " + objectName + " not found!");                                        
                                        continue;
                                    }

                                    Hashtable objectInfo = (Hashtable)aResult[0];

                                    //queryStr = "SELECT max(id) as `id` FROM `objects_map` WHERE id < 100000000";
                                    queryStr = "SELECT max(t.id_max) as `id` FROM (" +
                                               "SELECT max(id) as `id_max` FROM `objects_map` WHERE `id`<'100000000' union all " +
                                               "SELECT max(id_object) as `id_max` FROM `objects_map_changelog_items` WHERE `id_object`<'100000000') as `t`";

                                    aResult = mySql.QueryA(queryStr);
                                    if (aResult == null)
                                    {
                                        ClientMySQL.WriteToLog("[ERROR] max id not found!");
                                        continue;
                                    }

                                    Hashtable maxId = (Hashtable)aResult[0];
                                    UInt32 objectCopyId = (UInt32)maxId["id"] + 1;

                                    String objectCopyName = (String)(((ArrayList)action.cmdParam)[objIndex * 2]);
                                    Vector objectCopyCoords = (Vector)(((ArrayList)action.cmdParam)[objIndex * 2 + 1]);

                                    objectCopyCoords.Z += (Single)objectInfo["pz"];

                                    String _params = (String)objectInfo["params"];
                                    _params = _params.Replace("'", "\"");
                                    String objectRes = ((String)objectInfo["res"]).Replace("\\", "\\\\");
                                    queryStr = "INSERT INTO `objects_map` SET `id`='"+objectCopyId.ToString() + "', " +
                                                "`type`='" +((Int32)objectInfo["type"]).ToString()+ "', " +
                                                "`res`='" + objectRes + "', " +
                                                "`name`='" + objectCopyName + "', " +
                                                "`level`='" + ((Int32)objectInfo["level"]).ToString() + "', " +
                                                "`class`='" + (String)objectInfo["class"] + "', " +
                                                "`px`='" + ((Single)objectInfo["px"]).ToString() + "', " +
                                                "`py`='" + ((Single)objectInfo["py"]).ToString() + "', " +
                                                "`pz`='" + (objectCopyCoords.Z).ToString() + "', " +
                                                "`rx`='" + ((Single)objectInfo["rx"]).ToString() + "', " +
                                                "`ry`='" + ((Single)objectInfo["ry"]).ToString() + "', " +
                                                "`rz`='" + ((Single)objectInfo["rz"]).ToString() + "', " +
                                                "`rw`='" + ((Single)objectInfo["rw"]).ToString() + "', " +
                                                "`bx`='" + ((Single)objectInfo["bx"]).ToString() + "', " +
                                                "`by`='" + ((Single)objectInfo["by"]).ToString() + "', " +
                                                "`bz`='" + ((Single)objectInfo["bz"]).ToString() + "', " +
                                                "`sqId`='" + GetZoneID(objectCopyCoords.X, objectCopyCoords.Y).ToString() + "', " +
                                                "`params`='" + _params + "', " +
                                                "`scalex`='" + ((Single)objectInfo["scalex"]).ToString() + "', " +
                                                "`scaley`='" + ((Single)objectInfo["scaley"]).ToString() + "', " +
                                                "`scalez`='" + ((Single)objectInfo["scalez"]).ToString() + "'";

                                    int iResult = mySql.NonQuery(queryStr);

                                    changedObjectsId.AddRange(GetObjectsId(mySql, new String[] { objectCopyName }, changedObjectsId));

                                }
                                ClientMySQL.WriteToLog("History CMD_OBJECT_COPY end");
                                break;
                            }
                        case Commander.CMD_OBJECT_CREATE:
                            {
                                ClientMySQL.WriteToLog("History CMD_OBJECT_CREATE start");

                                String objectName = action.objNames[0];
                                String objectLibId = action.cmdParams[0];
                                Vector objectCoords = (Vector)action.cmdParam;

                                String queryStr = "SELECT * FROM `objects_lib` WHERE `id`='" + objectLibId + "'";
                                ArrayList aResult = mySql.QueryA(queryStr);

                                if ((aResult != null) && (aResult.Count != 0))
                                {
                                    Hashtable objectLibInfo = (Hashtable)aResult[0];

                                    //queryStr = "SELECT max(id) as `id` FROM `objects_map` WHERE id < 100000000";
                                    queryStr = "SELECT max(t.id_max) as `id` FROM (" +
                                               "SELECT max(id) as `id_max` FROM `objects_map` WHERE `id`<'100000000' union all " +
                                               "SELECT max(id_object) as `id_max` FROM `objects_map_changelog_items` WHERE `id_object`<'100000000') as `t`";

                                    aResult = mySql.QueryA(queryStr);
                                    if ((aResult == null) || (aResult.Count == 0))
                                    {
                                        ClientMySQL.WriteToLog("[ERROR] max id not found");
                                        continue;
                                    }

                                    Hashtable maxId = (Hashtable)aResult[0];
                                    UInt32 objectCreateId = (UInt32)maxId["id"] + 1;

                                    String objectRes = ((String)objectLibInfo["res"]).Replace("\\", "\\\\");
                                    queryStr = "INSERT INTO `objects_map` SET `id`='" + objectCreateId.ToString() + "', " +
                                                "`type`='1', " +
                                                "`res`='" + objectRes + "', " +
                                                "`name`='" + objectName + "', " +
                                                //"`reference`='', " +
                                                "`level`='" + ((Int32)objectLibInfo["level"]).ToString() + "', " +
                                                //"`lod`='0', " +
                                                "`class`='" + (String)objectLibInfo["class"] + "', " +
                                                //"`version`='1', " +
                                                "`px`='" + (WorldToZoneCoords(objectCoords.X)).ToString("F1") + "', " +
                                                "`py`='" + (WorldToZoneCoords(objectCoords.Y)).ToString("F1") + "', " +
                                                "`pz`='" + objectCoords.Z.ToString("F1") + "', " +
                                                "`rx`='0', " +
                                                "`ry`='0', " +
                                                "`rz`='1', " +
                                                "`rw`='0', " +
                                                "`bx`='" + ((Single)objectLibInfo["bx"]).ToString() + "', " +
                                                "`by`='" + ((Single)objectLibInfo["by"]).ToString() + "', " +
                                                "`bz`='" + ((Single)objectLibInfo["bz"]).ToString() + "', " +
                                                "`sqId`='" + GetZoneID(objectCoords.X, objectCoords.Y).ToString() + "', " +
                                                "`params`='" + (String)objectLibInfo["params"] + "', " +
                                                //"`grass`='0', " +
                                                "`scalex`='1', " +
                                                "`scaley`='1', " +
                                                "`scalez`='1'";

                                    int iResult = mySql.NonQuery(queryStr);

                                    changedObjectsId.AddRange(GetObjectsId(mySql, new String[] { objectName }, changedObjectsId));
                                }
                                else
                                {
                                    ClientMySQL.WriteToLog("Object in library with id = " + objectLibId.ToString() + " not found");
                                }
                                ClientMySQL.WriteToLog("History CMD_OBJECT_CREATE end");
                                break;
                            }
                        case Commander.CMD_CLASS:
                            {
                                ClientMySQL.WriteToLog("History CMD_CLASS start");

                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));

                                for (int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex++)
                                {
                                    String objectName = action.objNames[objectIndex];
                                   
                                    // установка нового класса
                                    String queryStr = "UPDATE `objects_map` SET `class`='"+ action.cmdParams[1] +"' WHERE `name`='" + objectName + "'";
                                    int iResult = mySql.NonQuery(queryStr);
                                }

                                ClientMySQL.WriteToLog("History CMD_CLASS end");
                                break;
                            }
                        case Commander.CMD_PARAMS:
                            {
                                ClientMySQL.WriteToLog("History CMD_PARAMS start");

                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));
                                for (int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex++)
                                {
                                    String objectName = action.objNames[objectIndex];

                                    // установка нового класса
                                    String queryStr = "UPDATE `objects_map` SET `params`='" + action.cmdParams[1] + "' WHERE `name`='" + objectName + "'";
                                    int iResult = mySql.NonQuery(queryStr);
                                }

                                ClientMySQL.WriteToLog("History CMD_PARAMS end");
                                break;                                
                            }
                        case Commander.CMD_LOCK:
                            {
                                ClientMySQL.WriteToLog("History CMD_LOCK start");

                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));

                                for (int objectIndex = 0; objectIndex < action.objNames.Length; objectIndex++)
                                {
                                    String objectName = action.objNames[objectIndex];

                                    String queryStr = "SELECT * FROM `objects_map_lock` WHERE `object_name`='" + objectName + "'";

                                    if (objectName != "")
                                    {
                                        ArrayList result = mySql.QueryA(queryStr);
                                        if ((result == null) || (result.Count == 0))
                                        {
                                            queryStr = "INSERT INTO `objects_map_lock` SET `object_name`='" + objectName + "', `lock_level`='" + ((Int32[])action.cmdParam)[1].ToString() + "'";
                                            mySql.NonQuery(queryStr);
                                        }
                                        else
                                        {
                                            // установка блокировки
                                            queryStr = "UPDATE `objects_map_lock` SET `lock_level`='" + ((Int32[])action.cmdParam)[1].ToString() + "' WHERE `object_name`='" + objectName + "'";
                                            int iResult = mySql.NonQuery(queryStr);
                                        }
                                    }
                                }
                                ClientMySQL.WriteToLog("History CMD_LOCK end");
                                break;
                            }
                        case Commander.CMD_MATERIAL:
                            {
                                ClientMySQL.WriteToLog("History CMD_MATERIAL start");
                                changedObjectsId.AddRange(GetObjectsId(mySql, action.objNames, changedObjectsId));
                                ClientMySQL.WriteToLog("History CMD_MATERIAL end");
                                break;
                            }
                        default:
                            {
                                break;
                            }
                    }
                }

                foreach(UInt32 objectId in changedObjectsId)
                {
                    changeQuery = "INSERT INTO `objects_map_changelog_items` SET `id_changelog`='" + changeId.ToString() + "', `id_object`='" + objectId.ToString() + "'";
                    mySql.NonQuery(changeQuery);
                }

                m_storage.Clear();

                mainForm.ReloadServers();

                ClientMySQL.WriteToLog("History applying end");
                            
                return true;
            }
            else if(clearHistoryOnCancel)
            {
                ClientMySQL.WriteToLog("Save is canceled");
                m_storage.Clear();
            }

            return false;
        }

        private ArrayList GetObjectsId(ClientMySQL _mySql, String[] objectsName, ArrayList changedObjectsId)
        {
            ArrayList objectsId = new ArrayList();

            if (_mySql == null)
            {
                return objectsId;
            }

            if (objectsName.Length == 0)
            {
                return objectsId;
            }

            String queryStr = "SELECT `id` FROM `objects_map` WHERE `name` IN (";

            for (int objectIndex = 0; objectIndex < objectsName.Length; objectIndex++)
            {
                if(objectIndex != 0)
                {
                    queryStr += ", ";
                }

                queryStr += "'" + objectsName[objectIndex] + "'";                
            }

            queryStr += ")";
            ArrayList aResult = _mySql.QueryA(queryStr);

            if (aResult != null)
            foreach (Hashtable objId in aResult)
            {
                if (objId["id"] != null)
                {
                    UInt32 objectId = (UInt32)objId["id"];
                    if (!changedObjectsId.Contains(objectId))
                    {
                        objectsId.Add(objectId);
                    }
                }
            }

            return objectsId;
        }

        public ulong GetZoneID(float ax, float ay)
        {
            int ny = (int)(ay / 20000.0f) + 16000;
            int nx = (int)(ax / 20000.0f) + 16000;
            if (ay < 0) ny -= 1;
            if (ax < 0) nx -= 1;
            ulong sqID = ((ulong)ny << 15) + (ulong)nx;
            return sqID;
        }

        public float WorldToZoneCoords(float worldCoord)
        {
            int ZONE_SIZE = 20000;

            int zoneID = (int)(worldCoord / ZONE_SIZE);
            if (worldCoord < 0) zoneID -= 1;
            return worldCoord - (zoneID * ZONE_SIZE + ZONE_SIZE / 2);
        }

        public void GetZoneCenter(Int64 zoneId, out int x, out int y)
        {
            int ZONE_SIZE = 20000;

            int ax = (int)(zoneId & 0x00007FFF) - 16000;
            int ay = (int)((zoneId & 0x3FFF8000) >> 15) - 16000;

            x = ax * ZONE_SIZE + ZONE_SIZE / 2;
            y = ay * ZONE_SIZE + ZONE_SIZE / 2;                        
        }
    }
}
