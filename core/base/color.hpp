// this file is from KlayGE
#pragma once
// RGBA����4����������ʾr, g, b, a
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
		Color_T(Color_T&& rhs) noexcept;
		Color_T(T r, T g, T b, T a) noexcept;
		explicit Color_T(uint32_t dw) noexcept;

		const T &operator[](size_t index) noexcept
		{
			return col_[index];
		}
		T &operator[](size_t index) const noexcept
		{
			return col_[index];
		}

		void RGBA(uint8_t& R, uint8_t& G, uint8_t& B, uint8_t& A) const noexcept;

		uint32_t ARGB() const noexcept;
		uint32_t ABGR() const noexcept;

		// ��ֵ������
		Color_T& operator+=(Color_T<T> const & rhs) noexcept;
		Color_T& operator-=(Color_T<T> const & rhs) noexcept;
		Color_T& operator*=(T rhs) noexcept;
		Color_T& operator*=(Color_T<T> const & rhs) noexcept;
		Color_T& operator/=(T rhs) noexcept;

		Color_T& operator=(Color_T const & rhs) noexcept;
		Color_T& operator=(Color_T&& rhs) noexcept;

		// һԪ������
		Color_T const operator+() const noexcept;
		Color_T const operator-() const noexcept;

		bool operator==(Color_T<T> const & rhs) const noexcept;
	};
}