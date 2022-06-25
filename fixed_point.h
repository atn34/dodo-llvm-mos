#pragma once

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
