#ifndef __ROSE_RANDOMIZER__
#define __ROSE_RANDOMIZER__

#include <random>
#include <type_traits>

template<class _T, class = typename std::enable_if<std::is_integral<_T>::value || std::is_floating_point<_T>::value>::type>
class NumericRandomizer {
private:
	_T minimumValue;
	_T maximumValue;

	std::mt19937 numberGeneratorEngine;
	std::uniform_real_distribution<> distributionEngine;
public:
	NumericRandomizer() : NumericRandomizer((_T)0, (_T)1) { }
	NumericRandomizer(_T minimum, _T maximum) {
		this->minimumValue = minimum;
		this->maximumValue = maximum;
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		numberGeneratorEngine = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
		distributionEngine = std::uniform_real_distribution<>(0.0, 1.0);
	}
	virtual ~NumericRandomizer() {

	}
	__inline const _T getMinimumBoundry() const {
		return minimumValue;
	}
	__inline const _T getMaximumBoundry() const {
		return maximumValue;
	}
	__inline void setNewBoundries(const _T newMinimum, const _T newMaximum) {
		minimumValue = newMinimum;
		maximumValue = newMaximum;
		if (maximumValue < minimumValue) {
			maximumValue = minimumValue;
		}
	}

	__inline _T generateRandomValue() {
		return minimumValue + static_cast<_T>(distributionEngine(numberGeneratorEngine) * (maximumValue - minimumValue));
	}
};

#endif //__ROSE_RANDOMIZER__