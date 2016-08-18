using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RecordEditor
{
    class Command
    {
        //in command codes 0 ..
        public const byte CMD_ON_UNKNOWN = 0;
        public const byte CMD_ON_IDLE = 1;
        public const byte CMD_ON_HANDSHAKE = 2;
        public const byte CMD_ON_GET_RECORD_LIST = 3;
        public const byte CMD_ON_EXIT = 4;
        public const byte CMD_ON_OPEN = 5;
        public const byte CMD_ON_GET_RECORD_INFO = 6;

        //out command codes 50 .. 
        public const byte CMD_UNKNOWN = 50;
        public const byte CMD_IDLE = 51;
        public const byte CMD_HANDSHAKE = 52;
        public const byte CMD_PLAY = 53;
        public const byte CMD_STOP = 54;
        public const byte CMD_PAUSE = 55;
        public const byte CMD_SET_PLAY_POSITION = 56;
        public const byte CMD_SET_BEGIN_SELECTION = 57;
        public const byte CMD_SET_END_SELECTION = 58;
        public const byte CMD_DELETE_SELECTION = 59;
        public const byte CMD_OPEN = 60;
        public const byte CMD_SAVE = 61;
        public const byte CMD_SAVE_AS = 62;
        public const byte CMD_GET_RECORD_LIST = 63;
        public const byte CMD_PASTE_SELECTION = 64;
        public const byte CMD_GET_RECORD_INFO = 65;
        public const byte CMD_SEEK = 66;
        public const byte CMD_UNDO = 67;
        public const byte CMD_REDO = 68;
    }

}
