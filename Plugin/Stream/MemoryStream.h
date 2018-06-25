#pragma once

//! Local Includes.
#include "Stream.h"

class MemoryStream final : public Stream
{
protected:
	void* m_Data;

	size_t m_Cursor;

	size_t m_Length;

	size_t m_Allocated;

	bool m_Managed;

	bool m_Resizable;

	bool m_Writable;

	bool m_Closed;

public:
	MemoryStream();
	MemoryStream(size_t length, bool resizable);
	MemoryStream(void* buffer, size_t length, bool writable, bool managed = false);
	~MemoryStream();

	const void* getBuffer() const;

	size_t getCapacity() const;

	bool canRead() const override;
	bool canWrite() const override;
	bool canSeek() const override;
	bool isEof() const override;

	size_t getPosition() const override;
	
	size_t getLength() const override;
	void setLength(size_t length) override;

	void copyTo(Stream* stream, size_t nSize) override;

	size_t read(void* buffer, size_t length) const override;
	size_t write(const void* buffer, size_t length) override;

	void seek(int offset, SeekOrigin origin) override;

	void flush() override;
	void close() override;

	template <typename TData> 
	TData read() const;

	template <typename TData> 
	void read(TData& d) const;

	template <typename TData> 
	void write(const TData& d);

	//...
	MemoryStream(const MemoryStream&) = delete;
	MemoryStream(MemoryStream&&) = delete;
	MemoryStream& operator=(const MemoryStream&) = delete;
	MemoryStream& operator=(MemoryStream&&) = delete;

private:
	void grow(uint64_t requestedSize);
};

inline const void* MemoryStream::getBuffer() const
{
	return m_Data;
}

inline size_t MemoryStream::getCapacity() const
{
	return m_Allocated;
}

inline bool MemoryStream::canRead() const
{
	return true;
}

inline bool MemoryStream::canWrite() const
{
	return m_Writable;
}

inline bool MemoryStream::canSeek() const
{
	return true;
}

inline bool MemoryStream::isEof() const
{
	return m_Cursor >= m_Length;
}

inline size_t MemoryStream::getPosition() const
{
	return m_Cursor;
}

inline size_t MemoryStream::getLength() const
{
	return m_Length;
}

template <typename TData>
TData MemoryStream::read() const
{
	TData d;
	Read(&d, sizeof(TData));
	return d;
}

template <typename TData>
void MemoryStream::read(TData& d) const
{
	Read(&d, sizeof(TData));
}

template <typename TData>
void MemoryStream::write(const TData& d)
{
	Write(&d, sizeof(TData));
}
