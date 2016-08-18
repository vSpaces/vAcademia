using System;
using System.Collections;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace VisualWorldEditor
{
    partial class Object3D
    {
        private String[] m_names;

        private Vector m_transformation;
        private Vector m_scale;
        private Hashtable m_materialParam;

        private int m_modifyLevel;
        private int m_collisionFlag;

        public int CollisionFlag
        {
            get
            {
                return m_collisionFlag;
            }
            set
            {
                m_collisionFlag = value;
                ObjectEditedEventArgs objectEvargs = new ObjectEditedEventArgs(value);
                OnCollisionFlagChanged(objectEvargs);
            }
        }

        public struct LodShadow
        {
            public float distance;
            public int shadow;
            public string meshName;

            public LodShadow(float _distance, int _shadow, string _meshName)
            {
                distance = _distance;
                shadow = _shadow;
                meshName = _meshName;
            }

            public override string ToString()
            {
                return "Лод " + distance.ToString();
            }
        };

        private ArrayList m_shadowsList;

        private static readonly Object3D instance = new Object3D();

        private Object3D()
        {
            m_names = new String[] { };
            m_transformation = new Vector();
            m_scale = new Vector();

            m_materialParam = null;
            m_shadowsList = null;
        }

        public static Object3D GetObject3D()
        {
            return instance;
        }

        public void DeleteInfo()
        {
            this.Names = new String[] {};
            this.Transformation = Vector.GetNaN();
            this.Scale = Vector.GetNaN();
            this.ShadowsList = new ArrayList();
            this.ModifyLevel = 0;
        }

        public String[] Names
        {
            get {
                    return (m_names);
                }
            set {
                    m_names = value;
                    String selectString = "";
                    ObjectEditedEventArgs objectEvargs = null;
                    bool readOnlyTextBox = true;
                    
                    if(m_names.Length > 0 )
                    {
                        
                        if(m_names.Length == 1)
                        {
                            selectString = m_names[0];
                            readOnlyTextBox = false;
                        }
                        else
                        {
                            selectString = m_names.Length.ToString() + " объектов выделено";
                            readOnlyTextBox = true;
                        }
                        
                    }

                    objectEvargs = new ObjectEditedEventArgs(new Object[] {selectString, readOnlyTextBox});
                    OnNamesChanged(objectEvargs);        
                }
        }

        public Vector Transformation
        {
            get {
                    return (m_transformation);
                }
            set {
                    m_transformation = value;
                    ObjectEditedEventArgs objectEvargs = new ObjectEditedEventArgs(m_transformation);
                    OnTransformationChanged(objectEvargs);
                }
        }

        public Vector Scale
        {
            get {
                    return (m_scale);
                }
            set {
                    m_scale = value;
                    ObjectEditedEventArgs objectEvargs = new ObjectEditedEventArgs(m_scale);
                    OnScaleChanged(objectEvargs);
                }
        }

        public Hashtable Material
        {
            get {
                    return (m_materialParam);
                }
            set {
                    m_materialParam = value;
                    ObjectEditedEventArgs objectEvargs = new ObjectEditedEventArgs(m_materialParam);
                    OnMaterialChanged(objectEvargs);
                }
        }

        public ArrayList ShadowsList
        {
            get {
                    return (m_shadowsList);
                }
            set {
                    m_shadowsList = value;
                    ObjectEditedEventArgs objectEvargs = new ObjectEditedEventArgs(m_shadowsList);
                    OnShadowsListChanged(objectEvargs);
                }
        }

        public int ModifyLevel
        {
            get {
                    return (m_modifyLevel); 
                }
            set {
                    m_modifyLevel = value;
                    ObjectEditedEventArgs objectEvargs = new ObjectEditedEventArgs(m_modifyLevel);
                    OnModifyLevelChanged(objectEvargs);
                }
        }
    }
}
