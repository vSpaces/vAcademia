using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    public class Vector
    {
        private float m_x;
        private float m_y;
        private float m_z;

        public Vector(float _x, float _y, float _z)
        {
            m_x = _x;
            m_y = _y;
            m_z = _z;
        }

        public Vector()
        {
            m_x = 0;
            m_y = 0;
            m_z = 0;
        }

        public void Set(float _x, float _y, float _z)
        {
            m_x = _x;
            m_y = _y;
            m_z = _z;
        }

        public float X
        {
            get { return (m_x); }
            set { m_x = value; }
        }

        public float Y
        {
            get { return (m_y); }
            set { m_y = value; }
        }

        public float Z
        {
            get { return (m_z); }
            set { m_z = value; }
        }

        public static Vector GetNaN()
        {
            Vector nanVector = new Vector();
            nanVector.Set(float.NaN, float.NaN, float.NaN);

            return nanVector;
        }

        public static bool IsNaN(Vector v)
        {
            if(float.IsNaN(v.X) && float.IsNaN(v.Y) && float.IsNaN(v.Z))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
