#pragma once

namespace nvc {

// http://www.pcg-random.org/download.html
class Pcg {
public:
	Pcg() {}

	Pcg(uint64_t state, uint64_t inc)
		: state { state }
		, inc { inc }
	{}

	void setState(uint64_t s, uint64_t i) {
		this->state = s;
		this->inc = i;
	}

	void getState(uint64_t& s, uint64_t& i) const {
		s = this->state;
		i = this->inc;
	}

	uint32_t getUint32() {
		const uint64_t oldstate = this->state;

		// Advance internal state
		this->state = oldstate * 6364136223846793005ULL + (this->inc|1);

		// Calculate output function (XSH RR), uses old state for max ILP
		const uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
		const int32_t rot = static_cast<int32_t>(oldstate >> 59u);
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	uint64_t getUint64() {
		const auto v0 = static_cast<uint64_t>(getUint32());
		const auto v1 = static_cast<uint64_t>(getUint32());
		return (v0 << 32) | v1;
	}

protected:
	uint64_t state = 1;
	uint64_t inc = 1;
};

} // namespace nvc
