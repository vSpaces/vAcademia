using System;
using System.Collections;
using System.Text;
using System.Xml;

namespace VisualWorldEditor
{
    class Translate
    {
        // переводы параметров материалов
        // по английскому названию возвращает русское
       
        private String m_translatesFile = "strings.xml";

        private Hashtable m_translates = new Hashtable();

        public Translate(String dirPath)
        {
           FillTranslates(dirPath);
        }

        private void FillTranslates(String dirPath)
        {
            String fullPath = dirPath + m_translatesFile;

            try
            {
                using (XmlReader xmlReader = XmlReader.Create(fullPath))
                {
                    xmlReader.MoveToContent();
                    while (xmlReader.Read())
                    {
                        if (xmlReader.Name == "translate")
                        {
                            m_translates.Add(xmlReader.GetAttribute("name"), xmlReader.ReadString());
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        public String GetTranslate(String value)
        {
            return (String)m_translates[value];
        }
    }
}
