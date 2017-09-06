// this file is from KlayGE
#pragma once
// RGBA，用4个浮点数表示r, g, b, a
///////////////////////////////////////////////////////////////////////////////
#include <gleam.h>
namespace gleam {
	template <typename T>
	class Color_T final
	{
	public:
		enum { elem_num = 4 };

		T r, g, b, a;
	public:
		Color_T() noexcept
		{
		}
		explicit Color_T(T const * rhs) noexcept;
		Color_T(Color_T const & rhs) noexcept;
		Color_T(T r, T g, T b, T a) noexcept;
		explicit Color_T(uint32_t dw) noexcept;

		const T &operator[](size_t index) const noexcept
		{
			assert((index >= 0) && (index < 4));
			return index == 0 ? r : (index == 1 ? g : (index == 2 ? b : a));
		}
		T &operator[](size_t index) noexcept
		{
			assert((index >= 0) && (index < 4));
			return index == 0 ? r : (index == 1 ? g : (index == 2 ? b : a));
		}

		void RGBA(uint8_t& R, uint8_t& G, uint8_t& B, uint8_t& A) const noexcept;

		uint32_t ARGB() const noexcept;
		uint32_t ABGR() const noexcept;

		// 赋值操作符
		Color_T& operator+=(Color_T<T> const & rhs) noexcept;
		Color_T& operator-=(Color_T<T> const & rhs) noexcept;
		Color_T& operator*=(T rhs) noexcept;
		Color_T& operator*=(Color_T<T> const & rhs) noexcept;
		Color_T& operator/=(T rhs) noexcept;

		Color_T& operator=(Color_T const & rhs) noexcept;
		Color_T& operator=(Color_T&& rhs) noexcept;

		// 一元操作符
		Color_T const operator+() const noexcept;
		Color_T const operator-() const noexcept;

		bool operator==(Color_T<T> const & rhs) const noexcept;
		bool operator!=(Color_T<T> const & rhs) const noexcept;
	};
}