#pragma once

//! Local Includes.
#include "Stream.h"

class FileStream final : Stream
{
public:
	FileStream();
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
};
