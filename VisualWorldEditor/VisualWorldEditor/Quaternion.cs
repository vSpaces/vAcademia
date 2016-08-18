using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    class Quaternion
    {
        private float m_x;
        private float m_y;
        private float m_z;
        private float m_w;

        public Quaternion()
        {
            m_x = 0;
            m_y = 0;
            m_z = 0;
            m_w = 1;
        }

        public Quaternion(float _x, float _y, float _z, float _w)
        {
            m_x = _x;
            m_y = _y;
            m_z = _z;
            m_w = _w;
        }

        public void Set(float _x, float _y, float _z, float _w)
        {
            m_x = _x;
            m_y = _y;
            m_z = _z;
            m_w = _w;
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

        public float W
        {
            get { return (m_w); }
            set { m_w = value; }
        }

        public static Quaternion GetNaN()
        {
            Quaternion nanQuaternion = new Quaternion();
            nanQuaternion.Set(float.NaN, float.NaN, float.NaN, float.NaN);

            return nanQuaternion;
        }

        public static bool IsNaN(Quaternion q)
        {
            if (float.IsNaN(q.X) && float.IsNaN(q.Y) && float.IsNaN(q.Z) && float.IsNaN(q.W))
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
