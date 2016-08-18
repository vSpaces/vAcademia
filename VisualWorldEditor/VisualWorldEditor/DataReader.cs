using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    public class DataReader
    {
        private byte[] data;
        public int pos;
        private int begin;
        public int end;

        public DataReader(int dataSize)
        {
            data = new byte[dataSize];
            begin = 0;
            pos = begin;
            end = dataSize - 1;
        }
        
        public void AttachData(byte[] aData)
        {
            data = aData;
            begin = 0;
            pos = begin;
            end = data.Length - 1;
        }
        
        public byte[] GetData()
        {
            return data;
        }

        private bool CanRead(int aPos, int aSize)
        {
            if (aPos < 0)
            {
                return false;
            }
            else if (aPos + aSize - 1 > end)
            {
                return false;
            }

            return true;
        }

        private bool EoData()
        {
            if (pos > end)
            {
                return true;
            }

            return false;
        }

        public void Reset()
        {
            pos = begin;
        }

        public bool Read(ref byte aValue)
        {
            if (!CanRead(pos, 1))
                return false;

            aValue = data[pos];
            pos += 1;

            return true;
        }

        public bool Read(ref short aValue)
        {
            if (!CanRead(pos, 2))
                return false;

            aValue = BitConverter.ToInt16(data, pos);
            pos += 2;

            return true;
        }

        public bool Read(ref int aValue)
        {
            if (!CanRead(pos, 4))
                return false;

            aValue = BitConverter.ToInt32(data, pos);
            pos += 4;

            return true;
        }

        public bool Read(ref long aValue)
        {
            if (!CanRead(pos, 8))
                return false;

            aValue = BitConverter.ToInt64(data, pos);
            pos += 8;

            return true;
        }

        public bool Read(ref float aValue)
        {
            if (!CanRead(pos, 4))
                return false;

            aValue = BitConverter.ToSingle(data, pos);
            pos += 4;

            return true;
        }

        public bool Read(ref double aValue)
        {
            if (!CanRead(pos, 8))
                return false;

            aValue = BitConverter.ToDouble(data, pos);
            pos += 8;

            return true;
        }

        public bool Read(ref String aValue)
        {
            short aValueSize = new short();
            if (!Read(ref aValueSize))
                return false;

            if (!CanRead(pos, aValueSize))
                return false;

            aValue = Encoding.Default.GetString(data, pos, aValueSize);

            pos += aValueSize;

            return true;
        }

        public int GetSize()
        {
            return data.Length;
        }
    }


}
