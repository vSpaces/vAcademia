using System;
using System.Linq;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace RecordEditor
{
    class DataWriter
    {
        private ArrayList data;

        public DataWriter()
        {
            data = new ArrayList();
        }

        public void AddData(byte aValue)
        {
            data.Add(aValue);
        }

        public void AddData(short aValue)
        {
            data.AddRange(BitConverter.GetBytes(aValue));
        }

        public void AddData(int aValue)
        {
            data.AddRange(BitConverter.GetBytes(aValue));
        }

        public void AddData(long aValue)
        {
            data.AddRange(BitConverter.GetBytes(aValue));
        }

        public void AddData(uint aValue)
        {
            data.AddRange(BitConverter.GetBytes(aValue));
        }

        public void AddData(float aValue)
        {
            data.AddRange(BitConverter.GetBytes(aValue));
        }

        public void AddData(double aValue)
        {
            data.AddRange(BitConverter.GetBytes(aValue));
        }

        public void AddData(String aValue)
        {
            short aLen = (short)aValue.Length;
            AddData(aLen);
            data.AddRange(Encoding.UTF8.GetBytes(aValue));
        }

        public void AddData(byte [] aValue, int dataSize)
        {
            data.AddRange(aValue);
        }

        public byte[] GetData()
        {
            byte[] dataBytes = new byte[data.Count];
            data.CopyTo(dataBytes);

            return dataBytes;
        }

        public int GetSize()
        {
            return data.Count;
        }
    }
}
