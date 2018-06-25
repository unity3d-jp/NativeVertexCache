namespace nvc {

inline size_t getSizeOfDataFormat(DataFormat dataFormat) {
	size_t size = 0;
	switch(dataFormat) {
	default:
	case Unknown:	size = 0;					break;
	case Int:		size = 1 * sizeof(int32_t);	break;
	case Int2:		size = 2 * sizeof(int32_t);	break;
	case Int3:		size = 3 * sizeof(int32_t);	break;
	case Int4:		size = 4 * sizeof(int32_t);	break;
	case Float:		size = 1 * sizeof(float);	break;
	case Float2:	size = 2 * sizeof(float);	break;
	case Float3:	size = 3 * sizeof(float);	break;
	case Float4:	size = 4 * sizeof(float);	break;
	case Half:		size = 1 * sizeof(half);	break;
	case Half2:		size = 2 * sizeof(half);	break;
	case Half3:		size = 3 * sizeof(half);	break;
	case Half4:		size = 4 * sizeof(half);	break;
	case SNorm16:	size = 1 * sizeof(snorm16);	break;
	case SNorm16x2:	size = 2 * sizeof(snorm16);	break;
	case SNorm16x3:	size = 3 * sizeof(snorm16);	break;
	case SNorm16x4:	size = 4 * sizeof(snorm16);	break;
	case UNorm16:	size = 1 * sizeof(unorm16);	break;
	case UNorm16x2:	size = 2 * sizeof(unorm16);	break;
	case UNorm16x3:	size = 3 * sizeof(unorm16);	break;
	case UNorm16x4:	size = 4 * sizeof(unorm16);	break;
	}
	return size;
}

} // namespace nvc
