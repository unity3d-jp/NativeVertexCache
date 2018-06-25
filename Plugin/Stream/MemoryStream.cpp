//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#include "MemoryStream.h"

MemoryStream::MemoryStream()
	:	m_Data(nullptr)
	,	m_Cursor(0)
	,	m_Length(0)
	,	m_Allocated(0)
	,	m_Managed(true)
	,	m_Resizable(true)
	,	m_Writable(true)
	,	m_Closed(false)
{
}

MemoryStream::MemoryStream(size_t length, bool resizable)
	:	m_Data(nullptr)
	,	m_Cursor(0)
	,	m_Length(0)
	,	m_Allocated(0)
	,	m_Managed(true)
	,	m_Resizable(resizable)
	,	m_Writable(true)
	,	m_Closed(false)
{
	grow(length);
}

MemoryStream::MemoryStream(void* buffer, size_t length, bool writable, bool managed)
	:	m_Data(buffer)
	,	m_Cursor(0)
	,	m_Length(length)
	,	m_Allocated(length)
	,	m_Managed(managed)
	,	m_Resizable(false)
	,	m_Writable(writable)
	,	m_Closed(false)
{
}

MemoryStream::~MemoryStream()
{
	if (m_Managed)
	{
		free(m_Data);
	}
}

void MemoryStream::setLength(size_t length)
{
	if (length > getCapacity())
	{
		grow(length);
	}

	m_Length = length;
}

void MemoryStream::copyTo(Stream* stream, size_t nSize)
{
	const uint64_t readSize = __min(getLength(), nSize);
	stream->write(m_Data, readSize);
}

size_t MemoryStream::read(void* buffer, size_t length) const
{
	assert(m_Closed == false);

	const size_t readSize = __min(getLength() - getPosition() - 1, length);
	if (readSize > 0)
	{
		memcpy(buffer, static_cast<int8_t*>(m_Data) + getPosition(), static_cast<size_t>(readSize));
		const_cast<MemoryStream*>(this)->seek(readSize, SeekOrigin::Current);
	}

	return readSize;
}

size_t MemoryStream::write(const void* buffer, size_t length)
{
	assert(m_Closed == false);

	size_t newPosition = getPosition() + length;
	if (newPosition > getCapacity())
	{
		grow(__max(length * 2, getCapacity() * 2));
	}

	// Update the length if we don't have a managed buffer (capacity won't change in the case of managed
	// buffer and counts as the extra if to cap the values).
	m_Length = __min(getCapacity(), __max(m_Length, newPosition));

	memcpy(static_cast<int8_t*>(m_Data) + getPosition(), buffer, static_cast<size_t>(length));
	seek(length, SeekOrigin::Current);

	return length;
}

void MemoryStream::seek(int offset, SeekOrigin origin)
{
	assert(m_Closed == false);

	switch (origin)
	{
	case SeekOrigin::Begin:
		{
		assert(offset <= static_cast<int>(getLength()));
			m_Cursor = offset;
		}
		break;

	case SeekOrigin::Current:
		{
			const size_t currentOffset = static_cast<size_t>(m_Cursor) + offset;
			assert(currentOffset <= static_cast<size_t>(getLength()));

			m_Cursor = currentOffset;
		}
		break;

	case SeekOrigin::End:
		{
			const size_t currentOffset = static_cast<size_t>(getLength()) + offset;
			assert((currentOffset <= static_cast<size_t>(getLength())) && (currentOffset > 0));

			m_Cursor = currentOffset;
		}
		break;

	default:
		break;
	}
}

void MemoryStream::flush()
{
	assert(m_Closed == false);

	// Memory stream write directly to the memory buffer, so nothing is required here.
}

void MemoryStream::close()
{
	if (m_Managed)
	{
		free(m_Data);
		m_Data = nullptr;
		m_Writable = false;
		m_Closed = true;
	}
}

void MemoryStream::grow(uint64_t requestedSize)
{
	if (m_Managed && (getCapacity() < requestedSize))
	{
		m_Data = realloc(m_Data, static_cast<size_t>(requestedSize));
		if (m_Data != nullptr)
		{
			m_Allocated = requestedSize;
		}
		else
		{
			m_Allocated = 0;
		}
	}
}
