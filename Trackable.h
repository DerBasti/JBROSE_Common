#pragma once
#include <type_traits>

template<class T>
struct is_character_based : std::false_type {};

template<>
struct is_character_based<char> : std::true_type {};

template<>
struct is_character_based<wchar_t> : std::true_type {};

template<class T, class = typename std::enable_if<is_character_based<std::remove_cv<std::remove_pointer<T>::type>::type>::value>::type>
class Trackable
{
protected:
	unsigned long capacity;
	T* dataHolder = nullptr;
public:
	Trackable() : Trackable(0) {}
	Trackable(unsigned long size) {
		if (size > 0) {
			dataHolder = new T[size];
		}
		else {
			dataHolder = nullptr;
		}
		capacity = size;
	}
	virtual ~Trackable() {
		delete[] dataHolder;
		dataHolder = nullptr;
		capacity = 0;
	}

	operator T*() const {
		return get();
	}

	__inline T* get() const {
		return dataHolder;
	}

	__inline unsigned long getCapacity() const {
		return capacity;
	}
};

template<class T, class X = typename std::enable_if<is_character_based<std::remove_cv<std::remove_pointer<T>::type>::type>::value>::type>
class TrackableAppender : public Trackable<T, X> {
private:
	unsigned long caret;
public:
	TrackableAppender() : TrackableAppender(0x400) {}

	TrackableAppender(unsigned long size) : Trackable<T, X>(size) {
		caret = 0;
	}
	virtual ~TrackableAppender() {
		caret = -1;
	}

	void add(const T& value) {
		dataHolder[caret] = value;
		caret++;
	}

	void add(const T* values, unsigned long amountOfValues) {
		for (unsigned long i = 0; i < amountOfValues; i++, values++) {
			add(*values);
		}
	}

	__inline unsigned long getCurrentlyUsedSize() const {
		return caret;
	}

	__inline void reset() {
		memset(dataHolder, 0x00, sizeof(T)*getCapacity());
		caret = 0;
	}
};
