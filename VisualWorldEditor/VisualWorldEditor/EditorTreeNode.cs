using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    class EditorTreeNode : TreeNode
    {
        private Int32 m_id;
        private Int32 m_subId;
        private String m_name;
        private String m_res;

        public EditorTreeNode(): base()
        {
            m_id = (Int32)(-1);
            m_subId = (Int32)(-1);
            m_name = "";
            m_res = "";
        }

        public Int32 GetId()
        {
            return m_id;
        }

        public void SetId(Int32 id)
        {
            m_id = id;
        }

        public Int32 GetSubId()
        {
            return m_subId;
        }

        public void SetSubId(Int32 subId)
        {
            m_subId = subId;
        }

        public String GetName()
        {
            return m_name;
        }

        public void SetName(String name)
        {
            m_name = name;
        }

        public String GetResourse()
        {
            return m_res;
        }

        public void SetResourse(String resourse)
        {
            m_res = resourse;
        }

        public override String ToString()
        {
            return m_name;
        }
    }
}
