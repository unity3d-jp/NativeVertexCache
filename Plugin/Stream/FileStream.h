#pragma once

//! Local Includes.
#include "Stream.h"

class FileStream final : Stream
{
public:
	enum OpenModes : int32_t {
		// note: writable file is always resizable
		None				= 0,
		Read				= 1 << 0,
		Write				= 1 << 1,
		SequentialAccess	= 0 << 2,
		RandomAccess		= 1 << 2,

		Random_ReadOnly			= Read | RandomAccess,
		Sequential_ReadOnly		= Read | SequentialAccess,
		Random_WriteOnly		= Write | RandomAccess,
		Sequential_WriteOnly	= Write | SequentialAccess,
		Random_ReadWrite		= Read | Write | RandomAccess,
		Sequential_ReadWrite	= Read | Write | SequentialAccess,
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
	bool isGood() const;

	OpenModes openModes = OpenModes::None;
	FILE* fp = nullptr;
};
