using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    class EditorTextBox: TextBox
    {
        private bool m_deltaMode;

        public event EventHandler DeltaModeChanged;
        protected virtual void OnDeltaModeChanged(EventArgs args)
        {
            if (DeltaModeChanged != null)
            {
                DeltaModeChanged(this, args);
            }
        }

        public EditorTextBox()
        {
            m_deltaMode = false;
        }

        public bool DeltaMode
        {
            get {
                    return (m_deltaMode);
                }
            set {
                    m_deltaMode = value;
                    OnDeltaModeChanged(new EventArgs());
                }
        }

        public void FireReturnKey()
        {
            KeyPressEventArgs e = new KeyPressEventArgs((char)Keys.Return);
            this.OnKeyPress(e);
        }
    }
}
