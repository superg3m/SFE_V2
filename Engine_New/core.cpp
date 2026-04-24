#include "core.hpp"

String String::create(char* data, u64 length) {
	String ret = {};
	ret.data = data;
	ret.length = length;

	return ret;
}

bool String::operator==(const String& other) const {
	return memory_equal(this->data, this->length, other.data, other.length);
}

bool String::operator!=(const String& other) const {
	return !(*this == other);
}