#include "api.h"

template <int kShiftAmount, class IntType>
struct FixedPoint {

	FixedPoint(IntType v) : shiftedAmount(v << kShiftAmount) {}

	FixedPoint<kShiftAmount, IntType> operator+(FixedPoint<kShiftAmount, IntType> rhs) {
		FixedPoint result;
		result.shiftedAmount = shiftedAmount + rhs.shiftedAmount;
		return result;
	}

	FixedPoint<kShiftAmount, IntType> operator-(FixedPoint<kShiftAmount, IntType> rhs) {
		FixedPoint result;
		result.shiftedAmount = shiftedAmount - rhs.shiftedAmount;
		return result;
	}

	FixedPoint<kShiftAmount, IntType> operator*(FixedPoint<kShiftAmount, IntType> rhs) {
		FixedPoint result;
		result.shiftedAmount = (shiftedAmount * rhs.shiftedAmount) >> kShiftAmount;
		return result;
	}

	FixedPoint<kShiftAmount, IntType> operator/(FixedPoint<kShiftAmount, IntType> rhs) {
		FixedPoint result;
		result.shiftedAmount = (shiftedAmount / rhs.shiftedAmount) << kShiftAmount;
		return result;
	}

	IntType asInt() const {
		return shiftedAmount >> kShiftAmount;
	}

private:
	FixedPoint() {}
	IntType shiftedAmount;
};

using Real = FixedPoint<2, int>;

// Use dodo_main instead of main so that we don't need extra code to return a value.
// void main is illegal in c++
extern "C" void dodo_main() {
	LED_ON();
	DELAY_MS(Real(255).asInt());
	LED_OFF();
}
