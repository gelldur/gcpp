//
// Created by Dawid Drozd aka Gelldur on 03.12.17.
//
#pragma once

#include <concepts>
#include <format>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace gcpp::math
{

template <class T>
class Percent;

namespace detail
{

template <class>
inline constexpr bool IS_PERCENT = false;

template <class T>
inline constexpr bool IS_PERCENT<Percent<T>> = true;

} // namespace detail

/**
 * @brief Any type that is not a Percent specialization (cv/ref ignored).
 *
 * Keeps the "apply percent to an amount" operators away from Percent-with-Percent arithmetic,
 * also for mixed specializations like `Percent<double> * Percent<float>`.
 */
template <class O>
concept NotPercent = !detail::IS_PERCENT<std::remove_cvref_t<O>>;

/**
 * @brief Ratio kept as an exact fraction `nominator / denominator`.
 *
 * `Percent{12.1}` is 12.1 % stored as 12.1/100, so `value() == 0.121` and `percentValue() == 12.1`.
 *
 * Applying to an amount (result has the amount's type):
 * @code
 * const Percent fee{2.0};
 * 150.0 * fee; // 3.0   (same as fee * 150.0)
 * 150.0 + fee; // 153.0
 * 150.0 - fee; // 147.0
 * @endcode
 * Scaling the percent itself (2 * 10 % = 20 %) is done with gcpp::math::multiply().
 *
 * @tparam T floating point or decimal-like type.
 * @warning Integral T truncates value(): `Percent<int>{12}.value() == 0`.
 */
template <class T = double>
class [[nodiscard]] Percent
{
public:
	/// 0 %
	explicit constexpr Percent() = default;

	/// @param nominator e.g. 1 and @p denominator 3 gives 33.(3) %
	/// @throws std::invalid_argument when @p denominator == 0 (checked before dividing)
	explicit constexpr Percent(T nominator, T denominator)
		: _nominator(std::move(nominator))
		, _denominator(std::move(denominator))
		, _value(checkedRatio(_nominator, _denominator))
	{ }

	/// @param percentValue e.g. 12.1 for 12.1 %
	explicit constexpr Percent(T percentValue)
		: _nominator(std::move(percentValue))
		, _denominator(100)
		, _value(_nominator / _denominator)
	{ }

	/// Converting a non-zero denominator to T can yield 0 (e.g. 0.4 -> int), so it is checked
	/// again.
	template <class E>
	explicit constexpr Percent(const Percent<E>& other)
		: _nominator(static_cast<T>(other.getNominator()))
		, _denominator(static_cast<T>(other.getDenominator()))
		, _value(checkedRatio(_nominator, _denominator))
	{ }

	template <class E>
	constexpr Percent& operator=(const Percent<E>& other)
	{
		*this = Percent{other};
		return *this;
	}

	/// @return ratio, e.g. 0.121 for 12.1 %
	[[nodiscard]] constexpr const T& value() const
	{
		return _value;
	}

	/// @return e.g. 12.1 for 12.1 %
	[[nodiscard]] constexpr T percentValue() const
	{
		if(_denominator == 100)
		{
			// Exact: going through value() would turn 12.1 into 12.100000000000001
			return _nominator;
		}
		// Multiply first: one rounding instead of two (1/3 gives 33.333333333333336)
		return (_nominator * 100) / _denominator;
	}

	[[nodiscard]] constexpr const T& getNominator() const
	{
		return _nominator;
	}

	[[nodiscard]] constexpr const T& getDenominator() const
	{
		return _denominator;
	}

	[[nodiscard]] constexpr bool isZero() const
	{
		return _nominator == 0;
	}

	/// `!=` is synthesized from this one (C++20 rewritten candidates).
	constexpr bool operator==(const Percent& rhs) const
	{
		if(_denominator == rhs._denominator)
		{
			return _nominator == rhs._nominator;
		}

		return value() == rhs.value();
	}

	constexpr bool operator<(const Percent& rhs) const
	{
		return value() < rhs.value();
	}

	constexpr bool operator>(const Percent& rhs) const
	{
		return rhs < *this;
	}

	constexpr bool operator<=(const Percent& rhs) const
	{
		return !(rhs < *this);
	}

	constexpr bool operator>=(const Percent& rhs) const
	{
		return !(*this < rhs);
	}

	/// Part of @p amount, e.g. `Percent{2.0} * 150.0 == 3.0`. Commutative with `amount * percent`.
	template <NotPercent O>
	constexpr O operator*(const O& amount) const
	{
		return applyTo(amount);
	}

	/// Part of @p amount, e.g. `150.0 * Percent{2.0} == 3.0`
	template <NotPercent O>
	friend constexpr O operator*(const O& amount, const Percent& percent)
	{
		return percent.applyTo(amount);
	}

	/// @p amount decreased by @p percent, e.g. `150.0 - Percent{2.0} == 147.0`
	template <NotPercent O>
	friend constexpr O operator-(const O& amount, const Percent& percent)
	{
		if(percent.isZero())
		{
			return amount;
		}

		return amount - percent.applyTo(amount);
	}

	/// @p amount increased by @p percent, e.g. `150.0 + Percent{2.0} == 153.0`
	template <NotPercent O>
	friend constexpr O operator+(const O& amount, const Percent& percent)
	{
		if(percent.isZero())
		{
			return amount;
		}

		return amount + percent.applyTo(amount);
	}

	/// Exact fraction sum: 12.1 % + 1 % keeps denominator 100 and gives exactly 13.1 %.
	constexpr Percent operator+(const Percent& rhs) const
	{
		if(_denominator == rhs._denominator)
		{
			return Percent{_nominator + rhs._nominator, _denominator};
		}

		return Percent{_nominator * rhs._denominator + rhs._nominator * _denominator,
					   _denominator * rhs._denominator};
	}

	/// Exact fraction difference, see operator+.
	constexpr Percent operator-(const Percent& rhs) const
	{
		if(_denominator == rhs._denominator)
		{
			return Percent{_nominator - rhs._nominator, _denominator};
		}

		return Percent{_nominator * rhs._denominator - rhs._nominator * _denominator,
					   _denominator * rhs._denominator};
	}

	/// Percent of a percent: 10 % * 10 % == 1 %
	constexpr Percent operator*(const Percent& rhs) const
	{
		return Percent{_nominator * rhs._nominator, _denominator * rhs._denominator};
	}

	/// Ratio of percents: 50 % / 25 % == 200 %
	/// @throws std::invalid_argument when @p rhs is 0 %
	constexpr Percent operator/(const Percent& rhs) const
	{
		return Percent{_nominator * rhs._denominator, _denominator * rhs._nominator};
	}

	constexpr Percent operator-() const
	{
		return Percent{-_nominator, _denominator};
	}

	/// Change relative to 100 %, e.g. 110 % -> 10 %, 95 % -> -5 %
	constexpr Percent diff() const
	{
		return Percent{_nominator - _denominator, _denominator};
	}

	/**
	 * Honors the stream's flags, precision and locale for the number, while width, fill and
	 * left/right apply to the whole text.
	 * @code
	 * std::cout << std::left << std::setw(11) << Percent{12.1}; // "12.1%      "
	 * @endcode
	 */
	template <class CharT, class Traits>
	friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& stream,
														 const Percent& percent)
	{
		std::basic_ostringstream<CharT, Traits> text;
		text.flags(stream.flags());
		text.precision(stream.precision());
		text.imbue(stream.getloc());
		text << percent.percentValue() << text.widen('%');
		// Inserting a string pads it as one unit and resets the stream width afterwards
		return stream << std::move(text).str();
	}

	/// @return 0 % instead of throwing when @p denominator == 0
	static constexpr Percent ZeroSafe(const T& nominator, const T& denominator)
	{
		if(denominator == 0)
		{
			return Percent{};
		}

		return Percent{nominator, denominator};
	}

private:
	T _nominator{};
	T _denominator{100};
	T _value{}; // Fast cache of _nominator / _denominator

	static constexpr T checkedRatio(const T& nominator, const T& denominator)
	{
		if(denominator == 0)
		{
			throw std::invalid_argument("Denominator can't be == 0");
		}

		return nominator / denominator;
	}

	/// Multiplies before dividing to keep precision for decimal types (no 1/3 cache rounding).
	template <NotPercent O>
	constexpr O applyTo(const O& amount) const
	{
		return static_cast<O>((amount * _nominator) / _denominator);
	}
};

/// Scales the percent itself: `multiply(2, Percent{10.0}) == Percent{20.0}`
template <class T, class O>
constexpr Percent<T> multiply(const O& lhs, const Percent<T>& rhs)
{
	return Percent<T>{lhs * rhs.getNominator(), rhs.getDenominator()};
}

inline namespace percent_literals
{

Percent<long double> operator""_bps(long double);
Percent<long double> operator""_bps(unsigned long long int);

} // namespace percent_literals

} // namespace gcpp::math

/**
 * @brief std::format support for gcpp::math::Percent: formats percentValue() with
 * `std::formatter<T>` and appends '%'.
 *
 * The whole spec belongs to T, so width and fill pad the number only, '%' comes after them.
 * @code
 * const Percent p{12.1};
 * std::format("{}", p);      // "12.1%"
 * std::format("{:.2f}", p);  // "12.10%"
 * std::format("{:>8}", p);   // "    12.1%"
 * std::format("{:<8}", p);   // "12.1    %"
 * @endcode
 */
template <class T, class CharT>
struct std::formatter<gcpp::math::Percent<T>, CharT> : std::formatter<T, CharT>
{
	template <class FormatContext>
	auto format(const gcpp::math::Percent<T>& percent, FormatContext& ctx) const
	{
		auto out = std::formatter<T, CharT>::format(percent.percentValue(), ctx);
		*out++ = CharT('%');
		return out;
	}
};
