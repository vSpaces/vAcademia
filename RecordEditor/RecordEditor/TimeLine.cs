using System;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Collections.Generic;
using RecordEditor.Logic;

namespace RecordEditor
{
    class TimeLine
    {
        // область timeline
        Rectangle m_timeLineRect;


        public TimeLine(Rectangle rect)
        {
            m_timeLineRect = rect;
        }

        public void Draw(System.Windows.Forms.PaintEventArgs e, Control control, HScrollBar scroll)
        {
//            m_timeLineRect = e.ClipRectangle;

            Graphics g = control.CreateGraphics();

            // Draw background
/*            SolidBrush backgroundBrush = new SolidBrush(control.BackColor);
            g.FillRectangle(backgroundBrush, control.ClientRectangle);
            backgroundBrush.Dispose();

            //Draw lines
            int step = 10;
            int up = 0;

            int width = m_timeLineRect.Width / step;

            int longLineLength = m_timeLineRect.Height * 2 / 3;
            int middleLineLength = m_timeLineRect.Height * 5 / 6;
            int shortLineLength = m_timeLineRect.Height / 2;

            for (uint i = 1; i < width; ++i)
            {
                if (i % 6 == 0)
                    up = longLineLength;
                else if (i % 3 == 0)
                    up = middleLineLength;
                else
                    up = shortLineLength;

                g.DrawLine(Pens.Blue, i * step, m_timeLineRect.Height - 2, i * step, m_timeLineRect.Height - up);
            }

            Point point = control.Location;
            point.X -= 1;
            control.Location = point;
*/
            //Draw time
/*            Font font = new Font("Arial", 9, GraphicsUnit.Pixel);
            StringFormat sf = new StringFormat();
            sf.Alignment = StringAlignment.Near;
            sf.LineAlignment = StringAlignment.Center;
            int x = startX;
            int t = (int)(startTime * 1000);
            while (x < this.Width)
            {
                Rectangle textRect = new Rectangle(x - 4, TIMELINEHEIGHT + TIMELINETOP - 13, 30, 20);

                string digitText = "" + (int)(t / 1000);
                SolidBrush sbText = new SolidBrush(Settings.TimeLineColor);
                e.Graphics.DrawString(digitText, font, sbText, textRect.Left, textRect.Top);
                sbText.Dispose();

                Pen pn = new Pen(Settings.TimeLineColor, 1);
                int divcount = 5;
                for (int ix = 0; ix < divcount; ix++)
                {
                    int cx = x + (int)(ix * (float)PIXELSPERSECOND / divcount);
                    e.Graphics.DrawLine(pn, cx, TIMELINEHEIGHT - 3, cx, TIMELINEHEIGHT - 1);
                }
                pn.Dispose();

                x += PIXELSPERSECOND;
                t += 1000;
            }*/

        }
    }
}
