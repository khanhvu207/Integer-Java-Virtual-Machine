#include "util.h"

word_t HexBytestoInt(byte_t* bytes) {
	return  ((bytes[0]&0xFF)<<24) | ((bytes[1]&0xFF)<<16) | ((bytes[2]&0xFF)<<8) | (bytes[3]&0xFF);
}
