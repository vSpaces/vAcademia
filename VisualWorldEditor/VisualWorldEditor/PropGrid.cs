using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{

    public partial class PropGrid : Form
    {
        private Commander m_commander = null;
        private ArrayList m_objectProps = null;

        private bool m_shown = false;

        public PropGrid()
        {
            InitializeComponent();
            
        }

        public void ShowProperties()
        {
            m_commander = Commander.GetCommander();

            if(m_commander != null)
            {
                Object3D.GetObject3D().NamesChanged += new ObjectEditedEventHandler(ReloadGrid);

                m_shown = true;

                FillGrid();

                this.ShowDialog();

                m_shown = false;
            }
        }

        private void FillGrid()
        {
            if(Object3D.GetObject3D().Names.Length > 0 && m_shown)
            {
                m_objectProps = m_commander.GetObjectInfo();

                propertyGrid.PropertySort = PropertySort.Alphabetical;
                if (m_objectProps != null)
                {
                    propertyGrid.SelectedObject = new PropGridObject(m_objectProps);
                }
                else
                {
                    propertyGrid.SelectedObject = null;
                }
            }
            else
            {
                propertyGrid.SelectedObject = null;
            }
        }

        private delegate void ReloadGridDelegate(Object sender, ObjectEditedEventArgs args);
        private void ReloadGrid(Object sender, ObjectEditedEventArgs args)
        {
            if(this.InvokeRequired)
            {
                ReloadGridDelegate reloadGrid = new ReloadGridDelegate(ReloadGrid);
                this.BeginInvoke(reloadGrid, new object[] { sender, args });
            }
            else
            {
                FillGrid();
            }           
        }

        private void OnGetCamera(object s, MouseEventArgs e)
        {            
            Vector eye = m_commander.GetCurrentCameraEye();
            Vector lookAt = m_commander.GetCurrentCameraLookAt();

            string res = "\"cam_coords\":{\"dx\": ";
            float a = eye.X;
            res += a.ToString("F1");
            res += ",\"dy\":";
            a = eye.Y;
            res += a.ToString("F1");
            res += ",\"dz\":";
            a = eye.Z;
            res += a.ToString("F1");
            res += ",\"ex\":";
            a = lookAt.X;
            res += a.ToString("F1");
            res += ",\"ey\":";
            a = lookAt.Y;
            res += a.ToString("F1");
            res += ",\"ez\":";
            a = lookAt.Z;
            res += a.ToString("F1");
            res += "}";

            string param = ((PropGridObject)propertyGrid.SelectedObject).m_params;
            int startPos = param.IndexOf("\"cam_coords\"");
            if (startPos != -1)
            {
                string oldValue = param;

                int endPos = param.IndexOf("}", startPos);
                string oldParam = param.Substring(startPos, endPos - startPos + 1);
                param = param.Replace(oldParam, res);

                ((PropGridObject)propertyGrid.SelectedObject).m_params = param;
                ((PropGridObject)propertyGrid.SelectedObject).SetProperty("Параметры", new TypeConverter(param));
                onPropertyValueChanged("Параметры", oldValue, param);
            }
        }

        private void onPropertyValueChanged(string label, string oldValue, string value)
        {
            if (label == "Блокировка")
            {
                m_commander.SendChangeLockLevel(Int32.Parse((String)oldValue), Int32.Parse((String)value));
            }
            else if (Object3D.GetObject3D().ModifyLevel == 0 ||
                    Object3D.GetObject3D().ModifyLevel == 2)
            {
                if (label == "Класс")
                {
                    m_commander.SendChangeClass((String)oldValue, (String)value);
                }
                else if (label == "Параметры")
                {
                    m_commander.SendChangeParams((String)oldValue, (String)value);
                }
            }
            else
            {
                ((PropGridObject)propertyGrid.SelectedObject).SetProperty(label, new TypeConverter(oldValue));
                mainForm.ShowObjectLockError(Object3D.GetObject3D().Names[0]);                    
                return;
            }
        }

        private void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            onPropertyValueChanged(e.ChangedItem.Label, (String)e.OldValue, (String)e.ChangedItem.Value);
        }

        private void cameraButton_Click(object sender, EventArgs e)
        {

        }
    }

    public class TypeConverter
    {
        public Object obj = null;

        public TypeConverter(Object o)
        {
            obj = o;
        }

        public static implicit operator String(TypeConverter o)
        {
            return o.obj.ToString();
        }

        public static implicit operator Int32(TypeConverter o)
        {
            return Int32.Parse(o.obj.ToString());
        }

    };
}