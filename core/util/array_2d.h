#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <stdint.h>

inline int32_t mini(const int32_t a, const int32_t b)
{
	return (a < b) ? a : b;
}

inline int32_t maxi(const int32_t a, const int32_t b)
{
	return (a > b) ? a : b;
}

inline int32_t Clamp(int32_t x, int32_t a, int32_t b)
{
	return mini(maxi(x, a), b);
}

// 2d array
template <class T>
class Array2D
{
public:
	typedef T value_type;

	Array2D()
	{
		data = NULL;
		rowPointers = NULL;
		w = h = 0;
		border = 0;
	}

	Array2D(int32_t width, int32_t height)
	{
		data = NULL;
		rowPointers = NULL;
		Init(width, height);
	}

	void Init(int32_t width, int32_t height)
	{
		if (data) delete[] data;
		if (rowPointers) delete[] rowPointers;

		w = width;
		h = height;
		data = new T[w*h];
		rowPointers = new T*[h];
		for (int32_t i = 0; i<h; i++)
			rowPointers[i] = &data[i*w];
		border = 0;
	}

	// create from external data
	Array2D(T *ptr, int32_t width, int32_t height)
	{
		w = width;
		h = height;
		data = ptr;
		border = 0;
	}

	~Array2D()
	{
		delete[] rowPointers;
		delete[] data;
	}

	T Get(int32_t x, int32_t y) const
	{
		//return data[y*w+x];
		return rowPointers[y][x];
	}

	T & Get(int32_t x, int32_t y)
	{
		//return data[y*w+x];
		return rowPointers[y][x];
	}

	// clamps to border
	T & GetBorder(int32_t x, int32_t y)
	{
		if ((x < 0) || (y < 0) || (x > w - 1) || (y > h - 1)) {
			return border;
		}
		else {
			//return data[y*w+x];
			return Get(x, y);
		}
	}

	// clamps to edge
	T & GetClamp(int32_t x, int32_t y)
	{
		x = Clamp(x, 0, w - 1);
		y = Clamp(y, 0, h - 1);
		//return data[y*w+x];
		return Get(x, y);
	}

	T Lerp(T a, T b, float t)
	{
		return a + t*(b - a);
	}

	void Set(int32_t x, int32_t y, T v)
	{
		//data[y*w+x] = v;
		rowPointers[y][x] = v;
	}

	void SetSafe(int32_t x, int32_t y, T v)
	{
		if ((x >= 0) && (y >= 0) && (x < w) && (y < h)) {
			//data[y*w+x] = v;
			rowPointers[y][x] = v;
		}
	}

	T * GetData()
	{
		return data;
	}

public:
	int32_t w, h;
	T border;
	T **rowPointers;
	T *data;
};

#endif
