using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Linq;
using System.Text;
using UI;

namespace RecordEditor.Logic
{
    /*
     * Класс хранит данные об открытой записи.
     * Генерит события изменения длительности, текущего положения, режима воспроизведения записи
     */
    public class RecordDocument
    {
        /// <summary>
        /// Вызывается при смене состояния воспроизведения
        /// </summary>
        /// <returns></returns>
        public delegate void OnPlayStateChangedDelegate();
        public event EventHandler OnPlayStateChanged = null;

        /// <summary>
        /// Вызывается при смене текущего времени воспроизведения
        /// </summary>
        /// <returns></returns>
        public delegate void OnPlayPositionChangedDelegate();
        public event EventHandler OnPlayPositionChanged = null;

        #region  Private members

        double pleyPosition = 0;
        Timer timerPlay = new System.Windows.Forms.Timer();

        #endregion  //Private members

        public RecordDocument()
        {
            timerPlay.Enabled = false;
            timerPlay.Interval = 100;
            timerPlay.Tick += new EventHandler(timerPlay_Tick);
        }

        void timerPlay_Tick(object sender, EventArgs e)
        {
            PlayPosition += ((double)timerPlay.Interval / 1000);
        }

        #region Public properties

        RecordDescription recordDescription = null;
        public RecordDescription Description
        {
            get
            {
                return recordDescription;
            }
            set
            {
                recordDescription = value;
            }
        }

        PlayMode playMode = PlayMode.MODE_PAUSE;
        public PlayMode PlayMode
        {
            get
            {
                return playMode;
            }
            set
            {
                if (playMode != value)
                {
                    playMode = value;
                    HandlePlayStateChanged();
                }
            }
        }

        double playPosition = 0;
        public double PlayPosition
        {
            get
            {
                return playPosition;
            }
            set
            {
                if (playPosition != value)
                {
                    playPosition = value;
                    HandlePlayPositionChanged();
                }
            }
        }

        TimeInterval mSelectedTimeInterval = new TimeInterval();
        public TimeInterval SelectedTimeInterval
        {
            get
            {
                return mSelectedTimeInterval;
            }
            set
            {
                mSelectedTimeInterval = value;
            }
        }

        #endregion  //Public properties

        #region Public methods

        public RD_ERROR Play()
        {
            RD_ERROR errorCode;
            errorCode = Program.context.playerSession.PlayRequest();
            if (errorCode == RD_ERROR.RDE_NOERROR)
                PlayMode = PlayMode.MODE_PLAY;
            return errorCode;
        }

        public RD_ERROR PlayInterval(TimeInterval aInterval)
        {
            RD_ERROR errorCode = RD_ERROR.RDE_NOERROR;
            return errorCode;
        }

        public RD_ERROR Rewind()
        {
            RD_ERROR errorCode;
            errorCode = Program.context.playerSession.SeekRequest(0);
            PlayMode = PlayMode.MODE_PAUSE;
            PlayPosition = 0;
            return errorCode;
        }

        public RD_ERROR Pause()
        {
            RD_ERROR errorCode;
            errorCode = Program.context.playerSession.PauseRequest();
            PlayMode = PlayMode.MODE_PAUSE;
            return errorCode;
        }

        public RD_ERROR CutSelection()
        {
            return Program.context.playerSession.CutSelection( mSelectedTimeInterval);
        }

        public RD_ERROR CopySelection()
        {
            //??
            return RD_ERROR.RDE_NOT_IMPLEMENTED;
        }

        public RD_ERROR DeleteSelection()
        {
            return Program.context.playerSession.DeleteSelection(mSelectedTimeInterval);
        }

        public RD_ERROR PasteSelection()
        {
            return Program.context.playerSession.PasteSelection( 0, mSelectedTimeInterval);
        }

        #endregion  //Public methods

        #region  Private methods

        void HandlePlayStateChanged()
        {
            OnPlayStateChangedDelegate pfn = Fire_OnPlayModeChanged;
            Program.context.recordEditor.Invoke(pfn);
        }

        void Fire_OnPlayModeChanged()
        {
            if (OnPlayStateChanged != null)
                OnPlayStateChanged(this, new EventArgs());
            if (playMode == PlayMode.MODE_PLAY)
            {
                timerPlay.Enabled = true;
            }
            else
            {
                timerPlay.Enabled = false;
            }
        }

        void HandlePlayPositionChanged()
        {
            OnPlayPositionChangedDelegate pfn = Fire_OnPlayPositionChanged;
            Program.context.recordEditor.Invoke(pfn);
        }

        void Fire_OnPlayPositionChanged()
        {
            if (OnPlayPositionChanged != null)
                OnPlayPositionChanged(this, new EventArgs());
        }

        #endregion  //Private methods
    }
}
