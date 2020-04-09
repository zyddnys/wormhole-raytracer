#pragma once
#include "common.h"

#include <string_view>
#include <vector>
#include <atlbase.h>
#include <wincodec.h>
#include <cstdio>


#include "Bitmap.h"
#include "float32x4.h"

struct Texture : Bitmap<float32x4>
{
	template<typename... Args>
	Texture(Args&&... args) :Bitmap<float32x4>(std::forward<Args>(args)...)
	{
		;
	}
	Texture(std::wstring_view filename)
	{
		Load(filename);
	}

	void Load(std::wstring_view filename)
	{
		CComPtr<IWICImagingFactory> pFactory;
		CComPtr<IWICBitmapDecoder> pDecoder;
		CComPtr<IWICBitmapFrameDecode> pFrame;
		CComPtr<IWICFormatConverter> pFormatConverter;

		CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID *)&pFactory);

		pFactory->CreateDecoderFromFilename(filename.data(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
		// Check the return value to see if:
		//  - The specified file exists and can be read.
		//  - A decoder is found for this file format.

		UINT frameCount = 0;
		pDecoder->GetFrameCount(&frameCount);

		pDecoder->GetFrame(0, &pFrame);
		// The zero-based index should be smaller than the frame count.

		std::uint32_t w, h;
		pFrame->GetSize(&w, &h);
		width = w, height = h;

		WICPixelFormatGUID pixelFormatGUID;
		pFrame->GetPixelFormat(&pixelFormatGUID);

		// The frame can use many different pixel formats.
		// You can copy the raw pixel values by calling "pFrame->CopyPixels( )".
		// This method needs a buffer that can hold all bytes.
		// The total number of bytes is: width x height x bytes per pixel

		// The disadvantage of this solution is that you have to deal with all possible pixel formats.

		// You can make your life easy by converting the frame to a pixel format of
		// your choice. The code below shows how to convert the pixel format to 24-bit RGB.

		pFactory->CreateFormatConverter(&pFormatConverter);

		pFormatConverter->Initialize(pFrame,                       // Input bitmap to convert
			GUID_WICPixelFormat24bppRGB,  // Destination pixel format
			WICBitmapDitherTypeNone,      // Specified dither pattern
			nullptr,                      // Specify a particular palette
			0.f,                          // Alpha threshold
			WICBitmapPaletteTypeCustom); // Palette translation type

		UINT bytesPerPixel = 3; // Because we have converted the frame to 24-bit RGB
		UINT stride = width * bytesPerPixel;
		UINT size = width * height * bytesPerPixel; // The size of the required memory buffer for
													// holding all the bytes of the frame.

		std::vector<BYTE> bitmap(size); // The buffer to hold all the bytes of the frame.
		pFormatConverter->CopyPixels(NULL, stride, size, bitmap.data());

		// Note: the WIC COM pointers should be released before 'CoUninitialize( )' is called.
		data = new float32x4[width * height];

		float32x4 *dst(data);
		BYTE const *src = bitmap.data();
		for (std::size_t i(0); i < width * height; ++i, src += 3, ++dst)
		{
			dst->x = static_cast<float>(src[0]) / 255.0f;
			dst->y = static_cast<float>(src[1]) / 255.0f;
			dst->z = static_cast<float>(src[2]) / 255.0f;
			dst->w = 1.0f;
		}
	}

	void Save(std::wstring_view filename, bool normalize = false)
	{
		using namespace std;
		FILE *pFile = _wfopen(filename.data(), L"wb");
		//assert( pFile  != NULL );

		int bmp_size = width * height * (32 / 8);

		// 【写位图文件头】
		BITMAPFILEHEADER bmpHeader;
		bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp_size;    // BMP图像文件的大小
		bmpHeader.bfType = 0x4D42;    // 位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’
		bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);            // BMP图像数据的偏移位置
		bmpHeader.bfReserved1 = 0;    // 总为0
		bmpHeader.bfReserved2 = 0;    // 总为0
		fwrite(&bmpHeader, sizeof(bmpHeader), 1, pFile);

		BITMAPINFOHEADER bmiHeader;
		bmiHeader.biSize = sizeof(bmiHeader);                // 本结构所占用字节数，即sizeof(BITMAPINFOHEADER);
		bmiHeader.biWidth = width;                            // 位图宽度（单位：像素）
		bmiHeader.biHeight = height;                        // 位图高度（单位：像素）
		bmiHeader.biPlanes = 1;                                // 目标设备的级别，必须为1
		bmiHeader.biBitCount = 32;                    // 像素的位数（每个像素所需的位数，范围：1、4、8、24、32）
		bmiHeader.biCompression = 0;                        // 压缩类型（0：不压缩 1：BI_RLE8压缩类型 2：BI_RLE4压缩类型）
		bmiHeader.biSizeImage = bmp_size;                    // 位图大小（单位：字节）
		bmiHeader.biXPelsPerMeter = 0;                        // 水平分辨率(像素/米)
		bmiHeader.biYPelsPerMeter = 0;                        // 垂直分辨率(像素/米)
		bmiHeader.biClrUsed = 0;                            // 位图实际使用的彩色表中的颜色索引数
		bmiHeader.biClrImportant = 0;                        // 对图象显示有重要影响的颜色索引的数目

															 // 【写位图信息头（BITMAPINFO的bmiHeader成员）】
		fwrite(&bmiHeader, sizeof(bmiHeader), 1, pFile);

		float max_r = -10000.0f, max_g = -10000.0f, max_b = -10000.0f, max_a = -10000.0f;
		float min_r = 10000.0f, min_g = 10000.0f, min_b = 10000.0f, min_a = 10000.0f;

		std::size_t i;
		std::size_t total(width * height);

		for (i = 0; i < total; i++) max_r = std::max(max_r, data[i].x);
		for (i = 0; i < total; i++) max_g = std::max(max_g, data[i].y);
		for (i = 0; i < total; i++) max_b = std::max(max_b, data[i].z);
		for (i = 0; i < total; i++) max_a = std::max(max_a, data[i].w);

		for (i = 0; i < total; i++) min_r = std::min(min_r, data[i].x);
		for (i = 0; i < total; i++) min_g = std::min(min_g, data[i].y);
		for (i = 0; i < total; i++) min_b = std::min(min_b, data[i].z);
		for (i = 0; i < total; i++) min_a = std::min(min_a, data[i].w);

		float rgba_diff[4] = {
			max_r - min_r,
			max_g - min_g,
			max_b - min_b,
			max_a - min_a
		};

		float rgba_min[4] = {
			min_r,
			min_g,
			min_b,
			min_a
		};

		if (normalize) {
			__m128 diff_value = _mm_load_ps(rgba_diff);
			__m128 min_value = _mm_load_ps(rgba_min);
			vector<std::uint32_t> data_bytes(width * height);
			uint8_t *dst(static_cast<uint8_t *>(static_cast<void *>(data_bytes.data())));
			for (i = 0; i < total; ++i, dst += 4)
			{
				const auto &s = data[i];
				__m128 tmp = _mm_sub_ps(s.vec, min_value);
				tmp = _mm_div_ps(tmp, diff_value);

				dst[2] = float2byte(tmp.m128_f32[0]);
				dst[1] = float2byte(tmp.m128_f32[1]);
				dst[0] = float2byte(tmp.m128_f32[2]);
				dst[3] = float2byte(tmp.m128_f32[3]);
			}

			// 【写像素内容】
			fwrite(data_bytes.data(), bmp_size, 1, pFile);
		}
		else {
			vector<std::uint32_t> data_bytes(width * height);
			uint8_t *dst(static_cast<uint8_t *>(static_cast<void *>(data_bytes.data())));
			for (size_t i{ 0 }; i < width * height; ++i, dst += 4)
			{
				float32x4 const &src(data[i]);
				// BGRA
				dst[2] = float2byte(src.x);
				dst[1] = float2byte(src.y);
				dst[0] = float2byte(src.z);
				dst[3] = float2byte(src.w);
			}

			// 【写像素内容】
			fwrite(data_bytes.data(), bmp_size, 1, pFile);
		}
		fclose(pFile);
	}

	template<typename T>
	float32x4 Sample(T u, T v) // bilinear sample
	{
		u = saturate(u) * static_cast<T>(width);
		v = saturate(v) * static_cast<T>(height);

		std::size_t ui = static_cast<std::size_t>(u + 0.5);
		std::size_t vi = static_cast<std::size_t>(v + 0.5);

		T diff_u = u - ui;
		T diff_v = v - vi;

		float32x4 u00 = Load(ui - 1, vi - 1);
		float32x4 u01 = Load(ui - 1, vi);
		float32x4 u10 = Load(ui, vi - 1);
		float32x4 u11 = Load(ui, vi);

		T s = diff_u + 0.5;
		T t = diff_v + 0.5;

		float32x4 u0 = lerp(s, u00, u10);
		float32x4 u1 = lerp(s, u01, u11);
		return lerp(t, u0, u1);
	}

	float32x4 Load(std::size_t u, std::size_t v)
	{
		u = clamp(u, 0ull, width - 1);
		v = clamp(v, 0ull, height - 1);
		return operator()(v, u);
	}
};
