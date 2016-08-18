using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    public delegate void CommanderWorkStateChangedEventHandler(Object sender, CommanderWorkStateChangedEventArgs args);

    public class CommanderWorkStateChangedEventArgs
    {
        private Commander.WorkStates args;

        public CommanderWorkStateChangedEventArgs(Commander.WorkStates _args)
        {
            args = _args;
        }

        public Commander.WorkStates Args
        {
            get { return (args); }
        }
    };

    public partial class Commander
    {
        public event CommanderWorkStateChangedEventHandler WorkStateChanged;

        protected virtual void OnWorkStateChanged(CommanderWorkStateChangedEventArgs args)
        {
            if (WorkStateChanged != null)
            {
                WorkStateChanged(this, args);
            }
        }

        public enum WorkStates : int
        {
            ClientNotWorking = 0,
            ClientWorking = 1,
            NoSelect = 2,
            OneSelect = 3,
            MoreSelect = 4
        };

        private WorkStates m_workState = WorkStates.ClientNotWorking;

        public WorkStates WorkState
        {
            get
            {
                return (m_workState);
            }
            set
            {
                m_workState = value;
                CommanderWorkStateChangedEventArgs commanderEvargs = new CommanderWorkStateChangedEventArgs(m_workState);
                OnWorkStateChanged(commanderEvargs);
            }
        }
    }
}
