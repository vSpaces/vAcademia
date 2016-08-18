using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace VisualWorldEditor
{
    class EditorNumVScroll: VScrollBar
    {
        EditorTextBox m_numTextBox;

        int m_scrollStep = 1;

        public EditorNumVScroll(EditorTextBox numTextBox): base()
        {
            m_numTextBox = numTextBox;
            InitScroll();
        }

        public int ScrollStep
        {
            get
            {
                return m_scrollStep;
            }
            set
            {
                m_scrollStep = value;
            }
        }

        void InitScroll()
        {
            // Уменьшим размер бокса, чтобы поместился скролл
            m_numTextBox.Size = new Size(m_numTextBox.Size.Width - m_numTextBox.Size.Height / 2, m_numTextBox.Size.Height);

            this.SmallChange = m_scrollStep;
            this.LargeChange = m_scrollStep;
            this.Minimum = -m_scrollStep;
            this.Maximum = m_scrollStep;
            this.Value = 0;

            // позиция
            Point scrollPosition = new Point();
            scrollPosition.X = m_numTextBox.Location.X + m_numTextBox.Size.Width;
            scrollPosition.Y = m_numTextBox.Location.Y;
            this.Location = scrollPosition;

            // размер
            Size scrollSize = new Size();
            scrollSize.Height = m_numTextBox.Height;
            scrollSize.Width = scrollSize.Height / 2;
            this.Size = scrollSize;

            
            // подключение обработчиков
            this.Scroll += new ScrollEventHandler(scrolling);
        }

        private void scrolling(Object sender, ScrollEventArgs e)
        {
            if (Object3D.GetObject3D().ModifyLevel == 0 ||
                Object3D.GetObject3D().ModifyLevel == 2)
            {
                if (m_numTextBox.Enabled && (e.Type == ScrollEventType.SmallDecrement || e.Type == ScrollEventType.SmallIncrement))
                {
                    float value = float.Parse(m_numTextBox.Text);
                    if (e.NewValue < 0)
                    {
                        value += m_scrollStep;
                        m_numTextBox.Text = value.ToString("F3");
                    }
                    else if (e.NewValue > 0)
                    {
                        value -= m_scrollStep;
                        m_numTextBox.Text = value.ToString("F3");
                    }

                    e.NewValue = 0;

                    // форсирование события отправки данных
                    m_numTextBox.FireReturnKey();
                }
            }
            else
            {
                //MessageBox.Show("Объект защищен от изменений свойств.", "Защита", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }  
        }
    }
}
