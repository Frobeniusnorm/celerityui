#ifndef INTERNAL_HPP
#define INTERNAL_HPP
#include <mutex>
class Internal {
	static std::mutex gl_lock;
};
#endif
