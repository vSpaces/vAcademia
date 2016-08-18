using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RecordEditor.Logic
{
    class Context
    {
        public RecordEditor      recordEditor = null;
        public RecordController  recordController = null;
        public PlayerSession     playerSession = null;
        public MessageHandlerManager messageHandlerManager = null;
    }
}
