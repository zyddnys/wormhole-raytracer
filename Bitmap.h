#pragma once

#include <memory>

template<typename T>
struct Bitmap
{
	T *data;
	std::size_t width, height;

	Bitmap() :data(nullptr), width(0), height(0)
	{
		;
	}

	Bitmap(std::size_t width, std::size_t height)
	{
		data = new T[width * height];
		this->width = width;
		this->height = height;
	}

	void Release() noexcept
	{
		try
		{
			if (data) {
				delete[] data;
			}
			width = height = 0;
		}
		catch (...)
		{
			;
		}
	}

	~Bitmap() noexcept
	{
		Release();
	}

	Bitmap(Bitmap const &a) :
		data(a.data),
		width(a.width),
		height(a.height)
	{
		data = new T[width * height];
		std::uninitialized_copy(a.data, a.data + width * height, data);
	}

	Bitmap &operator=(Bitmap const &a)
	{
		if (std::addressof(a) != this)
		{
			std::unique_ptr<T> tmp(new T[a.width * a.height]);
			std::uninitialized_copy(a.data, a.data + width * height, tmp);
			Release();
			data = tmp.release();
		}
		return *this;
	}

	Bitmap(Bitmap &&a) noexcept
	{
		data = a.data;
		width = a.width;
		height = a.height;
		a.data = nullptr;
		a.width = 0;
		a.height = 0;
	}

	Bitmap &operator=(Bitmap &&a) noexcept
	{
		if (std::addressof(a) != this)
		{
			Release();
			data = a.data;
			width = a.width;
			height = a.height;
			a.data = nullptr;
			a.width = 0;
			a.height = 0;
		}
		return *this;
	}

	T const &operator()(std::size_t i, std::size_t j) const
	{
		return data[i * width + j];
	}

	T &operator()(std::size_t i, std::size_t j)
	{
		return data[i * width + j];
	}
};
