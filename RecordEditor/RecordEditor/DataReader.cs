using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RecordEditor
{
 /*   public class DataReader
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
    }*/
    public class DataReader
    {
        protected byte[] data;
        protected int pos;
        protected int begin;
        protected int end;
        protected bool isReadMode = true;

        #region Инициализация

        public DataReader()
        {
            AttachData(new byte[] { });
        }

        public DataReader(int aDataSize)
        {
            byte[] data = new byte[aDataSize];
            AttachData(data);
        }

        public byte [] GetData()
        {
            return data;
        }

        public DataReader(byte[] aData)
        {
            AttachData(aData, 0, aData.Length);
        }

        public DataReader(byte[] aData, int aDataOffset, int aDataSize)
        {
            AttachData(aData, aDataOffset, aDataSize);
        }

        public void AttachData(byte[] aData)
        {
            AttachData(aData, 0, aData.Length);
        }

        public void AttachData(byte[] aData, int aDataOffset, int aDataSize)
        {
            data = aData;
            begin = Math.Max(0, aDataOffset);
            end = begin + Math.Max(0, aDataSize);
            Reset();
        }
        #endregion

        #region Позиционирование

        public void Reset()
        {
            pos = begin;
            isReadMode = true;
        }

        public void Seek(int aPos)
        {
            pos = Math.Max(begin, Math.Min(end, aPos));
        }

        public int GetPosition()
        {
            return pos;
        }

        public int GetDataSize()
        {
            return end - begin;
        }

        public bool EndOfData()
        {
            return pos >= end;
        }
        #endregion

        #region Чтение в заданной позиции

        public void SetReadMode()
        {
            isReadMode = true;
        }

        public void SetTestMode()
        {
            isReadMode = false;
        }

        protected bool MayBeReadAt(int aPos, int aSize)
        {
            if (aPos < begin)
                return false;

            if (aSize < 0)
                return false;

            if (aPos + aSize > end)
                return false;

            return true;
        }

        public bool ReadAt(int aPos, ref byte aValue)
        {
            if (!MayBeReadAt(aPos, 1))
                return false;
            if (isReadMode)
                aValue = data[aPos];
            return true;
        }

        public bool ReadAt(int aPos, ref bool aValue)
        {
            if (!MayBeReadAt(aPos, 1))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToBoolean(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref short aValue)
        {
            if (!MayBeReadAt(aPos, 2))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToInt16(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref ushort aValue)
        {
            if (!MayBeReadAt(aPos, 2))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToUInt16(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref int aValue)
        {
            if (!MayBeReadAt(aPos, 4))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToInt32(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref uint aValue)
        {
            if (!MayBeReadAt(aPos, 4))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToUInt32(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref long aValue)
        {
            if (!MayBeReadAt(aPos, 8))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToInt64(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref ulong aValue)
        {
            if (!MayBeReadAt(aPos, 8))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToUInt64(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref float aValue)
        {
            if (!MayBeReadAt(aPos, 4))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToSingle(data, aPos);
            return true;
        }

        public bool ReadAt(int aPos, ref double aValue)
        {
            if (!MayBeReadAt(aPos, 8))
                return false;
            if (isReadMode)
                aValue = BitConverter.ToDouble(data, aPos);
            return true;
        }

        public bool ReadAtUnicode(int aPos, int aSize, ref string aValue)
        {
            if (!MayBeReadAt(aPos, aSize * 2))
                return false;
            if (isReadMode)
                aValue = Encoding.Unicode.GetString(data, aPos, aSize * 2);
            return true;
        }

        protected bool ReadAtLength(int aPos, ref ushort aValue)
        {
            bool savedReadMode = isReadMode;
            isReadMode = true;
            if (!ReadAt(aPos, ref aValue))
                return false;
            isReadMode = savedReadMode;

            return true;
        }

        protected bool ReadAtLength(int aPos, ref int aValue)
        {
            bool savedReadMode = isReadMode;
            isReadMode = true;
            if (!ReadAt(aPos, ref aValue))
                return false;
            isReadMode = savedReadMode;

            return true;
        }

        protected bool ReadAtUnicode(int aPos, ref string aValue, ref ushort aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;
            return ReadAtUnicode(aPos + 2, aLength, ref aValue);
        }

        public bool ReadAtUnicode(int aPos, ref string aValue)
        {
            ushort len = 0;
            return ReadAtUnicode(aPos, ref aValue, ref len);
        }

        protected bool ReadAtUnicodeLange(int aPos, ref string aValue, ref int aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;
            return ReadAtUnicode(aPos + 4, aLength, ref aValue);
        }

        public bool ReadAtUnicodeLange(int aPos, ref string aValue)
        {
            int len = 0;
            return ReadAtUnicodeLange(aPos, ref aValue, ref len);
        }

        public bool ReadAtASCII(int aPos, int aSize, ref string aValue)
        {
            if (!MayBeReadAt(aPos, aSize))
                return false;
            if (isReadMode)
                aValue = Encoding.ASCII.GetString(data, aPos, aSize);
            return true;
        }

        protected bool ReadAtASCII(int aPos, ref string aValue, ref ushort aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;

            return ReadAtASCII(aPos + 2, aLength, ref aValue);
        }

        public bool ReadAtASCII(int aPos, ref string aValue)
        {
            ushort len = 0;
            return ReadAtASCII(aPos, ref aValue, ref len);
        }

        protected bool ReadAtASCIILange(int aPos, ref string aValue, ref int aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;
            return ReadAtASCII(aPos + 4, aLength, ref aValue);
        }

        public bool ReadAtASCIILange(int aPos, ref string aValue)
        {
            int len = 0;
            return ReadAtASCIILange(aPos, ref aValue, ref len);
        }

        public bool ReadAt(int aPos, int aSize, ref byte[] aValue)
        {
            if (!MayBeReadAt(aPos, aSize))
                return false;
            if (isReadMode)
            {
                aValue = new byte[aSize];
                Array.Copy(data, aPos, aValue, 0, aSize);
            }
            return true;
        }

        public bool ReadAtArray(int aPos, int aSize, ref byte[] aValue)
        {
            return ReadAt(aPos, aSize, ref aValue);
        }

        public bool ReadAtArray(int aPos, ref byte[] aValue, ref ushort aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;

            return ReadAt(aPos + 2, aLength, ref aValue);
        }

        public bool ReadAtArray(int aPos, ref byte[] aValue)
        {
            ushort len = 0;
            return ReadAtArray(aPos, ref aValue, ref len);
        }

        public bool ReadAtArray(int aPos, DataBuffer aValue, ref ushort aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;

            if (!MayBeReadAt(aPos + 2, aLength))
                return false;

            if (isReadMode)
                aValue.AddArray(data, (uint)aPos + 2, aLength);
            return true;
        }

        public bool ReadAtArray(int aPos, DataBuffer aValue)
        {
            ushort len = 0;
            return ReadAtArray(aPos, aValue, ref len);
        }

        public bool ReadAtArrayLange(int aPos, ref byte[] aValue, ref int aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;

            return ReadAt(aPos + 4, aLength, ref aValue);
        }

        public bool ReadAtArrayLange(int aPos, ref byte[] aValue)
        {
            int len = 0;
            return ReadAtArrayLange(aPos, ref aValue, ref len);
        }

        public bool ReadAtArrayLange(int aPos, DataBuffer aValue, ref int aLength)
        {
            if (!ReadAtLength(aPos, ref aLength))
                return false;
            aPos += 4;

            if (!MayBeReadAt(aPos, aLength))
                return false;

            if (isReadMode)
                aValue.AddLargeArray(data, (uint)aPos, (uint)aLength);
            return true;
        }

        public bool ReadAtArrayLange(int aPos, DataBuffer aValue)
        {
            int len = 0;
            return ReadAtArrayLange(aPos, aValue, ref len);
        }

        public virtual bool ReadAtSubreader(int aPos, ref int aLength, ref DataReader aClone)
        {
            if (aClone == null)
                return false;
            if (!ReadAtLength(aPos, ref aLength))
                return false;
            aPos += 4;
            return ReadAtSubreader(aPos, aLength, ref aClone);
        }

        public virtual bool ReadAtSubreader(int aPos, int aDataSize, ref DataReader aClone)
        {
            if (aClone == null)
                return false;

            if (!MayBeReadAt(aPos, aDataSize))
                return false;
            aClone.AttachData(data, aPos, aDataSize);
            return true;
        }
        #endregion

        #region Чтение

        public bool Read(ref byte aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 1;
            return true;
        }

        public bool Read(ref bool aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 1;
            return true;
        }

        public bool Read(ref short aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 2;
            return true;
        }

        public bool Read(ref ushort aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 2;
            return true;
        }

        public bool Read(ref int aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 4;
            return true;
        }

        public bool Read(ref uint aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 4;
            return true;
        }

        public bool Read(ref long aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 8;
            return true;
        }

        public bool Read(ref ulong aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 8;
            return true;
        }

        public bool Read(ref float aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 4;
            return true;
        }

        public bool Read(ref double aValue)
        {
            if (!ReadAt(pos, ref aValue))
                return false;
            pos += 8;
            return true;
        }

        public bool ReadUnicode(int aSize, ref string aValue)
        {
            if (!ReadAtUnicode(pos, aSize, ref aValue))
                return false;
            pos += aSize * 2;
            return true;
        }

        public bool ReadUnicode(ref string aValue)
        {
            ushort len = 0;
            if (!ReadAtUnicode(pos, ref aValue, ref len) || len < 0)
                return false;
            pos += 2 + len * 2;
            return true;
        }

        public bool ReadUnicodeLange(ref string aValue)
        {
            int len = 0;
            if (!ReadAtUnicodeLange(pos, ref aValue, ref len) || len < 0)
                return false;
            pos += 4 + len * 2;
            return true;
        }

        public bool ReadASCII(int aSize, ref string aValue)
        {
            if (!ReadAtASCII(pos, aSize, ref aValue))
                return false;
            pos += aSize;
            return true;
        }

        public bool ReadASCII(ref string aValue)
        {
            ushort len = 0;
            if (!ReadAtASCII(pos, ref aValue, ref len))
                return false;
            pos += 2 + len;
            return true;
        }

        public bool ReadASCIILange(ref string aValue)
        {
            int len = 0;
            if (!ReadAtASCIILange(pos, ref aValue, ref len) || len < 0)
                return false;
            pos += 4 + len;
            return true;
        }

        public bool Read(int aSize, ref byte[] aValue)
        {
            if (!ReadAt(pos, aSize, ref aValue))
                return false;
            pos += aSize;
            return true;
        }

        public bool ReadArray(int aSize, ref byte[] aValue)
        {
            return Read(aSize, ref aValue);
        }

        public bool ReadArray(ref byte[] aValue)
        {
            ushort len = 0;
            if (!ReadAtArray(pos, ref aValue, ref len))
                return false;
            pos += 2 + len;
            return true;
        }

        public bool ReadArray(DataBuffer aDataBuffer)
        {
            ushort len = 0;
            if (!ReadAtArray(pos, aDataBuffer, ref len))
                return false;
            pos += 2 + len;
            return true;
        }

        public bool ReadArrayLange(ref byte[] aValue)
        {
            int len = 0;
            if (!ReadAtArrayLange(pos, ref aValue, ref len))
                return false;
            pos += 4 + len;
            return true;
        }

        public virtual bool ReadSubreader(ref DataReader aClone)
        {
            int len = 0;
            if (!ReadAtSubreader(pos, ref len, ref aClone))
                return false;
            pos += 4 + len;
            return true;
        }

        public virtual bool ReadSubreader(int aDataSize, ref DataReader aClone)
        {
            if (!ReadAtSubreader(pos, aDataSize, ref aClone))
                return false;
            pos += 4 + aDataSize;
            return true;
        }
        #endregion
    }
}
