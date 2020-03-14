#ifndef FILE_DYNAR_HPP
#define FILE_DYNAR_HPP
#include "syscall.h"
#include "stdlib.h"
#include "util.h"

namespace util {
	template<class T>
	class dynar {
	private:
		T* _Data;
		size_t _Size, _Cap;
	public:
		constexpr dynar() noexcept
			: _Data(), _Size(), _Cap() {}
		inline dynar(size_t cap) noexcept
			: _Data(new T[cap]), _Size(), _Cap(cap) {}
		inline dynar(const dynar& a) noexcept
			: _Data(copy(new T[a._Cap], a._Data, a._Cap)), _Size(a._Size), _Cap(a._Cap) {}
		inline dynar(dynar&& a) noexcept
			: _Data(a._Data), _Size(a._Size), _Cap(a._Cap) { a._Data = nullptr; }
		inline ~dynar() noexcept {
			if (_Data) {
				delete[] _Data;
				_Data = nullptr;
			}
		}
		constexpr size_t size() const noexcept { return _Size; }
		constexpr size_t capacity() const noexcept { return _Cap; }
		constexpr const T* data() const noexcept { return _Data; }
		inline T* data() noexcept { return _Data; }
		inline dynar& operator=(const dynar& a) noexcept {
			if (this == &a) return *this;
			if (_Cap < a._Cap) {
				delete[] _Data;
				_Data = new T[_Cap = a._Cap];
			}
			copy(_Data, a._Data, a._Cap);
			_Size = a._Size;
			return *this;
		}
		inline dynar& operator=(dynar&& a) noexcept {
			delete[] _Data;
			_Data = a._Data;
			_Size = a._Size;
			_Cap = a._Cap;
			a._Data = nullptr;
			return *this;
		}
		inline bool resize(size_t new_cap) {
			if (new_cap > _Cap) {
				T* data = copy(new T[new_cap], _Data, _Cap);
				if (!data) return false;
				delete[] _Data;
				_Cap = new_cap;
				_Data = data;
			}
			return true;
		}
		inline void add(const T& x) noexcept {
			if (_Size >= _Cap) {
				if (!resize(_Cap * 2)) {
					// TODO: error handling
					return;
				}
			}
			_Data[_Size++] = x;
		}
		inline void add(T&& x) noexcept {
			if (_Size >= _Cap) {
				if (!resize(_Cap * 2)) {
					sys_excep("Couldn't allocate more RAM\n");
					return;
				}
			}
			_Data[_Size++] = (T&&)x;
		}
		template<class... Args>
		inline void emplace(Args... args) noexcept {
			if (_Size >= _Cap) {
				if (!resize(_Cap * 2)) {
					sys_excep("Couldn't allocate more RAM\n");
				}
			}
			_Data[_Size++] = T(args...);
		}
		T& operator[](size_t i) {
			if (i >= _Cap) out_of_bounds();
			else return _Data[i];
		}
		const T& operator[](size_t i) const {
			if (i >= _Cap) out_of_bounds();
			else return _Data[i];
		}
		void clear() noexcept {
			_Size = 0;
		}
	private:
		static T* copy(T* dest, const T* src, size_t size) noexcept {
			for (size_t i = 0; i < size; ++i) dest[i] = src[i];
			return dest;
		}
	};
}

#endif /* FILE_DYNAR_HPP */