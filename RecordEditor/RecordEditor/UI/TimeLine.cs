using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using RecordEditor.Logic;
using RecordEditor;

namespace UI
{
    public partial class TimeLine : FlashGenie.sePanel
    {
        static int LeftPadding = 107;
        static int RightPadding = 8;
        static int TopPadding = 44;
        static int SmallLineHeight = 6;
        static int NormalLineHeight = 8;
        static int BigLineHeight = 13;
        /* static int SubdividesCount = 5;            // количество мелких отсечек между крупными
        static int MinDistanceBetweenDots = 21;    // минимальное расстояние между ячейками
        static int MaxDistanceBetweenDots = 42;    // максимальное расстояние между ячейками*/

        // Параметры рассчетов
        int alignedStartTime = 0;
        int alignedEndTime = 0;
        int trunkedStartTime = 0;
        int trunkedEndTime = 0;
        int drawStep = 0;
        float drawingMultiplier = 1;

        // Графические объекты
        Pen dottedPen = null;
        Pen solidPen = null;
        Pen selectionPen = null;
        SolidBrush transparentBrush = null;
        SolidBrush solidBrush = null;
        Font numbersFont = null;

        // Параметры выделения
        bool selectingInProgress = false;
        bool intervalSelected = false;
        TimeInterval mSelectedTimeInterval = new TimeInterval();

        double k = 1;

        public TimeLine()
        {
            InitializeComponent();

            dottedPen = new Pen(Color.White, 1);
            dottedPen.DashStyle = DashStyle.Dot;
            solidPen = new Pen(Color.White, 1);
            selectionPen = new Pen(Color.FromArgb(255, 255, 120, 0));
            numbersFont = new Font("Arial", 10, GraphicsUnit.Pixel);
            solidBrush = new SolidBrush(Color.White);
            transparentBrush = new SolidBrush(Color.FromArgb(128, 255, 120, 0));
        }

        public void SubscribeEvents()
        {
            Program.context.recordController.OnRecordOpenSuccess += new global::RecordEditor.Logic.RecordControllerEventHandler(recordController_OnRecordOpenSuccess);
        }

        void recordController_OnRecordOpenSuccess(object sender, global::RecordEditor.Logic.RecordControllerEventArgs args)
        {
            RecordDocument recordDocument = sender as RecordDocument;

            recordDocument.OnPlayStateChanged += new EventHandler(recordDocument_OnPlayStateChanged);
            recordDocument.OnPlayPositionChanged += new EventHandler(recordDocument_OnPlayPositionChanged);

            btnRewind.Enabled = true;
            btnPlay.Enabled = true;
            TimeInterval = new TimeInterval( 0, recordDocument.Description.duration);
        }

        void recordDocument_OnPlayPositionChanged(object sender, EventArgs e)
        {
            // ??
        }

        void recordDocument_OnPlayStateChanged(object sender, EventArgs e)
        {
            RecordDocument recordDocument = sender as RecordDocument;

            btnPlay.Visible = recordDocument.PlayMode == PlayMode.MODE_PLAY ? false : true;
            btnPause.Visible = !btnPlay.Visible;
        }

        public TimeInterval mTimeInterval = new TimeInterval();
        public TimeInterval TimeInterval
        {
            get
            {
                return mTimeInterval;
            }
            set
            {
                if (value.Duration >= 0)
                {
                    mTimeInterval = value;
                    OnTimeIntervalChanged();
                }
            }
        }

        protected void OnTimeIntervalChanged()
        {
            UpdateTimeIntervalParameters();
            Invalidate();
            Update();
        }

        protected void UpdateTimeIntervalParameters()
        {
            if (mTimeInterval.Duration == 0)
                return;

            // переводим время в границу от 1 до 100
            drawingMultiplier = 1;
            if (mTimeInterval.Duration < 100)
            {
                while (mTimeInterval.Duration * drawingMultiplier <= 10)
                    drawingMultiplier *= 10;
            }
            else if (mTimeInterval.Duration > 100)
            {
                while (mTimeInterval.Duration * drawingMultiplier > 100)
                    drawingMultiplier /= 10;
            }

            drawStep = 10;
            if (mTimeInterval.Duration * drawingMultiplier <= 20)
                drawStep = 2;
            else if (mTimeInterval.Duration * drawingMultiplier <= 50)
                drawStep = 5;

            // умножаем границы на коэффициент
            alignedStartTime = (int)(mTimeInterval.IntervalStart * drawingMultiplier);
            alignedEndTime = (int)(mTimeInterval.IntervalEnd * drawingMultiplier);

            if (alignedStartTime == alignedEndTime)
                return;

            trunkedStartTime = (alignedStartTime - (alignedStartTime % drawStep));
            trunkedEndTime = (alignedEndTime - (alignedStartTime % drawStep)) + drawStep;

        }

        protected int    TimeToX( double aTime)
        {
            if (alignedEndTime - alignedStartTime == 0)
                return LeftPadding;
            // Пропорция такая
            // (alignedEndTime - alignedStartTime) - drawWidth
            // 1                                   - x
            // x = drawWidth / (alignedEndTime - alignedStartTime)
            double drawWidth = (int)Width - LeftPadding - RightPadding;
            double widthMult = drawWidth / (alignedEndTime - alignedStartTime);

            return (int)(LeftPadding + (aTime - trunkedStartTime) * widthMult - (alignedStartTime - trunkedStartTime) * widthMult);
        }

        protected double XToTime(int aX)
        {
            if (alignedEndTime - alignedStartTime == 0)
                return LeftPadding;
            // Пропорция такая
            // (alignedEndTime - alignedStartTime) - drawWidth
            // 1                                   - x
            // x = drawWidth / (alignedEndTime - alignedStartTime)
            double drawWidth = (int)Width - LeftPadding - RightPadding;
            double widthMult = drawWidth / (alignedEndTime - alignedStartTime);

            return (aX - LeftPadding) / widthMult + trunkedStartTime + (alignedStartTime - trunkedStartTime);
        }

        // Отрисуем панель
        // "K:\VU2008\interface\редактор записей\v3.psd" 
        private void TimeLine_Paint(object sender, PaintEventArgs e)
        {
            int i=0;
            int previousX = 0;
            for (i = 0; i <= 10; i++ )
            {
                double timeToDraw = trunkedStartTime + i * drawStep;

                int ax = TimeToX(timeToDraw);
                if (ax >= LeftPadding && ax <= Width - RightPadding)
                {
                    e.Graphics.DrawLine(solidPen, ax, TopPadding, ax, TopPadding - BigLineHeight);

                    double realTime = timeToDraw / drawingMultiplier;
                    System.Drawing.StringFormat format = new StringFormat();
                    format.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(FormatTime(realTime), numbersFont, solidBrush, new Point((int)ax, (int)TopPadding), format);
                }

                if( i != 0)
                {
                    int j;
                    double dx = (ax - previousX) / 10.0;
                    for (j = 1; j < 10; j++ )
                    {
                        int bx = (int)(previousX + dx * j);
                        if (bx >= LeftPadding && bx <= Width - RightPadding)
                        {
                            e.Graphics.DrawLine(solidPen, bx, TopPadding - 1, bx, TopPadding - 1 - ((j % 2 == 1) ? SmallLineHeight : NormalLineHeight));
                        }
                    }
                }

                previousX = ax;
            }

            e.Graphics.DrawLine(dottedPen, LeftPadding, TopPadding, Width - RightPadding, TopPadding);

            if (selectingInProgress || intervalSelected)
            {
                int selectionStartX = TimeToX( mSelectedTimeInterval.IntervalStart);
                int selectionEndX = TimeToX( mSelectedTimeInterval.IntervalEnd);

                e.Graphics.DrawLine(selectionPen, selectionStartX, 0, selectionStartX, Height);
                e.Graphics.DrawLine(selectionPen, selectionEndX, 0, selectionEndX, Height);

                e.Graphics.FillRectangle(transparentBrush, Math.Min(selectionStartX, selectionEndX), 0, Math.Abs(selectionEndX - selectionStartX), Height);
                
            }

            if( Program.context.recordController.CurrentRecord != null)
            {
                int playTimeX = TimeToX(Program.context.recordController.CurrentRecord.PlayPosition);
                e.Graphics.DrawLine(selectionPen, playTimeX, 0, playTimeX, Height);
            }
        }

        private string FormatTime(double adTime)
        {
            return "";
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            this.Invalidate();
            this.Update();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            mTimeInterval.IntervalStart += 11;
            TimeInterval = mTimeInterval;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            mTimeInterval.IntervalEnd += 12;
            TimeInterval = mTimeInterval;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            k *= 2;
        }

        private void TimeLine_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.X >= LeftPadding && e.X <= Width - RightPadding)
            {
                selectingInProgress = true;
                mSelectedTimeInterval.IntervalStart = XToTime( e.X);
                mSelectedTimeInterval.IntervalEnd = mSelectedTimeInterval.IntervalStart;
            }
        }

        private void TimeLine_MouseUp(object sender, MouseEventArgs e)
        {
            selectingInProgress = false;
            mSelectedTimeInterval.IntervalEnd = XToTime( e.X);

            if (mSelectedTimeInterval.Duration == 0)
                intervalSelected = false;
            else
            {
                intervalSelected = true;
            }
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.SelectedTimeInterval = mSelectedTimeInterval;
        }

        private void TimeLine_MouseMove(object sender, MouseEventArgs e)
        {
            if (selectingInProgress)
            {
                mSelectedTimeInterval.IntervalEnd = XToTime(e.X);
                Invalidate();
                Update();
            }
        }

        private void btnRewind_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.Rewind();
        }

        private void btnPlay_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.Play();
        }

        private void btnPause_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.Pause();
        }
    }
}
