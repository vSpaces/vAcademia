using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    public delegate void UndoAllowedEventHandler(Object sender, HistoryAllowedEventArgs args);
    public delegate void RedoAllowedEventHandler(Object sender, HistoryAllowedEventArgs args);

    public class HistoryAllowedEventArgs
    {
        private bool allowedState;

        public HistoryAllowedEventArgs(bool _allowedState)
        {
            allowedState = _allowedState;
        }

        public bool AllowedState
        {
            get { return (allowedState); }
        }
    };

    partial class HistoryStorage
    {
        public event UndoAllowedEventHandler UndoAllowed; // событие изменения состояния команды "отмена"
        public event RedoAllowedEventHandler RedoAllowed; // событие изменения состояния команды "повтор"

        protected virtual void OnUndoAllowed(HistoryAllowedEventArgs args)
        {
            if (UndoAllowed != null)
            {
                UndoAllowed(this, args);
            }
        }

        protected virtual void OnRedoAllowed(HistoryAllowedEventArgs args)
        {
            if (RedoAllowed != null)
            {
                RedoAllowed(this, args);
            }
        } 
    }
}
