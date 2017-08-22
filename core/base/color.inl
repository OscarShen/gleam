#include "color.hpp"
#include "math.h"
namespace gleam {
	template Color_T<float>::Color_T(float const * rhs) noexcept;
	template Color_T<float>::Color_T(Color const & rhs) noexcept;
	template Color_T<float>::Color_T(Color&& rhs) noexcept;
	template Color_T<float>::Color_T(float r, float g, float b, float a) noexcept;
	template Color_T<float>::Color_T(uint32_t dw) noexcept;
	template void Color_T<float>::RGBA(uint8_t& R, uint8_t& G, uint8_t& B, uint8_t& A) const noexcept;
	template uint32_t Color_T<float>::ARGB() const noexcept;
	template uint32_t Color_T<float>::ABGR() const noexcept;
	template Color& Color_T<float>::operator+=(Color const & rhs) noexcept;
	template Color& Color_T<float>::operator-=(Color const & rhs) noexcept;
	template Color& Color_T<float>::operator*=(float rhs) noexcept;
	template Color& Color_T<float>::operator*=(Color const & rhs) noexcept;
	template Color& Color_T<float>::operator/=(float rhs) noexcept;
	template Color& Color_T<float>::operator=(Color const & rhs) noexcept;
	template Color& Color_T<float>::operator=(Color&& rhs) noexcept;
	template Color const Color_T<float>::operator+() const noexcept;
	template Color const Color_T<float>::operator-() const noexcept;
	template bool Color_T<float>::operator==(Color const & rhs) const noexcept;


	template <typename T>
	Color_T<T>::Color_T(T const * rhs) noexcept
	{
		r = *rhs;
		g = *(rhs + 1);
		b = *(rhs + 2);
		a = *(rhs + 3);
	}

	template <typename T>
	Color_T<T>::Color_T(Color_T const & rhs) noexcept
		: r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a)
	{
	}

	template <typename T>
	Color_T<T>::Color_T(Color_T&& rhs) noexcept
		: col_(std::move(rhs.col_))
	{
	}

	template <typename T>
	Color_T<T>::Color_T(T r, T g, T b, T a) noexcept
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	template <typename T>
	Color_T<T>::Color_T(uint32_t dw) noexcept
	{
		static T const f(1 / T(255));
		this->a = f * (static_cast<T>(static_cast<uint8_t>(dw >> 24)));
		this->r = f * (static_cast<T>(static_cast<uint8_t>(dw >> 16)));
		this->g = f * (static_cast<T>(static_cast<uint8_t>(dw >> 8)));
		this->b = f * (static_cast<T>(static_cast<uint8_t>(dw >> 0)));
	}

	template <typename T>
	void Color_T<T>::RGBA(uint8_t& R, uint8_t& G, uint8_t& B, uint8_t& A) const noexcept
	{
		R = static_cast<uint8_t>(clamp(this->r, T(0), T(1)) * 255 + 0.5f);
		G = static_cast<uint8_t>(clamp(this->g, T(0), T(1)) * 255 + 0.5f);
		B = static_cast<uint8_t>(clamp(this->b, T(0), T(1)) * 255 + 0.5f);
		A = static_cast<uint8_t>(clamp(this->a, T(0), T(1)) * 255 + 0.5f);
	}

	template <typename T>
	uint32_t Color_T<T>::ARGB() const noexcept
	{
		uint8_t r, g, b, a;
		this->RGBA(r, g, b, a);
		return (a << 24) | (r << 16) | (g << 8) | (b << 0);
	}

	template <typename T>
	uint32_t Color_T<T>::ABGR() const noexcept
	{
		uint8_t r, g, b, a;
		this->RGBA(r, g, b, a);
		return (a << 24) | (b << 16) | (g << 8) | (r << 0);
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator+=(Color_T<T> const & rhs) noexcept
	{
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		a += rhs.a;
		return *this;
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator-=(Color_T<T> const & rhs) noexcept
	{
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		a -= rhs.a;
		return *this;
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator*=(T rhs) noexcept
	{
		r *= rhs;
		g *= rhs;
		b *= rhs;
		a *= rhs;
		return *this;
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator*=(Color_T<T> const & rhs) noexcept
	{
		r *= rhs.r;
		g *= rhs.g;
		b *= rhs.b;
		a *= rhs.a;
		return *this;
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator/=(T rhs) noexcept
	{
		r /= rhs;
		g /= rhs;
		b /= rhs;
		a /= rhs;
		return *this;
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator=(Color_T<T> const & rhs) noexcept
	{
		if (this != &rhs)
		{
			r = rhs.r;
			g = rhs.g;
			b = rhs.b;
			a = rhs.a;
		}
		return *this;
	}

	template <typename T>
	Color_T<T>& Color_T<T>::operator=(Color_T<T>&& rhs) noexcept
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	template <typename T>
	Color_T<T> const Color_T<T>::operator+() const noexcept
	{
		return *this;
	}

	template <typename T>
	Color_T<T> const Color_T<T>::operator-() const noexcept
	{
		return Color_T(-this->r, -this->g, -this->b, -this->a);
	}

	template <typename T>
	bool Color_T<T>::operator==(Color_T<T> const & rhs) const noexcept
	{
		return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
	}
}