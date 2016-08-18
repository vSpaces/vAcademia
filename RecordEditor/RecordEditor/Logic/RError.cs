using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RecordEditor.Logic
{
    public enum RD_ERROR
    {
        RDE_NOERROR = 0,                    // нет ощибки
        RDE_INCORRECT_PARAMETERS = 1,        // не верные параметры
        RDE_NOT_IMPLEMENTED = 2,        // не верные параметры
        RDE_NOT_CONNECTED = 3,              // не подключен к плееру
        RDE_COMMAND_OBSOLETE = 4           // не подключен к плееру
    }
}
