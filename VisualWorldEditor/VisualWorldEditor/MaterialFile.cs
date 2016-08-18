using System;
using System.Collections;
using System.Text;

namespace VisualWorldEditor
{
    class MaterialFile
    {
        ArrayList m_materialParams;

        public bool isBillboard = false;

        public MaterialFile()
        {
            m_materialParams = new ArrayList();
        }

        public void AddParam(String paramType, String paramName, String value, String valueType, String valueMin, String valueMax)
        {
            Hashtable param = new Hashtable();
            param.Add("paramType", paramType);
            param.Add("paramName", paramName);
            param.Add("value", value);
            param.Add("valueType", valueType);
            param.Add("valueMin", valueMin);
            param.Add("valueMax", valueMax);

            m_materialParams.Add(param);

            if(paramName == "billboard_orient")
            {
                isBillboard = true;
            }
        }

        public void AddParam(Hashtable param)
        {
            m_materialParams.Add(param);
        }

        public ArrayList GetParams()
        {
            return m_materialParams;
        }

        public Hashtable GetParam(String paramName)
        {
            foreach(Hashtable param in m_materialParams)
            {
                if(((String)param["paramType"] == "state" || (String)param["paramType"] == "param") &&
                    (String)param["paramName"] == paramName)
                {
                    return param;
                }
            }

            return null;
        }

        public void ChangeParam(String materialName, String paramName, String paramValue)
        {
            Hashtable sendingParam = null;

            foreach(Hashtable param in m_materialParams)
            {
                if(param.ContainsValue(paramName))
                {
                    if (((String)param["valueType"] == "texture") || ((String)param["valueType"] == "cubemap"))
                    {
                        Hashtable textureParam = SetNewTexture((String)param["value"], paramValue);
                        if(textureParam != null)
                        {
                            sendingParam = new Hashtable(param);
                            sendingParam["paramName"] = textureParam["value"];
                            sendingParam["value"] = textureParam["paramName"];
                            sendingParam["valueType"] = textureParam["paramType"];

                            mainForm.ReloadServers();
                        }                        
                        break;
                    }
                    else
                    {
                        param["value"] = paramValue;
                        sendingParam = new Hashtable(param);
                        break;
                    }
                }
            }


            if(sendingParam != null)
            {
                sendingParam.Add("updateFlag", 0);
                sendingParam.Add("materialName", materialName);
                Object3D.GetObject3D().Material = sendingParam; // גûחûגאוע סמבûעטו
            }
        }

        private Hashtable SetNewTexture(String level, String texSrc)
        {
            foreach (Hashtable param in m_materialParams)
            {
                if ((param["paramType"] != null) && (param["value"] != null))
                if(((String)param["paramType"] == "texture" || (String)param["paramType"] == "cubemap") && (String)param["value"] == level)
                {
                    param["paramName"] = texSrc;
                    return param;
                }
            }

            mainForm.ReloadServers();

            return null;
        }
    }
}
