using System;
using System.Text;
using System.Collections;

namespace RecordEditor
{
	public class DataBuffer
	{
		#region Инициализация

		protected byte[] buffer;
		protected uint bufferSize;

		public DataBuffer( int aCapacity)
		{
			buffer = new byte[aCapacity];
			bufferSize = 0;
		}

		public DataBuffer() : this( 65536)
		{			
		}

		public uint GetBufferSize()
		{
			return bufferSize;
		}

		public void Clear()
		{
			bufferSize = 0;
		}

		public byte[] GetBufferCopy()
		{
			byte[] bufferCopy = new byte[ bufferSize];
			lock (buffer)
				Array.Copy( buffer, 0, bufferCopy, 0, bufferSize);
			return bufferCopy;
		}

		internal byte[] GetBuffer()
		{
			return buffer;
		}

		protected void Grow( uint aCapacity)
		{
			if (buffer.Length < aCapacity)
			{
				uint newSize = (uint)Math.Max( aCapacity, buffer.Length * 2);
				byte[] newBuffer = new byte[ newSize];
				Array.Copy( buffer, 0, newBuffer, 0, bufferSize);
				buffer = newBuffer;
			}
		}
		#endregion

		#region Добавление в конец

		public void Add( byte[] aData, uint aDataOffset, uint aSize)
		{
			SetAt( bufferSize, aData, aDataOffset, aSize);
		}

		public void Add( byte[] aData, uint aSize)
		{
			SetAt( bufferSize, aData, 0, aSize);
		}

		public void Add( byte[] aData)
		{
			SetAt( bufferSize, aData, 0, (uint)aData.Length);
		}

		public void Add( byte aNumber)
		{
			SetAt( bufferSize, aNumber);
		}

		public void Add( bool aNumber)
		{
			SetAt( bufferSize, aNumber);
		}

		public void Add( short aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( ushort aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( int aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( uint aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( long aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( ulong aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( float aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void Add( double aNumber)
		{
			SetAt( bufferSize, BitConverter.GetBytes( aNumber));
		}

		public void AddUnicode( string str)
		{
			SetAtUnicode( bufferSize, str);
		}

		public void AddLargeUnicode( string str)
		{
			SetAtLargeUnicode( bufferSize, str);
		}

		public void AddASCII( string str)
		{
			SetAtASCII( bufferSize, str);
		}

		public void AddLargeASCII( string str)
		{
			SetAtLargeASCII( bufferSize, str);
		}

		public void AddArray( byte[] aData, uint aDataOffset, ushort aSize)
		{
			SetAtArray( bufferSize, aData, aDataOffset, aSize);
		}

		public void AddArray( byte[] aData)
		{
			SetAtArray( bufferSize, aData, 0, (ushort)aData.Length);
		}

		public void AddLargeArray( byte[] aData)
		{
			SetAtArray( bufferSize, aData, 0, (uint)aData.Length);
		}

		public void AddLargeArray( byte[] aData, uint aDataOffset, uint aSize)
		{
			SetAtArray( bufferSize, aData, aDataOffset, aSize);
		}

		public void AddData( DataBuffer aData)
		{
			SetAtArray( bufferSize, aData.GetBuffer(), 0, aData.GetBufferSize());
		}

		public void Truncate( uint anOffset)
		{
			bufferSize = Math.Min( bufferSize, anOffset);
		}
		#endregion

		#region Вставка по заданному смещению

		public void SetAt( uint anOffset, byte[] aData, uint aDataOffset, uint aSize)
		{
			lock (buffer)
			{
				Grow( anOffset + aSize);
				Array.Copy( aData, aDataOffset, buffer, anOffset, aSize);
				bufferSize = Math.Max( bufferSize, anOffset + aSize);
			}
		}

		public void SetAt( uint anOffset, byte[] aData, uint aSize)
		{
			SetAt( anOffset, aData, 0, aSize);
		}

		public void SetAt( uint anOffset, byte[] aData)
		{
			SetAt( anOffset, aData, 0, (uint)aData.Length);
		}

		public void SetAt( uint anOffset, byte aNumber)
		{
			lock (buffer)
			{
				Grow( anOffset + 1);
				buffer[ anOffset] = aNumber;
				bufferSize = Math.Max( bufferSize, anOffset + 1);
			}
		}

		public void SetAt( uint anOffset, bool aNumber)
		{
				SetAt( anOffset, BitConverter.GetBytes(aNumber)) ;
		}

		public void SetAt( uint anOffset, short aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, ushort aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, int aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, uint aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, long aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, ulong aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, float aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAt( uint anOffset, double aNumber)
		{
			SetAt( anOffset, BitConverter.GetBytes( aNumber));
		}

		public void SetAtUnicode( uint anOffset, string str)
		{
			lock (buffer)
			{
				ushort len = (ushort)str.Length;
				SetAt( anOffset, BitConverter.GetBytes( len));
				if (str.Length == 0)
					return;
				SetAt( anOffset + 2, Encoding.Unicode.GetBytes( str));
			}
		}

		public void SetAtLargeUnicode( uint anOffset, string str)
		{
			lock (buffer)
			{
				SetAt( anOffset, BitConverter.GetBytes( str.Length));
				if (str.Length == 0)
					return;
				SetAt( anOffset + 2, Encoding.Unicode.GetBytes( str));
			}
		}

		public void SetAtASCII( uint anOffset, string str)
		{
			lock (buffer)
			{
				ushort len = (ushort)str.Length;
				SetAt( anOffset, BitConverter.GetBytes( len));
				if (str.Length == 0)
					return;
				SetAt( anOffset + 2, Encoding.Default.GetBytes( str));
			}
		}

		public void SetAtLargeASCII( uint anOffset, string str)
		{
			lock (buffer)
			{
				SetAt( anOffset, BitConverter.GetBytes( str.Length));
				if (str.Length == 0)
					return;
				SetAt( anOffset + 4, Encoding.Default.GetBytes( str));
			}
		}

		public void SetAtArray( uint anOffset, byte[] aData, uint aDataOffset, ushort aSize)
		{
			lock (buffer)
			{
				SetAt( anOffset, aSize);
				if (aSize == 0)
					return;
				SetAt( anOffset + 2, aData, aDataOffset, aSize);
			}
		}

		public void SetAtArray( uint anOffset, byte[] aData)
		{
			SetAtArray( anOffset, aData, 0, (ushort)aData.Length);
		}

		public void SetAtArray( uint anOffset, byte[] aData, uint aDataOffset, uint aSize)
		{
			lock (buffer)
			{
				SetAt( anOffset, aSize);
				if (aSize == 0)
					return;
				SetAt( anOffset + 4, aData, aDataOffset, aSize);
			}
		}
		#endregion

	}
}
