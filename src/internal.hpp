#ifndef INTERNAL_HPP
#define INTERNAL_HPP
#include <mutex>
struct Internal {
	static std::mutex gl_lock;
};
#endif
