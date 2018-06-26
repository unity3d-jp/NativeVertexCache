//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#if defined(_MSC_VER)
#include <io.h> // _filelengthi64
#include <fcntl.h> // _O_RDONLY, etc.
#endif
#include "FileStream.h"

namespace {
	DWORD DWORD_HI(int64_t x) {
		return static_cast<DWORD>(x >> 32);
	}

	DWORD DWORD_LO(int64_t x) {
		return static_cast<DWORD>(x);
	}

	class AutoMmap {
	public:
		AutoMmap(HANDLE handle, int64_t offset, int64_t length, DWORD dwDesiredAccess, int64_t pageSize)
			: m_Handle { handle }
			, m_Offset { offset }
			, m_Length { length }
			, m_dwDesiredAccess { dwDesiredAccess }
			, m_PageSize { pageSize }
			, m_Map { nullptr }
		{
			map();
		}

		~AutoMmap() {
			unmap();
		}

		bool good() const {
			return data() != nullptr;
		}

		uint8_t* data() const {
			return m_Base;
		}

		int64_t size() const {
			return m_Length;
		}

	protected:
		void map() {
			// @todo : align m_Offset to m_PageSize
			// @todo : extend m_Length which covers entire window with aligned m_Offset
			// @todo : tweak returned address of data()
			const auto alignedOffset = [&]() {
				return (m_Offset / m_PageSize) * m_PageSize;
			} ();
			const auto headDiff = [&]() {
				return (m_Offset - alignedOffset);
			} ();
			const auto alignedLength = [&]() {
				return headDiff + m_Length;
			} ();

			m_Map = static_cast<uint8_t*>(MapViewOfFileEx(
				  m_Handle
				, m_dwDesiredAccess
				, DWORD_HI(alignedOffset)
				, DWORD_LO(alignedOffset)
				, alignedLength
				, nullptr
			));
			assert(m_Map != nullptr);

			m_Base = m_Map + headDiff;
		}

		void unmap() {
			if(m_Map != nullptr) {
				UnmapViewOfFile(m_Map);
				m_Map = nullptr;
			}
		}

		int64_t alignToPageSize(int64_t size) const {
			return ((size + m_PageSize - 1) / m_PageSize) * m_PageSize;
		}

		HANDLE m_Handle;
		int64_t m_Offset;
		int64_t m_Length;
		DWORD m_dwDesiredAccess;
		int64_t m_PageSize;
		uint8_t* m_Map;
		uint8_t* m_Base;
	};
} // Anonymous namespace

FileStream::FileStream()
{
}

FileStream::FileStream(const char* filename, OpenModes openModes)
	: FileStream()
{
	open(filename, openModes);
	if(!isGood()) {
		close();
	}
}

FileStream::~FileStream()
{
	close();
}

bool FileStream::canRead() const
{
	return isGood()
		&& ((m_OpenModes & OpenModes::Read) != 0);
}

bool FileStream::canWrite() const
{
	return isGood()
		&& ((m_OpenModes & OpenModes::Write) != 0);
}

bool FileStream::canSeek() const
{
	return isGood()
		&& ((m_OpenModes & OpenModes::RandomAccess) != 0);
}

bool FileStream::isEof() const
{
	return (!isGood())
		|| (getPosition() >= getLength())
	;
}


size_t FileStream::getPosition() const
{
	assert(isGood());
	return static_cast<size_t>(m_Cursor);
}

size_t FileStream::getLength() const
{
	assert(isGood());
	return static_cast<size_t>(m_Length);
}

void FileStream::setLength(size_t length)
{
	assert(canWrite());

	const auto sLength = static_cast<int64_t>(length);
	grow(sLength);
	m_Length = sLength;
}

void FileStream::copyTo(Stream* stream, size_t length)
{
	assert(canRead() && stream->canWrite());

	const auto sLength = static_cast<int64_t>(length);
	const auto readSize = min(m_Length - m_Cursor, sLength);
	if(readSize > 0) {
		AutoMmap autoMmap(
			  m_HandleMap
			, m_Cursor
			, readSize
			, m_dwDesiredAccess
			, m_PageSize
		);
		stream->write(autoMmap.data(), readSize);
		seek(readSize, SeekOrigin::Current);
	}
}

size_t FileStream::read(void* buffer, size_t length) const
{
	assert(canRead());
	const auto sLength = static_cast<int64_t>(length);
	const auto readSize = min(m_Length - m_Cursor, sLength);
	if(readSize > 0) {
		AutoMmap autoMmap(
			  m_HandleMap
			, m_Cursor
			, readSize
			, m_dwDesiredAccess
			, m_PageSize
		);

		memcpy(buffer, autoMmap.data(), readSize);
		const_cast<FileStream*>(this)->seek(readSize, SeekOrigin::Current);
	}
	// note: should we take care about error case?
	return readSize;
}

size_t FileStream::write(const void* buffer, size_t length)
{
	assert(canWrite());

	const auto sLength = static_cast<int64_t>(length);
	const auto nextCursor = m_Cursor + sLength;
	if(nextCursor > m_Length) {
		setLength(nextCursor);
	}

	{
		AutoMmap autoMmap(
			  m_HandleMap
			, m_Cursor
			, sLength
			, m_dwDesiredAccess
			, m_PageSize
		);
		memcpy(autoMmap.data(), buffer, sLength);
		seek(sLength, SeekOrigin::Current);
	}
	return length;
}

void FileStream::seek(int64_t offset, SeekOrigin origin)
{
	assert(canSeek());
	int64_t nextCursor = -1;

	switch(origin) {
	default:
		break;
	case Stream::SeekOrigin::Begin:
		nextCursor = offset;
		break;
	case Stream::SeekOrigin::Current:
		nextCursor = m_Cursor + offset;
		break;
	case Stream::SeekOrigin::End:
		nextCursor = m_Length - offset;
		break;
	}
	assert(nextCursor >= 0 && nextCursor <= m_Length);

	m_Cursor = nextCursor;
}

void FileStream::flush()
{
	assert(isGood());
	// note : since we're always map/unmap view for each file access,
    //        we don't need to flush so far.
//	if((m_OpenModes & OpenModes::Write) != 0) {
//		FlushViewOfFile(m_Map, m_Length);
//	}
}

bool FileStream::open(const char* filename, OpenModes openModes) {
	close();
	this->m_OpenModes = openModes;

	int oflag = -1;
	int pmode = 0;

	switch(openModes) {
	default:	break;
	case FileStream::OpenModes::Random_ReadOnly:		oflag = _O_RANDOM     | _O_RDONLY;						m_flProtect = PAGE_READONLY;	m_dwDesiredAccess = FILE_MAP_READ;		break;
	case FileStream::OpenModes::Sequential_ReadOnly:	oflag = _O_SEQUENTIAL | _O_RDONLY;						m_flProtect = PAGE_READONLY;	m_dwDesiredAccess = FILE_MAP_READ;		break;
	case FileStream::OpenModes::Random_WriteOnly:		oflag = _O_RANDOM     | _O_WRONLY | _O_TRUNC;			m_flProtect = PAGE_READWRITE;	m_dwDesiredAccess = FILE_MAP_ALL_ACCESS;	break;
	case FileStream::OpenModes::Sequential_WriteOnly:	oflag = _O_SEQUENTIAL | _O_WRONLY | _O_TRUNC;			m_flProtect = PAGE_READWRITE;	m_dwDesiredAccess = FILE_MAP_ALL_ACCESS;	break;
	case FileStream::OpenModes::Random_ReadWrite:		oflag = _O_RANDOM     | _O_RDWR | _O_CREAT | _O_TRUNC;	m_flProtect = PAGE_READWRITE;	m_dwDesiredAccess = FILE_MAP_ALL_ACCESS;	break;
	case FileStream::OpenModes::Sequential_ReadWrite:	oflag = _O_SEQUENTIAL | _O_RDWR | _O_CREAT | _O_TRUNC;	m_flProtect = PAGE_READWRITE;	m_dwDesiredAccess = FILE_MAP_ALL_ACCESS;	break;
	}
	if(oflag == -1) {
		return false;
	}
	oflag |= _O_BINARY;

	if(oflag & _O_TRUNC) {
		pmode = _S_IREAD | _S_IWRITE;
	} else {
		pmode = _S_IREAD;
	}

	m_Fd = _open(filename, oflag, pmode);
	if(m_Fd < 0) {
		return false;
	}

	m_PageSize = 64 * 1024;
	m_GrowSize = m_PageSize * 16;

	if(openModes & OpenModes::CreateAlways) {
		m_Length = 0;
	} else {
		m_Length = _filelengthi64(m_Fd);
	}

	m_Capacity = alignToPageSize(m_Length + m_PageSize * 16);

	m_HandleMap = CreateFileMapping(
		  reinterpret_cast<HANDLE>(_get_osfhandle(m_Fd))
		, NULL
		, m_flProtect
		, DWORD_HI(m_Capacity)
		, DWORD_LO(m_Capacity)
		, NULL
	);
	if(m_HandleMap == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

void FileStream::close()
{
	// Since we'll close m_HandleMap after this line, must call canWrite() and getLength() here.
	const auto oCanWrite = canWrite();

	if(m_HandleMap != INVALID_HANDLE_VALUE) {
		CloseHandle(m_HandleMap);
		m_HandleMap = INVALID_HANDLE_VALUE;
	}

	if(m_Fd != InvalidFd) {
		if(oCanWrite) {
			const auto truncateResult = truncate(m_Length);
			assert(truncateResult);
		}
		_close(m_Fd);
		m_Fd = InvalidFd;
	}

	m_OpenModes = OpenModes::None;
}

bool FileStream::isGood() const
{
	return (m_Fd != InvalidFd) && (m_HandleMap != INVALID_HANDLE_VALUE);
}

bool FileStream::grow(int64_t size)
{
	if(size <= m_Capacity) {
		return true;
	}

	assert(canWrite());

	if(m_HandleMap != INVALID_HANDLE_VALUE) {
		CloseHandle(m_HandleMap);
		m_HandleMap = INVALID_HANDLE_VALUE;
	}

	m_Capacity = alignToPageSize(size + m_GrowSize);

	m_HandleMap = CreateFileMapping(
		  reinterpret_cast<HANDLE>(_get_osfhandle(m_Fd))
		, NULL
		, m_flProtect
		, DWORD_HI(m_Capacity)
		, DWORD_LO(m_Capacity)
		, NULL
	);
	if(m_HandleMap == INVALID_HANDLE_VALUE) {
		return false;
	}
	return true;
}

int64_t FileStream::alignToPageSize(int64_t size) const {
	return ((size + m_PageSize - 1) / m_PageSize) * m_PageSize;
}

bool FileStream::truncate(int64_t size) {
	// _chsize_s
	// https://msdn.microsoft.com/en-us/library/whx354w1.aspx
	const auto result = _chsize_s(m_Fd, size);
	return result == 0;
}
