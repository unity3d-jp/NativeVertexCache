//! PrecompiledHeader Include.
#include "Plugin/PrecompiledHeader.h"

//! Header Include.
#if defined(_MSC_VER)
#include <io.h> // _filelengthi64
#endif
#include "FileStream.h"

namespace {
	using File = FILE*;

	bool isGoodFile(const File& file) {
		return file != nullptr;
	}

	bool FsetInvalid(File& file) {
		file = nullptr;
		return true;
	}

	bool Fopen(const std::string& filename, File* outFp, FileStream::OpenModes openModes) {
		*outFp = nullptr;
		const char* oms = nullptr;

#if defined(_MSC_VER)
		switch(openModes) {
		default:	break;
		case FileStream::OpenModes::Random_ReadOnly:		oms = "Rrb";	break;
		case FileStream::OpenModes::Sequential_ReadOnly:	oms = "Srb";	break;
		case FileStream::OpenModes::Random_WriteOnly:		oms = "Rwb";	break;
		case FileStream::OpenModes::Sequential_WriteOnly:	oms = "Swb";	break;
		case FileStream::OpenModes::Random_ReadWrite:		oms = "Rwb";	break;
		case FileStream::OpenModes::Sequential_ReadWrite:	oms = "Swb";	break;
		}
#else
		///! @todo : implement
		assert(0);
#endif

		if(oms == nullptr) {
			return false;
		}
		*outFp = fopen(filename.c_str(), "rb");
		return isGoodFile(*outFp);
	}

	bool Fclose(File fp) {
		assert(isGoodFile(fp));

		if(isGoodFile(fp)) {
			fclose(fp);
		}
		return true;
	}

	bool Fread(File fp, void* buf, int64_t bufLength, int64_t* readBytes) {
		assert(isGoodFile(fp));
		*readBytes = 0;
		const auto result = fread(buf, 1, bufLength, fp);
		*readBytes = result;
		return *readBytes == bufLength;
	}

	bool Fwrite(File fp, const void* buf, int64_t bufLength, int64_t* writtenBytes) {
		assert(isGoodFile(fp));
		*writtenBytes = 0;
		const auto result = fwrite(buf, 1, bufLength, fp);
		*writtenBytes = result;
		return *writtenBytes == bufLength;
	}

	bool Fseek(File fp, int64_t offset, Stream::SeekOrigin origin) {
		assert(isGoodFile(fp));

		switch(origin) {
		default:
			return false;
			break;
		case Stream::SeekOrigin::Begin:
			{
				const auto result = _fseeki64(fp, offset, SEEK_SET);
				return result == 0;
			}
			break;
		case Stream::SeekOrigin::Current:
			{
				const auto result = _fseeki64(fp, offset, SEEK_CUR);
				return result == 0;
			}
			break;
		case Stream::SeekOrigin::End:
			{
				const auto result = _fseeki64(fp, offset, SEEK_END);
				return result == 0;
			}
			break;
		}
	}

	bool Ftell(File fp, int64_t* position) {
		assert(isGoodFile(fp));
		const auto result = _ftelli64(fp);
		// TODO : Error handling
		*position = result;
		return true;
	}

	bool FgetLength(File fp, int64_t* length) {
		assert(isGoodFile(fp));
		*length = 0;
		const auto fno = _fileno(fp);
#if defined(_MSC_VER)
		// _filelength, _filelengthi64
		// https://msdn.microsoft.com/en-us/library/dfbc2kec.aspx
		*length = _filelengthi64(fno);
#else
		///! @todo : implement with fstat()
		assert(0);
#endif
		return true;
	}

	bool FFlush(File fp) {
		assert(isGoodFile(fp));
		fflush(fp);
		return true;
	}

	bool Ftruncate(File fp, int64_t size) {
		const auto fno = _fileno(fp);
#if defined(_MSC_VER)
		// _chsize_s
		// https://msdn.microsoft.com/en-us/library/whx354w1.aspx
		const auto result = _chsize_s(fno, size);
		return result == 0;
#else
		///! @todo : implement
		assert(0);
#endif
	}
} // namespace (anonymous)


FileStream::FileStream()
{
}

FileStream::FileStream(const char* filename, OpenModes openModes)
	: FileStream()
{
	this->openModes = openModes;
	const auto result = Fopen(filename, &fp, openModes);
	if(! result) {
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
		&& ((openModes & OpenModes::Read) != 0);
}

bool FileStream::canWrite() const
{
	return isGood()
		&& ((openModes & OpenModes::Write) != 0);
}

bool FileStream::canSeek() const
{
	return isGood()
		&& ((openModes & OpenModes::RandomAccess) != 0);
}

bool FileStream::isEof() const
{
	return (!isGood())
		|| (getPosition() == getLength())
	;
}


size_t FileStream::getPosition() const
{
	assert(isGood());
	int64_t position = 0;
	if(! Ftell(fp, &position)) {
		return 0;
	}
	return static_cast<size_t>(position);
}

size_t FileStream::getLength() const
{
	assert(isGood());
	int64_t length = 0;
	if(! FgetLength(fp, &length)) {
		return 0;
	}
	return static_cast<size_t>(length);
}

void FileStream::setLength(size_t length)
{
	assert(canWrite());

	const auto sLength = static_cast<int64_t>(length);
	Ftruncate(fp, sLength);
	// - setLength() always set file pointer to the end of file.
	// - if filestream is not seekable, we ignore it here.
	Fseek(fp, length, SeekOrigin::Begin);
}

void FileStream::copyTo(Stream* stream, size_t length)
{
	const bool ok = canRead() && stream->canWrite();
	assert(ok);
	if(ok) {
		///! @TODO : implement more efficient code.
		///! @TODO : if we can allocate a buffer in stream directly, we can avoid to copy.
		std::vector<uint8_t> buf(1024 * 1024);
		for(auto remain = length; remain > 0; ) {
			const int64_t bytesToRead = std::min(remain, buf.size());
			{
				int64_t readBytes;
				Fread(fp, buf.data(), bytesToRead, &readBytes);
				assert(readBytes == bytesToRead);
			}
			stream->write(buf.data(), bytesToRead);
			remain -= bytesToRead;
		}
	}
}

size_t FileStream::read(void* buffer, size_t length) const
{
	assert(canRead());
	int64_t readBytes = 0;
	{
		// note : read() is some kind of mutable method. Because this method changes file pointer/cursor.
		const auto sLength = static_cast<int64_t>(length);
		const auto result = Fread(fp, buffer, sLength, &readBytes);
		assert(result && (readBytes == sLength));
	}
	return readBytes;
}

size_t FileStream::write(const void* buffer, size_t length)
{
	assert(canWrite());
	int64_t writtenBytes = 0;
	{
		const auto sLength = static_cast<int64_t>(length);
		const auto result = Fwrite(fp, buffer, sLength, &writtenBytes);
		assert(result && (writtenBytes == sLength));
	}
	return writtenBytes;
}

void FileStream::seek(int64_t offset, SeekOrigin origin)
{
	assert(canSeek());
	const auto result = Fseek(fp, offset, origin);
	assert(result);
}

void FileStream::flush()
{
	assert(isGood());
	FFlush(fp);
}

void FileStream::close()
{
	if(isGood()) {
		Fclose(fp);
		FsetInvalid(fp);
	}
	openModes = OpenModes::None;
}

bool FileStream::isGood() const
{
	return isGoodFile(fp);
}
