using System;
using System.Collections;
using System.Text;
using System.ComponentModel;
using Jayrock.Json.Conversion;
using Jayrock.Json;

namespace VisualWorldEditor
{
    class PropGridObject
    {
        private String m_class = "";
        private Int32 m_lockLevel = 0;
        public String m_params = "";

        internal class ListToProperty: StringConverter
        //internal class ListToProperty : Int32Converter
        {
            public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
            {
                //True - means show a Combobox
                //and False for show a Modal 
                return true;
            }

            public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
            {
                //False - a option to edit values 
                //and True - set values to state readonly
                return true;
            }

            public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
            {
                return new StandardValuesCollection(new String[] { "0 - Нет", "1 - Свойств", "2 - Геометрии", "3 - Полная" });
                //return new StandardValuesCollection(new Int32[] { 0, 1, 2, 3 });
            }
        };

        public PropGridObject(ArrayList propSet)
        {
            if(propSet.Count == 1)
            {
                Hashtable objectProps = (Hashtable)propSet[0];
                m_class = (String)objectProps["class"];
                m_lockLevel = (Int32)objectProps["lock_level"];
                m_params = (String)objectProps["params"];

                /*try
                {
                    JsonArray jsarr = (JsonArray)JsonConvert.Import("["+m_params+"]");
                    
                    for(int jsIndex = 0; jsIndex < jsarr.Count; jsIndex++)
                    {
                        JsonObject jsObj = (JsonObject)jsarr[jsIndex];
                    }
                }
                catch(Exception e)
                {
                    Console.WriteLine(e.Message);
                }*/
            }            
        }

        public String Класс
        {
            get
            {
                return m_class;
            }
            set
            {
                m_class = value;
            }
        }

        public String Параметры
        {
            get
            {
                return m_params;
            }
            set
            {
                m_params = value;
            }
        }

        [TypeConverter(typeof(ListToProperty))]
        public String Блокировка
        {
            get
            {
                return m_lockLevel.ToString();
            }
            set
            {               
                m_lockLevel = Int32.Parse(value[0].ToString());
            }
        }

        public void SetProperty(String name, TypeConverter value)
        {
            if(name == "Класс")
            {
                m_class = value;
            }
            else if (name == "Блокировка")
            {
                m_lockLevel = value;
            }
            else if (name == "Параметры")
            {
                m_params = value;
            }
        }       
    }
}
