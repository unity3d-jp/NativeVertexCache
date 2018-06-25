//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "FileStream.h"

FileStream::FileStream()
{
}

FileStream::~FileStream()
{
}

bool FileStream::canRead() const
{
	return false;
}


bool FileStream::canWrite() const
{
	return false;
}

bool FileStream::canSeek() const
{
	return false;
}

bool FileStream::isEof() const
{
	return false;
}


size_t FileStream::getPosition() const
{
	return 0;
}

size_t FileStream::getLength() const
{
	return 0;
}

void FileStream::setLength(size_t length)
{
}

void FileStream::copyTo(Stream* stream, size_t length)
{
}

size_t FileStream::read(void* buffer, size_t length) const
{
	return 0;
}

size_t FileStream::write(const void* buffer, size_t length)
{
	return 0;
}

void FileStream::seek(int offset, SeekOrigin origin)
{
}

void FileStream::flush()
{
}

void FileStream::close()
{
}
