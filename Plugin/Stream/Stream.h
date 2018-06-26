#pragma once

class Stream
{
public:
	enum class SeekOrigin
	{
		Begin,
		Current,
		End,
	};

public:
	Stream() = default;
	virtual ~Stream() = default;

	virtual bool canRead() const = 0;
	virtual bool canWrite() const = 0;
	virtual bool canSeek() const = 0;
	virtual bool isEof() const = 0;

	virtual size_t getPosition() const = 0;

	virtual size_t getLength() const = 0;
	virtual void setLength(size_t length) = 0;

	virtual void copyTo(Stream* stream, size_t length) = 0;

	virtual size_t read(void* buffer, size_t length) const = 0;
	virtual size_t write(const void* buffer, size_t length) = 0;

	virtual void seek(int64_t offset, SeekOrigin origin) = 0;

	virtual void flush() = 0;
	virtual void close() = 0;

	template <typename TData>
	TData read() { TData d; read(&d, sizeof(TData)); }

	template <typename TData>
	void read(TData& d) { read(&d, sizeof(TData)); }

	template <typename TData>
	void write(const TData& d) { write(&d, sizeof(TData)); }

	//...
	Stream(const Stream&) = delete;
	Stream(Stream&&) = delete;
	Stream& operator=(const Stream&) = delete;
	Stream& operator=(Stream&&) = delete;
};
