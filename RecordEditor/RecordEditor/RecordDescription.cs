using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RecordEditor
{
    public class RecordDescription
    {
        // ID записи
        public int recordID = 0;
        // реальность
        public int realityID = 0;
        // продолжительность
        public uint duration = 0;
        // название записи
        public string name = "";
        // автор
        public string author = "";
        // время создания
        public string creationTime = "";
        // локация
        public string location = "";
        // занятие
        public string lesson = "";
        // описание
        public string description = "";
        // URL
        public string URL = "";

        public RecordDescription()
        {
        }

        public void Set( RecordDescription apDescription)
        {
            if (apDescription == null)
                return;
            // ID записи
            recordID = apDescription.recordID;
            // реальность
            realityID = apDescription.realityID;
            // продолжительность
            duration = apDescription.duration;
            // название записи
            name = apDescription.name;
            // автор
            author = apDescription.author;
            // время создания
            creationTime = apDescription.creationTime;
            // локация
            location = apDescription.location;
            // занятие
            lesson = apDescription.lesson;
            // описание
            description = apDescription.description;
            // URL
            URL = apDescription.URL;
        }
    }
}
