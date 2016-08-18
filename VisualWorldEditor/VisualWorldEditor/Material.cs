using System;
using System.Collections;
using System.Text;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using System.Threading; 

namespace VisualWorldEditor
{
    class Material
    {
        private String m_objectName;
        private String m_modelName;
        private ArrayList m_texturesList;
        private ArrayList m_materialsList;

        private ClientMySQL m_mySql;

        private Translate m_translates;

        //шаблоны
        private String m_templateDir = "mtemplates";
        private ArrayList m_templatesList;
        //дополнительные параметры
        private MaterialFile m_templateItems;

        private ArrayList m_resPaths; 

        static Thread saveMaterialThread = null;
        private Boolean isSaveMaterial = false;
        private static Boolean isSaveMaterialThreadWorking = true;

        private const uint SWP_SHOWWINDOW = 0x0040;
        private const uint SWP_NOMOVE = 0x0002;
        private const uint SWP_NOSIZE = 0x0001;

        private IntPtr HWND_TOPMOST = ((IntPtr)(-1));
        
        public Material()
        {
            m_objectName = null;

            m_resPaths = null;

            m_texturesList = null;
            m_templatesList = null;
            m_templateItems = null;

            m_mySql = new ClientMySQL();
            m_mySql.Connect();

            m_translates = new Translate(m_templateDir + "/");

            if (saveMaterialThread == null)
            {
                saveMaterialThread = new Thread(new ThreadStart(ShowSaveMaterialWnd));
                saveMaterialThread.Start(); 
            }
        }

        public static void DeleteThreads()
        {
            isSaveMaterialThreadWorking = false;
            Thread.Sleep(100);
            try
            {
                saveMaterialThread.Abort();
            }
            catch (Exception /*e*/)
            {
            }
        }

        private void ShowSaveMaterialWnd()
        {
            while (isSaveMaterialThreadWorking)
            {
                if (isSaveMaterial)
                {
                    IntPtr iHandle = ClientLoader.FindWindow(null, "ѕодтверждение сохранени€ материала");
                    if (iHandle != null)
                    {
                        ClientLoader.SetWindowPos(iHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                    }
                    Thread.Sleep(20);
                }
                else
                {
                    Thread.Sleep(100);
                }
            }
        }

        public void UpdateConnection()
        {
            m_mySql.Connect();
        }

        private String GetGeometryPath()
        {
            return ServerPath.GetServerPath(Options.GetHost()) + ServerPath.geometryDir;
        }

        public ArrayList GetResoursesPaths()
        {
            if(m_resPaths == null)
            {
                // пройтись по истории и найти изначальное им€ %)
                HistoryProcessor hproc = HistoryProcessor.GetHProcessor();

                byte libFlag;
                String objectParent = hproc.FindObjectParent(m_objectName, out libFlag);

                String queryStr = "";
                if (libFlag == 0)
                {
                    queryStr = "SELECT `res` FROM `objects_map` WHERE `name`='" + objectParent + "'";
                }
                else
                {
                    queryStr = "SELECT `res` FROM `objects_lib` WHERE `id`='" + objectParent + "'";
                }

                ArrayList queryResult = m_mySql.QueryA(queryStr);
                if (queryResult == null)
                {
                    m_resPaths = new ArrayList();
                    m_resPaths.Add(GetGeometryPath() + "common");
                }
                else
                {
                    if (((Hashtable)(queryResult[0]))["res"] != null)
                    {
                        String objectRes = (String)((Hashtable)(queryResult[0]))["res"];

                        // пути к ресурсам
                        m_resPaths = new ArrayList();
                        String[] splittedRes = objectRes.Split(';');

                        for (int splitItemIndex = 0; splitItemIndex < splittedRes.Length; splitItemIndex++)
                        {
                            if (splitItemIndex % 3 == 0)
                            {
                                if (splittedRes[splitItemIndex] == "")
                                {
                                    break;
                                }

                                m_resPaths.Add(GetGeometryPath() + splittedRes[splitItemIndex].Substring(0, splittedRes[splitItemIndex].IndexOf("\\mesh.bmf.zip")));
                            }
                        }
                    }
                }

            }

            return m_resPaths;
        }

         private String GetModelName(int lod)
        {
            //if(m_modelName == null)
            //{
                String resPath = (String)m_resPaths[lod];
                int slashIndex = resPath.LastIndexOf("\\");
                if (slashIndex == -1)
                {
                    slashIndex = resPath.LastIndexOf("/");
                }
                if (slashIndex >= 0)
                {
                    m_modelName = resPath.Substring(slashIndex + 1);                
                }
            //}

            return m_modelName;
        }

        public ArrayList GetTextures()
        {
            if(m_texturesList == null)
            {
                m_texturesList = new ArrayList();

                try
                {
                    foreach(String resPath in GetResoursesPaths())
                    {
                        ArrayList lodTextures = new ArrayList();

                        using (XmlReader materialReader = XmlReader.Create(resPath + "/materials.xml"))
                        {
                            materialReader.MoveToContent();
                            while (materialReader.Read())
                            {
                                if ((materialReader.Name == "texture") || (materialReader.Name == "cubemap"))
                                {
                                    String textureName = materialReader.ReadString();
                                    if (!m_texturesList.Contains(textureName))
                                    {
                                        lodTextures.Add(textureName);
                                    }
                                }
                            }
                        }

                        m_texturesList.Add(lodTextures);
                    }
                }
                catch(Exception e)
                {
                    // отсутствует файл со списком текстур
                    Console.WriteLine(e.Message);
                }
            }

            return m_texturesList;
        }

        public ArrayList GetObjectMaterials()
        {
            if(m_materialsList == null)
            {
                m_materialsList = new ArrayList();

                foreach (String resPath in m_resPaths)
                {
                    ArrayList lodMaterials = new ArrayList();
                    try
                    {
                        
                        using (StreamReader materialsReader = new StreamReader(resPath + "/mesh.ini"))
                        {
                            String line;
                            while ((line = materialsReader.ReadLine()) != null)
                            {
                                lodMaterials.Add(line);
                            }
                        }                        
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("The file 'mesh.ini' could not be read:");
                        Console.WriteLine(e.Message);
                    }
                    m_materialsList.Add(lodMaterials);
                }
            }

            return m_materialsList;
        }

        public bool IsUsingMaterial(String textureName, int lod)
        {
            if (GetObjectMaterials().Count == 0)
            {
                return false;
            }

            ArrayList usingMaterials = (ArrayList)(GetObjectMaterials())[lod];
            String materialName = GetMaterialNameFromTexture(textureName, lod);
            for (int materialIndex = 0; materialIndex < usingMaterials.Count; materialIndex++)
            {
                if (materialName.CompareTo((String)usingMaterials[materialIndex]) == 0)
                {
                    return true;
                }
            }

            return false;
        }

        public String GetMaterialNameFromTexture(String textureName, int lod)
        {
            string res = "";

            try 
            {
                int dotIndex = textureName.LastIndexOf(".");
                if (dotIndex <= 0)
                {
                    res = "";
                }
                else
                {
                    res = textureName.Substring(0, dotIndex) + "_" +
                        textureName.Substring(dotIndex + 1) + ".xml_" + GetModelName(lod);
                }   
            }
            catch (Exception /*e*/)
            {
            }

            return res;
        }

        public String[] GetMaterialNameFromTexture(String[] texturesNames, int lod)
        {
            String[] formattedNames = new String[texturesNames.Length];

            for(int nameIndex = 0; nameIndex < texturesNames.Length; nameIndex++)
            {
                formattedNames[nameIndex] = GetMaterialNameFromTexture(texturesNames[nameIndex], lod);
            }

            return formattedNames;
        }

        public void SaveUsingMaterials(ArrayList materialsList)
        {
            for(int lodIndex = 0; lodIndex < m_materialsList.Count; lodIndex++)
            {
                ArrayList lodMaterials;
                if(lodIndex >= materialsList.Count)
                {
                    lodMaterials = new ArrayList();
                }
                else
                {
                    lodMaterials = (ArrayList)materialsList[lodIndex]; // отмеченные пользователем материалы (имена текстур)
                }                
                ArrayList material = (ArrayList)m_materialsList[lodIndex];// список материалов, загруженных из mesh.ini
                // проверка на изменение списка
                if(lodMaterials.Count == material.Count)
                {
                    bool equals = true;
                    for (int materialIndex = 0; materialIndex < lodMaterials.Count; materialIndex++)
                    {
                        if (material.IndexOf(GetMaterialNameFromTexture((String)lodMaterials[materialIndex], lodIndex)) == -1)
                        {
                            equals = false;
                            break;
                        }
                    }

                    if (equals)
                    {
                        continue;
                    }
                }

                String[] formattedNames = GetMaterialNameFromTexture((String[])lodMaterials.ToArray(typeof(String)), lodIndex);
                m_materialsList[lodIndex] = new ArrayList(formattedNames);

                // сохранить, если произошли изменени€
                try
                {
                    File.WriteAllLines((String)GetResoursesPaths()[lodIndex] + "/mesh.ini", formattedNames);
                    //HistoryProcessor.GetHProcessor().AddMaterial(); // history
                }
                catch (Exception e)
                {
                    Console.WriteLine("The file 'mesh.ini' could not be write:");
                    Console.WriteLine(e.Message);
                }   
            }     
        }

        public ArrayList GetTemplatesList()
        {
            if(m_templatesList == null)
            {
                m_templatesList = new ArrayList();

                //String listPath = GetGeometryPath() + m_templateDir + "/templates.xml";
                String listPath = m_templateDir + "/templates.xml";

                try
                {
                    using(XmlReader templatesReader = XmlReader.Create(listPath))
                    {
                        templatesReader.MoveToContent();
                        while (templatesReader.Read())
                        {
                            if (templatesReader.Name == "template")
                            {
                                String templateSrc = templatesReader.GetAttribute("src");
                                String templateName = templatesReader.ReadString();                                
                                m_templatesList.Add(templateName);
                                m_templatesList.Add(templateSrc);
                            }
                        }
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                }
            }

            return m_templatesList;
        }

        private MaterialFile ReadMaterialXml(String xmlPath)
        {
            MaterialFile newMaterial = null;

            try
            {
                using (XmlReader xmlReader = XmlReader.Create(xmlPath))
                {
                    xmlReader.MoveToContent();
                    newMaterial = new MaterialFile();
                    bool isAnisotropy = false;
                    int anisotropyValue  = 0;

                    while (xmlReader.Read())
                    {
                        if (xmlReader.Name == "shader")
                        {
                            newMaterial.AddParam(xmlReader.Name,
                                                 null,
                                                 xmlReader.ReadString(),
                                                 null,
                                                 null,
                                                 null);
                        }
                        else if(xmlReader.Name == "state" || xmlReader.Name == "param")
                        {
                            if (xmlReader.GetAttribute("name") == "anisotropy")
                            {
                                isAnisotropy = true;
                                anisotropyValue = int.Parse(xmlReader.GetAttribute("value"));          
                            }
                            else
                            {
                                if ((xmlReader.GetAttribute("name") == "min_filter") || (xmlReader.GetAttribute("name") == "mipmap") ||
                                    (xmlReader.GetAttribute("name") == "mag_filter") || (xmlReader.GetAttribute("name") == "mipmap_bias") ||
                                    (xmlReader.GetAttribute("name") == "anisotropy_power"))
                                {
                                    isAnisotropy = false;
                                                                   
                                }
    
                                    newMaterial.AddParam(xmlReader.Name,
                                                      xmlReader.GetAttribute("name"),
                                                      xmlReader.GetAttribute("value"),
                                                      xmlReader.GetAttribute("type"),
                                                      xmlReader.GetAttribute("vmin"),
                                                      xmlReader.GetAttribute("vmax"));         
                            }

                        }
                        else if (xmlReader.Name == "texture" || xmlReader.Name == "texture3d" || xmlReader.Name == "cubemap")
                        {
                            newMaterial.AddParam(xmlReader.Name,
                                                 xmlReader.GetAttribute("src"), // в качестве имени параметра
                                                 xmlReader.GetAttribute("level"), // в качестве значени€
                                                 xmlReader.GetAttribute("alphablend"), // в качестве типа значени€
                                                 null,
                                                 null);
                        }
                        else if(xmlReader.Name == "animation")
                        {
                            newMaterial.AddParam(xmlReader.Name,
                                                 xmlReader.GetAttribute("level"), // в качестве имени параметра
                                                 xmlReader.GetAttribute("delta"), // в качестве значени€
                                                 xmlReader.ReadInnerXml(), // в качестве типа значени€
                                                 null,
                                                 null);
                        }
                    }

                    if (isAnisotropy)
                    {
                        if (anisotropyValue > 0)
                        {
                            newMaterial.AddParam("state", "min_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap", "2", null, null, null);
                            newMaterial.AddParam("state", "mag_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap_bias", "0.0", "float", "0.0", "3.0");
                            newMaterial.AddParam("state", "anisotropy_power", xmlReader.GetAttribute("value"), null, null, null);
                        }
                        else if (anisotropyValue == 0)
                        {
                            newMaterial.AddParam("state", "min_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap", "2", null, null, null);
                            newMaterial.AddParam("state", "mag_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap_bias", "0.0", "float", "0.0", "3.0");
                            newMaterial.AddParam("state", "anisotropy_power", "1", null, null, null);
                        }
                        else if (anisotropyValue == -3)
                        {
                            newMaterial.AddParam("state", "min_filter", "nearest", null, null, null);
                            newMaterial.AddParam("state", "mipmap", "1", null, null, null);
                            newMaterial.AddParam("state", "mag_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap_bias", "0.0", "float", "0.0", "3.0");
                            newMaterial.AddParam("state", "anisotropy_power", "1", null, null, null);
                        }
                        else if (anisotropyValue == -2)
                        {
                            newMaterial.AddParam("state", "min_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap", "0", null, null, null);
                            newMaterial.AddParam("state", "mag_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap_bias", "0.0", "float", "0.0", "3.0");
                            newMaterial.AddParam("state", "anisotropy_power", "1", null, null, null);
                        }
                        else if (anisotropyValue == -1)
                        {
                            newMaterial.AddParam("state", "min_filter", "nearest", null, null, null);
                            newMaterial.AddParam("state", "mipmap", "0", null, null, null);
                            newMaterial.AddParam("state", "mag_filter", "nearest", null, null, null);
                            newMaterial.AddParam("state", "mipmap_bias", "0.0", "float", "0.0", "3.0");
                            newMaterial.AddParam("state", "anisotropy_power", "1", null, null, null);
                        }
                        else
                        {
                            newMaterial.AddParam("state", "min_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap", "2", null, null, null);
                            newMaterial.AddParam("state", "mag_filter", "linear", null, null, null);
                            newMaterial.AddParam("state", "mipmap_bias", "0.0", "float", "0.0", "3.0");
                            newMaterial.AddParam("state", "anisotropy_power", "1", null, null, null);
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            return newMaterial;
        }

        public MaterialFile GetTemplate(int templateIndex)
        {
            return ReadMaterialXml(m_templateDir + "/" + (String)m_templatesList[templateIndex * 2 + 1]);
        }

        private String GetTemplateContent(int templateIndex)
        {
            String content = "";

            try
            {
                using (TextReader textReader = new StreamReader(m_templateDir + "/" + (String)m_templatesList[templateIndex * 2 + 1]))
                {
                    content = textReader.ReadToEnd();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            return content;
        }

        public MaterialFile GetMaterial(String textureName, int lodIndex)
        {
            return ReadMaterialXml((String)GetResoursesPaths()[lodIndex] + "/" + GetMaterialNameFromTexture(textureName, lodIndex));
        }

        private String GetMaterialContent(String materialName, int lod)
        {
            String content = "";

            try
            {
                using (TextReader textReader = new StreamReader((String)GetResoursesPaths()[lod] + "/" + materialName))
                {
                    content = textReader.ReadToEnd();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            return content;
        }

        public void SaveMaterial(String textureName, int lod, MaterialFile material)
        {
            ArrayList oldParams = new ArrayList();
            String materialName = GetMaterialNameFromTexture(textureName,lod);
            String line;

            // чтение необновленного файла материала
            try
            {
                using (StreamReader materialList = new StreamReader((String)GetResoursesPaths()[lod] + "/" + materialName))
                {
                    while ((line = materialList.ReadLine()) != null)
                    {
                        oldParams.Add(line);
                    }
                }
            }
            catch (IOException e)
            {
                Console.WriteLine(e.Message);
            }

            string[] contentArrOld = oldParams.ToArray(typeof(string)) as string[];
            // сохранить материал
            if(material != null)
            {
                ArrayList materialParams = material.GetParams();

                    try
                    {
                        ArrayList xmlLines = new ArrayList();
                        xmlLines.Add("<material>");

                        foreach (Hashtable param in materialParams)
                        {
                            String paramType = (String)param["paramType"];

                            if (paramType == "shader")
                            {
                                xmlLines.Add("\t<shader>" + (String)param["value"] + "</shader>");
                            }
                            else if (paramType == "state" || paramType == "param")
                            {
                                String paramString = "\t<" + paramType + " name=\"" + (String)param["paramName"] + "\" value=\"" + (String)param["value"] + "\" ";

                                if (param["valueType"] != null)
                                {
                                    paramString += "type=\"" + (String)param["valueType"] + "\" ";
                                }

                                if (param["valueMin"] != null)
                                {
                                    paramString += "vmin=\"" + (String)param["valueMin"] + "\" ";
                                }

                                if (param["valueMax"] != null)
                                {
                                    paramString += "vmax=\"" + (String)param["valueMax"] + "\" ";
                                }

                                paramString += "/>";
                                xmlLines.Add(paramString);
                            }
                            else if (paramType == "texture" || paramType == "texture3d" || paramType == "cubemap")
                            {
                                String paramString = "\t<" + paramType + " src=\"" + (String)param["paramName"] + "\" ";

                                if (param["valueType"] != null)
                                {
                                    paramString += "alphablend=\"" + (String)param["valueType"] + "\" ";
                                }

                                if (param["value"] != null)
                                {
                                    paramString += "level=\"" + (String)param["value"] + "\" ";
                                }

                                paramString += "/>";
                                xmlLines.Add(paramString);
                            }
                            else if (paramType == "animation")
                            {
                                xmlLines.Add("\t<animation level=\"" + (String)param["paramName"] + "\" delta=\"" + (String)param["value"] + "\">" +
                                    (String)param["valueType"] + "</animation>");
                            }
                        }

                        xmlLines.Add("</material>");

                        string res = (String)(GetResoursesPaths()[lod]) + "/" + materialName;
                        string[] contentArr = (String[])xmlLines.ToArray(typeof(String));

                        //если изменени€ в материале произошли, перезаписываем файл
                        bool isMaterialChanged = false;

                        if (contentArr.Length != contentArrOld.Length)
                        {
                            isMaterialChanged = true;
                        }
                                            
                        if (!isMaterialChanged)
                        for (int j = 0; j < contentArr.Length; j++)
                        {
                            if (contentArr[j] != contentArrOld[j]) 
                            {   
                                isMaterialChanged = true; 
                                break;
                            }
                        }

                        if ((isMaterialChanged) && (contentArrOld.Length != 0))
                        {
                            isSaveMaterial = true;
                            if (MessageBox.Show("¬ы действительно хотите сохранить материал: \n “екстура: " + textureName + "\n ќбъект: " + GetModelName(lod) + "\n Ћод: "+lod.ToString()+"?", "ѕодтверждение сохранени€ материала", MessageBoxButtons.OKCancel) == DialogResult.OK)
                            {
                                WriteFile(res, contentArr, xmlLines);
                            }
                        }
                        else if (contentArrOld.Length == 0)
                        {
                           WriteFile(res, contentArr, xmlLines);
                        }
                        isSaveMaterial = false;

                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e.Message);
                    } 
            }
        }

        private void WriteFile(String res, String[] contentArr, ArrayList xmlLines)
        {
            File.WriteAllLines(res, contentArr);

            string content = "";
            for (int i = 0; i < xmlLines.Count; i++)
            {
                content += (string)xmlLines[i];
            }

            Commander.GetCommander().AddUserLog(0, res, content);

            mainForm.ReloadServers();
        }

        public void SetNewObject(String objectName)
        {
            m_objectName = objectName;
            m_modelName = null;
            m_texturesList = null;
            m_materialsList = null;
            m_resPaths = null;
            // процедура очистки "кэша"
        }

        public String GetParamTranslate(String value)
        {
            return m_translates.GetTranslate(value);
        }

        public void SwitchState(String textureName, int lod, String state)
        {
            String materialName = GetMaterialNameFromTexture(textureName, lod);

            Hashtable sendingParam = new Hashtable();
            sendingParam.Add("updateFlag", 2);
            sendingParam.Add("materialName", materialName);
            sendingParam.Add("state", state);

            if (state.CompareTo("on") == 0)
            {
                String materialContent = GetMaterialContent(materialName, lod);
                sendingParam.Add("materialContent", materialContent);
            }

            Object3D.GetObject3D().Material = sendingParam;
        }

        public void OnCreated(String materialName, int templateIndex)
        {
            String materialContent = GetTemplateContent(templateIndex);
            //String materialName = GetMaterialNameFromTexture(textureName, 0);

            Hashtable sendingParam = new Hashtable();
            sendingParam.Add("updateFlag", 1);
            sendingParam.Add("materialName", materialName);
            sendingParam.Add("materialContent", materialContent);

            Object3D.GetObject3D().Material = sendingParam;
        }

        public MaterialFile GetTemplateItems()
        {
            if(m_templateItems == null)
            {
                m_templateItems = ReadMaterialXml(m_templateDir + "/template_items.xml");
                m_templateItems.AddParam("state", "anisotropy", "-1", null, null, null);
            }

            return m_templateItems;
        }

        public bool IsAvatar(String objName)
        {
            String queryStr = "SELECT `type` FROM `objects_map` WHERE `name`='" + objName + "'";
            ArrayList queryResult = m_mySql.QueryA(queryStr);
            if(queryResult != null)
            {
                if((Int32)(((Hashtable)(queryResult[0]))["type"]) == 8)
                {
                    return true;
                }
            }

            return false;
        }

        public String GetObjectName()
        {
            return m_objectName;
        }
    }
}
