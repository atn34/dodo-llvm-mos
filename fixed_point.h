#pragma once

template <int kShiftAmount, class IntType>
struct FixedPoint {

	FixedPoint() : shiftedAmount(0) {}
	FixedPoint(IntType v) : shiftedAmount(v << kShiftAmount) {}

	FixedPoint<kShiftAmount, IntType> operator+(FixedPoint<kShiftAmount, IntType> rhs) const {
		FixedPoint result;
		result.shiftedAmount = shiftedAmount + rhs.shiftedAmount;
		return result;
	}

	FixedPoint<kShiftAmount, IntType> operator-(FixedPoint<kShiftAmount, IntType> rhs) const {
		FixedPoint result;
		result.shiftedAmount = shiftedAmount - rhs.shiftedAmount;
		return result;
	}

	FixedPoint<kShiftAmount, IntType> operator*(FixedPoint<kShiftAmount, IntType> rhs) const {
		FixedPoint result;
		result.shiftedAmount = (shiftedAmount * rhs.shiftedAmount) >> kShiftAmount;
		return result;
	}

	FixedPoint<kShiftAmount, IntType> operator/(FixedPoint<kShiftAmount, IntType> rhs) const {
		FixedPoint result;
		result.shiftedAmount = (shiftedAmount / rhs.shiftedAmount) << kShiftAmount;
		return result;
	}

	IntType asInt() const {
		return shiftedAmount >> kShiftAmount;
	}

	FixedPoint<kShiftAmount, IntType> operator+=(FixedPoint<kShiftAmount, IntType> rhs){
		shiftedAmount += rhs.shiftedAmount;
		return *this;
	}

	FixedPoint<kShiftAmount, IntType> operator-=(FixedPoint<kShiftAmount, IntType> rhs) const {
		shiftedAmount -= rhs.shiftedAmount;
		*this;
	}
private:
	IntType shiftedAmount;
};

using Real = FixedPoint<2, int>;
