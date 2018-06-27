#pragma once

//! Local Includes.
#include "Stream.h"
#if defined(_MSC_VER)
#include <windows.h>
#endif

class FileStream final : public Stream
{
public:
	enum OpenModes : int32_t {
		// note: writable file is always resizable
		None				= 0,
		Read				= 1 << 0,
		Write				= 1 << 1,
		CreateAlways		= 1 << 2,
		CreateIfNotExist	= 1 << 3,
		SequentialAccess	= 0 << 4,
		RandomAccess		= 1 << 4,

		Random_ReadOnly			= Read | RandomAccess,
		Sequential_ReadOnly		= Read | SequentialAccess,
		Random_WriteOnly		= Write | CreateAlways | RandomAccess,
		Sequential_WriteOnly	= Write | CreateAlways | SequentialAccess,
		Random_ReadWrite		= Read | Write | CreateIfNotExist | RandomAccess,
		Sequential_ReadWrite	= Read | Write | CreateIfNotExist | SequentialAccess,
	};

	FileStream(const char* filename, OpenModes openModes);
	~FileStream();

	bool canRead() const override;
	bool canWrite() const override;
	bool canSeek() const override;
	bool isEof() const override;

	size_t getPosition() const override;

	size_t getLength() const override;
	void setLength(size_t length) override;

	void copyTo(Stream* stream, size_t length) override;

	size_t read(void* buffer, size_t length) const override;
	size_t write(const void* buffer, size_t length) override;

	void seek(int64_t offset, SeekOrigin origin) override;

	void flush() override;
	void close() override;

	//...
	FileStream(const FileStream&) = delete;
	FileStream(FileStream&&) = delete;
	FileStream& operator=(const FileStream&) = delete;
	FileStream& operator=(FileStream&&) = delete;

protected:
	FileStream();
	bool open(const char* filename, OpenModes openModes);
	bool grow(int64_t size);
	bool isGood() const;
	int64_t alignToPageSize(int64_t size) const;
	bool truncate(int64_t size);

	const int InvalidFd = -1;

	OpenModes m_OpenModes = OpenModes::None;
	int m_Fd = InvalidFd;
	HANDLE m_HandleMap = INVALID_HANDLE_VALUE;
//	uint8_t* m_Map = nullptr;
//	const int DefaultWindowLength = 1024 * 1024;
//	int64_t m_WindowOffset = 0;
//	int64_t m_WindowLength = DefaultWindowLength;

	int64_t m_Cursor = 0;
	int64_t m_Length = 0;
	int64_t m_Capacity = 0;
	int64_t m_PageSize = 0;
	int64_t m_GrowSize = 0;
	DWORD m_flProtect = 0;
	DWORD m_dwDesiredAccess = 0;
};
