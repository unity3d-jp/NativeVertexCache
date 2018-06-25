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
}


bool FileStream::canWrite() const
{
}

bool FileStream::canSeek() const
{
}

bool FileStream::isEof() const
{
}


size_t FileStream::getPosition() const
{
}

size_t FileStream::getLength() const
{
}

void FileStream::setLength(size_t length)
{
}

void FileStream::copyTo(Stream* stream, size_t length)
{
}

size_t FileStream::read(void* buffer, size_t length) const
{
}

size_t FileStream::write(const void* buffer, size_t length)
{
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
