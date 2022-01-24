#ifndef __ROSE_WRAPPING_NUMERIC__
#define __ROSE_WRAPPING_NUMERIC__

#include <type_traits>
#include <stdint.h>

template<class _Number, class = typename std::enable_if<std::is_integral<_Number>::value || std::is_floating_point<_Number>::value>::type>
class LimitingNumber {
private:
	_Number number;
	_Number maximum;

	__inline void setNumber(_Number newNumber) {
		number = (std::min)(newNumber, maximum);
	}
public:
	LimitingNumber() : LimitingNumber(_Number(0)) {

	}
	LimitingNumber(_Number value) : LimitingNumber(value, (std::numeric_limits<_Number>::max)()) {

	}
	LimitingNumber(_Number value, _Number maximum) {
		number = value;
		this->maximum = maximum;
	}
	LimitingNumber& operator=(_Number number) {
		setNumber(number);
		return (*this);
	}
	operator _Number() const {
		return number;
	}
	__inline _Number getValue() const {
		return number;
	}
	__inline _Number getMaximumLimit() const {
		return maximum;
	}
	__inline void setMaximumLimit(_Number limit) {
		this->maximum = limit;
		setNumber(number);
	}
};

template<class _Number>
std::wostream& operator<<(std::wostream& out, LimitingNumber<_Number> number) {
	out << number.getValue();
	return out;
}

typedef LimitingNumber<uint8_t> LimitingByte;
typedef LimitingNumber<int16_t> LimitingShort;
typedef LimitingNumber<uint16_t> LimitingUShort;
typedef LimitingNumber<int32_t> LimitingInt;
typedef LimitingNumber<uint32_t> LimitingUInt;
typedef LimitingNumber<float> LimitingFloat;
typedef LimitingNumber<double> LimitingDouble;

template<class _T, class = typename std::enable_if<std::is_integral<_T>::value || std::is_floating_point<_T>::value>::type>
class WrappingNumeric {
private:
	_T maximum;
	_T current;
public:
	WrappingNumeric() : WrappingNumeric<_T>(static_cast<_T>(-1)) {}
	WrappingNumeric(_T maximum) {
		this->maximum = maximum;
		current = 0;
	}
	virtual ~WrappingNumeric() { }

	operator _T() const {
		return getCurrentValue();
	}
	WrappingNumeric& operator=(const _T value) {
		setCurrentValue(value);
		return (*this);
	}
	__inline _T getCurrentValue() const {
		return current;
	}
	__inline void setCurrentValue(const _T newCurrent) {
		current = newCurrent;
		if (current >= maximum) {
			current = 0;
		}
	}
	__inline _T getMaximum() const {
		return maximum;
	}
	__inline void setMaximum(_T maximumValue) {
		this->maximum = maximumValue;
	}
};

class WrappingUInt : public WrappingNumeric<uint32_t> {
public:
	WrappingUInt() : WrappingUInt(-1) {
	
	}

	WrappingUInt(uint32_t maximum) : WrappingNumeric<uint32_t>(maximum) {

	}

	virtual ~WrappingUInt() {
	
	}

	__inline WrappingUInt& operator++(int) {
		setCurrentValue(getCurrentValue() + 1);
		return (*this);
	}

	__inline WrappingUInt operator++() {
		WrappingUInt temp = (*this);
		(*this)++;
		return temp;
	}
};

#endif 