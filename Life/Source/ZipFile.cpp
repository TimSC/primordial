// ZipFile.cpp
//
// Original Author: Kevin English
//
// Declaration of CCompressFile, CUncompressFile, and their base class CZBaseFile.
//
// These classes provide CFile wrappers around the public domain ZLib library
//
// These classes are heavily based on the functions found in the gzio.c
// module of the ZLib Library.
//
// These classes currently maintain a CRC value, but do not do anything with it
// and do not provide any member functions for accessing it.
//
// This CFile implementation is for streaming only.  Random access methods
// are unimplemented.
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ZipFile.h"
#include "zlib.h"

//#define _TRACE

IMPLEMENT_DYNAMIC(CZBaseFile, CFile)

CZBaseFile::CZBaseFile(int hFile)
:	CFile(hFile)
{
	m_stream.zalloc = (alloc_func)0;
	m_stream.zfree = (free_func)0;
	m_stream.opaque = (voidpf)0;
	m_stream.next_in = Z_NULL;
	m_stream.next_out = Z_NULL;
	m_stream.avail_in = m_stream.avail_out = 0;
	m_z_err = Z_OK;
	m_z_eof = 0;
	m_crc = crc32(0L, Z_NULL, 0);
	m_position = 0;

	m_mode = mode_none;
}

CZBaseFile::CZBaseFile(LPCTSTR lpszFileName, UINT nOpenFlags)
: CFile(lpszFileName, nOpenFlags)
{
	m_stream.zalloc = (alloc_func)0;
	m_stream.zfree = (free_func)0;
	m_stream.opaque = (voidpf)0;
	m_stream.next_in = Z_NULL;
	m_stream.next_out = Z_NULL;
	m_stream.avail_in = m_stream.avail_out = 0;
	m_z_err = Z_OK;
	m_z_eof = 0;
	m_crc = crc32(0L, Z_NULL, 0);
	m_position = 0;

	m_mode = mode_none;
}

CZBaseFile::~CZBaseFile()
{
	Close();
}

DWORD CZBaseFile::GetPosition() const
{
	return m_position;
}

BOOL CZBaseFile::GetStatus(CFileStatus& /*rStatus*/)
{
	return FALSE;
};
CString CZBaseFile::GetFileName() const
{
	CString result; 
	result.Format(_T("Compressing %s"), (LPCSTR)(CFile::GetFileName()));
	return result;
}
CString CZBaseFile::GetFileTitle() const
{
	CString result; 
	result.Format(_T("Compressing %s"), (LPCSTR)(CFile::GetFileTitle()));
	return result;
}
CString CZBaseFile::GetFilePath() const
{
//	ASSERT(FALSE);
	CString result;
	return result;
}
void CZBaseFile::SetFilePath(LPCTSTR /*lpszNewName*/)
{
	ASSERT(FALSE);
}

BOOL CZBaseFile::Open(LPCTSTR /*lpszFileName*/, UINT /*nOpenFlags*/, CFileException* /*pError*/)
{
	ASSERT(FALSE);
	return FALSE;
}

DWORD CZBaseFile::SeekToEnd()
{
	ASSERT(FALSE);
	return 0xFFFFFFFF;
}
void CZBaseFile::SeekToBegin()
{
	ASSERT(FALSE);
}

DWORD CZBaseFile::ReadHuge(void* lpBuffer, DWORD dwCount)
{
	return Read(lpBuffer, dwCount);
}
void CZBaseFile::WriteHuge(const void* lpBuffer, DWORD dwCount)
{
	Write(lpBuffer, dwCount);
}

LONG CZBaseFile::Seek(LONG /*lOff*/, UINT /*nFrom*/)
{
//	ASSERT(FALSE);
	return 0xFFFFFFFF;
}
void CZBaseFile::SetLength(DWORD /*dwNewLen*/)
{
	ASSERT(FALSE);
}
DWORD CZBaseFile::GetLength() const
{
	ASSERT(FALSE);
	return 0xFFFFFFFF;
}

UINT CZBaseFile::Read(void* /*lpBuf*/, UINT /*nCount*/)
{
	ASSERT(FALSE);
	return 0;
}

void CZBaseFile::Write(const void* /*lpBuf*/, UINT /*nCount*/)
{
	ASSERT(FALSE);
}

void CZBaseFile::LockRange(DWORD /*dwPos*/, DWORD /*dwCount*/)
{
	ASSERT(FALSE);
}
void CZBaseFile::UnlockRange(DWORD /*dwPos*/, DWORD /*dwCount*/)
{
	ASSERT(FALSE);
}

void CZBaseFile::Abort()
{
	ASSERT(FALSE);
}
void CZBaseFile::Flush()
{
}

void CZBaseFile::Close()
{
}

#ifdef _DEBUG
	void CZBaseFile::AssertValid() const
	{
	}
	void CZBaseFile::Dump(CDumpContext& /*dc*/) const
	{
	}
#endif

UINT CZBaseFile::GetBufferPtr(UINT /*nCommand*/, UINT /*nCount*/,
	void** /*ppBufStart*/, void** /*ppBufMax*/)
{
	// CArchive calls here to see if it needs to allocate its
	// own buffer or if it can use the CFile's directly.  The 
	// zero return works fine with CArchive which then allocates
	// its own buffer.  There may be a class out there that needs
	// this support but I doubt it.
	TRACE("Warning: GetBufferPtr called\n");
//	ASSERT(FALSE);
	return 0;
}

void CZBaseFile::WrapWrite(CFile* file, const void *p, UINT n)
{
	SetTestMode(mode_write);

	try
	{
		file->CFile::Write(p, n);
	}
	catch(CFileException*)
	{
		ASSERT(FALSE);
		m_z_err = Z_ERRNO;
		throw;
	}
	catch(CException*)
	{
		ASSERT(FALSE);
		m_z_err = Z_ERRNO;
		throw;
	}
	catch(...)
	{
		ASSERT(FALSE);
		m_z_err = Z_ERRNO;
		throw;
	}
}

UINT CZBaseFile::WrapRead(CFile* file, void *p, UINT n)
{
	SetTestMode(mode_read);

	UINT result = 0;
	try
	{
		result = file->CFile::Read(p, n);
		if (result == 0)
		{
			m_z_eof = 1;
		}
	}
	catch(CFileException*)
	{
		ASSERT(FALSE);
		m_z_err = Z_ERRNO;
		throw;
	}
	catch(CException*)
	{
		ASSERT(FALSE);
		m_z_err = Z_ERRNO;
		throw;
	}
	catch(...)
	{
		ASSERT(FALSE);
		m_z_err = Z_ERRNO;
		throw;
	}
	return result;
}

int CZBaseFile::get_byte()
{
    if (m_z_eof) return EOF;
    if (m_stream.avail_in == 0) 
	{
		m_stream.avail_in = WrapRead(this, m_buf, Z_BUFSIZE);
		if (m_stream.avail_in == 0) 
		{
		    m_z_eof = 1;
			return EOF;
		}
		m_stream.next_in = m_buf;
    }
    m_stream.avail_in--;
    return *(m_stream.next_in)++;
}

ULONG CZBaseFile::getLong ()
{
    ULONG x = (ULONG)get_byte();
    int c;

    x += ((ULONG)get_byte())<<8;
    x += ((ULONG)get_byte())<<16;
    c = get_byte();
    if (c == EOF) m_z_err = Z_DATA_ERROR;
    x += ((ULONG)c)<<24;
    return x;
}


//=============================================================================
// CCompressFile
//
// This class compresses a file.  If the Read method is to be called, then
// the CFile passed in to the constructor should be an uncompressed file that
// will be compressed during a series of calls to the Read method, if the 
// Write method is to be called, the the CFile passed into the constructor
// should be the destination file for the compressed data.
//
//=============================================================================

IMPLEMENT_DYNAMIC(CCompressFile, CZBaseFile)

CCompressFile::CCompressFile(int hFile, int level)
	: CZBaseFile(hFile)
{
    int err = deflateInit2(&(m_stream), level,
                       Z_DEFLATED, -MAX_WBITS, 9, 0);
    /* windowBits is passed < 0 to suppress zlib header */

    if (err != Z_OK) 
	{
        ASSERT(FALSE);
		AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile - != Z_OK"));
    }
}


CCompressFile::CCompressFile(LPCTSTR lpszFileName, UINT nOpenFlags, int level)
: CZBaseFile(lpszFileName, nOpenFlags)
{
    int err = deflateInit2(&(m_stream), level,
                       Z_DEFLATED, -MAX_WBITS, 9, 0);
    /* windowBits is passed < 0 to suppress zlib header */

    if (err != Z_OK) 
	{
        ASSERT(FALSE);
		AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile - != Z_OK"));
    }
}


CCompressFile::~CCompressFile()
{
	if (m_stream.state != NULL)
	{
		Close();
	}
}

UINT CCompressFile::EndRead(void* lpBuf, UINT nCount)
{
	UINT nReceive = 0;

	if (m_z_err == Z_DATA_ERROR || m_z_err == Z_ERRNO)
	{
		ASSERT(FALSE);
		AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile::EndRead - Z_DATA_ERROR || Z_ERRNO"));
	}

	m_stream.next_out = (BYTE*)lpBuf;
	m_stream.avail_out = nCount;

	while (m_stream.avail_out != 0)
	{
		m_z_err = deflate(&(m_stream), Z_FINISH);
		if (m_z_err == Z_STREAM_END)
		{
			break;
		}

		if (m_z_err != Z_OK)
		{
			ASSERT(FALSE);
			AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile::EndRead != Z_OK"));
		}
	}
	nReceive = nCount - m_stream.avail_out;

	return nReceive;
}

UINT CCompressFile::Read(void* lpBuf, UINT nCount)
{
	UINT nReceive = 0;

	if (m_z_err == Z_DATA_ERROR || m_z_err == Z_ERRNO)
	{
		ASSERT(FALSE);
		AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile::Read - Z_DATA_ERROR || Z_ERRNO"));
	}

	if (m_z_eof)
	{
		nReceive = EndRead(lpBuf, nCount);
	}
	else
	{
		m_stream.next_out = (BYTE*)lpBuf;
		m_stream.avail_out = nCount;

		while (m_stream.avail_out != 0 && !m_z_eof)
		{
			if (m_stream.avail_in == 0)
			{
				m_size = WrapRead(this, m_buf, sizeof(m_buf));
				m_stream.next_in = m_buf;
				m_stream.avail_in = m_size;
			}
			
			if (m_stream.avail_in > 0)
			{
				m_z_err = deflate(&(m_stream), Z_NO_FLUSH);
				if (m_z_err != Z_OK)
				{
					ASSERT(FALSE);
					AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile::Read != Z_OK"));
				}
			}
		}
		nReceive = nCount - m_stream.avail_out;

		if (nReceive < nCount)
		{
			if (m_z_eof == 1)
			{
				UINT nEndRead = EndRead((((BYTE*)lpBuf)+nReceive), nCount - nReceive);
				nReceive += nEndRead;
			}
		}
	}

	m_crc = crc32(m_crc, (BYTE*)lpBuf, nReceive);

	m_position += nReceive;
	return nReceive;
}

void CCompressFile::Write(const void* lpBuf, UINT nCount)
{
    m_stream.next_in = (BYTE*)lpBuf;
    m_stream.avail_in = nCount;

	if (m_stream.next_out == NULL)
	{	// This is our first write
		m_stream.next_out = m_buf;
		m_stream.avail_out = Z_BUFSIZE;
	}

    while (m_stream.avail_in != 0) 
	{
        if (m_stream.avail_out == 0) 
		{
            m_stream.next_out = m_buf;
            WrapWrite(this, m_buf, Z_BUFSIZE);
            m_stream.avail_out = Z_BUFSIZE;
        }
		if (m_stream.avail_in > 0)
		{
			m_z_err = deflate(&(m_stream), Z_NO_FLUSH);
			if (m_z_err != Z_OK)
			{
				ASSERT(FALSE);
				AfxThrowFileException(CFileException::generic, -1, _T("CCompressFile::Write != Z_OK"));
			}
		}
    }
    m_crc = crc32(m_crc, (BYTE*)lpBuf, nCount);

	ASSERT(0 == m_stream.avail_in);
	m_position += nCount;
}

void CCompressFile::Flush()
{
	if (m_stream.state == NULL)
	{
		// This sucka a'ready clozed
		return;
	}

	uInt len;
	int done = 0;

	m_stream.avail_in = 0; /* should be zero already anyway */

	for (;;) 
	{
		len = Z_BUFSIZE - m_stream.avail_out;

		if (len != 0) 
		{
			WrapWrite(this, m_buf, len);
			m_stream.next_out = m_buf;
			m_stream.avail_out = Z_BUFSIZE;
		}
		if (done) break;
		m_z_err = deflate(&(m_stream), Z_FINISH);

		/* deflate has finished flushing only when it hasn't used up
		 * all the available space in the output buffer: 
		 */
		done = (m_stream.avail_out != 0 || m_z_err == Z_STREAM_END);
	}
	CFile::Flush();

	ASSERT(m_z_err == Z_STREAM_END);
}

void CCompressFile::Close()
{
	if (m_stream.state == NULL)
	{
		// This sucka a'ready clozed
		return;
	}

	int err = deflateEnd(&(m_stream));
	ASSERT(err == Z_OK);
}

//=============================================================================
// CUncompressFile
//
// This class uncompresses a file.  If the Read method is to be called, then
// the CFile passed in to the constructor should be a compressed file that
// will be decompressed during a series of calls to the Read method, if the 
// Write method is to be called, then the CFile passed into the constructor
// should be the destination file for the uncompressed data.
//
//=============================================================================

IMPLEMENT_DYNAMIC(CUncompressFile, CZBaseFile)

// Constructors
CUncompressFile::CUncompressFile(int hFile)
	: CZBaseFile(hFile)
{
	#ifdef _TRACE
		char szFile[80], szPath[256], *szIni = "I:\\tmp\\ZipFile.INI";
		GetPrivateProfileString("ZipFile", "TraceFile", "A.LOG", szFile, sizeof(szFile), szIni);
		wsprintf(szPath, "I:\\tmp\\%s", szFile);
		szFile[0]++;
		WritePrivateProfileString("ZipFile", "TraceFile", szFile, szIni);
		m_TraceFile.Open(szPath, CFile::modeCreate|CFile::modeWrite);
	#endif

    int err = inflateInit2(&(m_stream), -MAX_WBITS);

    if (err != Z_OK)
	{
		AfxThrowFileException(CFileException::generic, -1, _T("CUncompressFile != Z_OK"));
    }
}

CUncompressFile::CUncompressFile(LPCTSTR lpszFileName, UINT nOpenFlags)
: CZBaseFile(lpszFileName, nOpenFlags)
{
	#ifdef _TRACE
		char szFile[80], szPath[256], *szIni = "I:\\tmp\\ZipFile.INI";
		GetPrivateProfileString("ZipFile", "TraceFile", "A.LOG", szFile, sizeof(szFile), szIni);
		wsprintf(szPath, "I:\\tmp\\%s", szFile);
		szFile[0]++;
		WritePrivateProfileString("ZipFile", "TraceFile", szFile, szIni);
		m_TraceFile.Open(szPath, CFile::modeCreate|CFile::modeWrite);
	#endif

    int err = inflateInit2(&(m_stream), -MAX_WBITS);

    if (err != Z_OK)
	{
		AfxThrowFileException(CFileException::generic, -1, _T("CUncompressFile != Z_OK"));
    }
}

CUncompressFile::~CUncompressFile()
{
	if (m_stream.state != NULL)
	{
		Close();
	}
}


UINT CUncompressFile::Read(void* lpBuf, UINT nCount)
{
	#ifdef _TRACE
		m_TraceFile.Write("READ\n", 5);
		m_TraceFile.Write(&nCount, 4);
		m_TraceFile.Write("\n", 1);
	#endif

	if (0 == nCount)
	{
		return 0;
	}

	if (m_stream.state == NULL)
	{
		// This sucka a'ready clozed
		ASSERT(FALSE);
		return 0;
	}

	BYTE *start = (BYTE*)lpBuf; /* starting point for crc computation */

	if (m_z_err == Z_DATA_ERROR || m_z_err == Z_ERRNO) 
		return 0;
	if (m_z_err == Z_STREAM_END) 
		return 0;  /* EOF */

	m_stream.next_out = (BYTE*)lpBuf;
	m_stream.avail_out = nCount;

	while (m_stream.avail_out != 0 && m_z_err != Z_STREAM_END) 
	{
		if (m_stream.avail_in == 0 && !m_z_eof) 
		{
			errno = 0;
			m_stream.avail_in = WrapRead(this, m_buf, sizeof(m_buf));
			if (m_stream.avail_in == 0)
			{
				m_z_eof = 1;
			}
			m_stream.next_in = m_buf;
		}

		#ifdef _TRACE
			BYTE *p_old_next_in  = m_stream.next_in;
			BYTE *p_old_next_out = m_stream.next_out;
			UINT  old_avail_in   = m_stream.avail_in;
			UINT  old_avail_out  = m_stream.avail_out;
		#endif

		m_z_err = inflate(&(m_stream), Z_NO_FLUSH);

		#ifdef _TRACE
			m_TraceFile.Write("IN:\n", 4);
			m_TraceFile.Write(p_old_next_in, old_avail_in - m_stream.avail_in);
			m_TraceFile.Write("\n", 1);

			m_TraceFile.Write("OUT:\n", 5);
			m_TraceFile.Write(p_old_next_out, old_avail_out - m_stream.avail_out);
			m_TraceFile.Write("\n", 1);
		#endif

		if (m_z_err == Z_STREAM_END) 
		{
			m_crc = crc32(m_crc, start, (uInt)(m_stream.next_out - start));
			start = m_stream.next_out;
		}
		if (m_z_err != Z_OK || m_z_eof) break;
	}
	m_crc = crc32(m_crc, start, (uInt)(m_stream.next_out - start));

	m_position += (nCount - m_stream.avail_out);

	return nCount - m_stream.avail_out;
}

void CUncompressFile::Write(const void *lpBuf, UINT nCount)
{
	if (m_stream.state == NULL)
	{
		// This sucka a'ready clozed
		ASSERT(FALSE);
		return;
	}

    m_stream.next_in = (BYTE*)lpBuf;
    m_stream.avail_in = nCount;

	if (m_stream.next_out == NULL)
	{	// This is our first write
		m_stream.next_out = m_buf;
		m_stream.avail_out = Z_BUFSIZE;
	}

    while (m_stream.avail_in != 0) 
	{
        if (m_stream.avail_out == 0) 
		{
            m_stream.next_out = m_buf;
            WrapWrite(this, m_buf, Z_BUFSIZE);
            m_stream.avail_out = Z_BUFSIZE;
        }
        m_z_err = inflate(&(m_stream), Z_NO_FLUSH);
        if (m_z_err != Z_OK) break;
    }
    m_crc = crc32(m_crc, (BYTE*)lpBuf, nCount);

	ASSERT(0 == m_stream.avail_in);
	m_position += nCount;
}

void CUncompressFile::Flush()
{
	if (m_stream.state == NULL)
	{
		// This sucka a'ready clozed
		return;
	}

    uInt len;
    int done = 0;

    m_stream.avail_in = 0; /* should be zero already anyway */

    for (;;) 
	{
        len = Z_BUFSIZE - m_stream.avail_out;

        if (len != 0) 
		{
			WrapWrite(this, m_buf, len);
            m_stream.next_out = m_buf;
            m_stream.avail_out = Z_BUFSIZE;
        }
        if (done) break;
        m_z_err = inflate(&(m_stream), Z_FINISH);

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer: 
         */
        done = (m_stream.avail_out != 0 || m_z_err == Z_STREAM_END);
 
        if (m_z_err != Z_OK && m_z_err != Z_STREAM_END) break;
    }
	CFile::Flush();

	ASSERT(m_z_err == Z_STREAM_END);
}

void CUncompressFile::Close()
{
	if (m_stream.state == NULL)
	{
		// This sucka a'ready clozed
		ASSERT(FALSE);
		return;
	}

	int err = inflateEnd(&(m_stream));
	ASSERT(err == Z_OK);
}


DWORD  CUncompressFile::GetPosition() const
{
	return CFile::GetPosition();
}

DWORD  CUncompressFile::GetLength() const
{
	return CFile::GetLength();
}

DWORD  CUncompressFile::GetExpandedLength() const
{
	return m_position;
}
