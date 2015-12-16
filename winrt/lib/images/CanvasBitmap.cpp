// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <propkey.h>

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Storage::Streams;
    using namespace ABI::Windows::Storage;
    using namespace ::Microsoft::WRL::Wrappers;

#if WINVER > _WIN32_WINNT_WINBLUE
    using ABI::Windows::Graphics::Imaging::BitmapPixelFormat;
#endif

    //
    // Block compressed formats have a block size of 4x4 pixels.
    //
    // For other formats that work in pixels we treat the "block" size as 1x1.
    //
    static unsigned GetBlockSize(DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return 4;

        default:
            return 1;
        }
    }

    
    static unsigned GetBytesPerBlock(DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS: return 16;
        case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
        case DXGI_FORMAT_R32G32B32A32_UINT: return 16;
        case DXGI_FORMAT_R32G32B32A32_SINT: return 16;
        case DXGI_FORMAT_R32G32B32_TYPELESS: return 12;
        case DXGI_FORMAT_R32G32B32_FLOAT: return 12;
        case DXGI_FORMAT_R32G32B32_UINT: return 12;
        case DXGI_FORMAT_R32G32B32_SINT: return 12;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return 8;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;
        case DXGI_FORMAT_R16G16B16A16_UNORM: return 8;
        case DXGI_FORMAT_R16G16B16A16_UINT: return 8;
        case DXGI_FORMAT_R16G16B16A16_SNORM: return 8;
        case DXGI_FORMAT_R16G16B16A16_SINT: return 8;
        case DXGI_FORMAT_R32G32_TYPELESS: return 8;
        case DXGI_FORMAT_R32G32_FLOAT: return 8;
        case DXGI_FORMAT_R32G32_UINT: return 8;
        case DXGI_FORMAT_R32G32_SINT: return 8;
        case DXGI_FORMAT_R32G8X24_TYPELESS: return 8;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return 8;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return 8;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return 8;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS: return 4;
        case DXGI_FORMAT_R10G10B10A2_UNORM: return 4;
        case DXGI_FORMAT_R10G10B10A2_UINT: return 4;
        case DXGI_FORMAT_R11G11B10_FLOAT: return 4;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return 4;
        case DXGI_FORMAT_R8G8B8A8_UINT: return 4;
        case DXGI_FORMAT_R8G8B8A8_SNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_SINT: return 4;
        case DXGI_FORMAT_R16G16_TYPELESS: return 4;
        case DXGI_FORMAT_R16G16_FLOAT: return 4;
        case DXGI_FORMAT_R16G16_UNORM: return 4;
        case DXGI_FORMAT_R16G16_UINT: return 4;
        case DXGI_FORMAT_R16G16_SNORM: return 4;
        case DXGI_FORMAT_R16G16_SINT: return 4;
        case DXGI_FORMAT_R32_TYPELESS: return 4;
        case DXGI_FORMAT_D32_FLOAT: return 4;
        case DXGI_FORMAT_R32_FLOAT: return 4;
        case DXGI_FORMAT_R32_UINT: return 4;
        case DXGI_FORMAT_R32_SINT: return 4;
        case DXGI_FORMAT_R24G8_TYPELESS: return 4;
        case DXGI_FORMAT_D24_UNORM_S8_UINT: return 4;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return 4;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return 4;
        case DXGI_FORMAT_R8G8_TYPELESS: return 2;
        case DXGI_FORMAT_R8G8_UNORM: return 2;
        case DXGI_FORMAT_R8G8_UINT: return 2;
        case DXGI_FORMAT_R8G8_SNORM: return 2;
        case DXGI_FORMAT_R8G8_SINT: return 2;
        case DXGI_FORMAT_R16_TYPELESS: return 2;
        case DXGI_FORMAT_R16_FLOAT: return 2;
        case DXGI_FORMAT_D16_UNORM: return 2;
        case DXGI_FORMAT_R16_UNORM: return 2;
        case DXGI_FORMAT_R16_UINT: return 2;
        case DXGI_FORMAT_R16_SNORM: return 2;
        case DXGI_FORMAT_R16_SINT: return 2;
        case DXGI_FORMAT_R8_TYPELESS: return 1;
        case DXGI_FORMAT_R8_UNORM: return 1;
        case DXGI_FORMAT_R8_UINT: return 1;
        case DXGI_FORMAT_R8_SNORM: return 1;
        case DXGI_FORMAT_R8_SINT: return 1;
        case DXGI_FORMAT_A8_UNORM: return 1;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return 4;
        case DXGI_FORMAT_B5G6R5_UNORM: return 2;
        case DXGI_FORMAT_B5G5R5A1_UNORM: return 2;
        case DXGI_FORMAT_B8G8R8A8_UNORM: return 4;
        case DXGI_FORMAT_B8G8R8X8_UNORM: return 4;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return 4;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return 4;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return 4;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS: return 4;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return 4;        
        case DXGI_FORMAT_P8: return 1;
        case DXGI_FORMAT_A8P8: return 2;
        case DXGI_FORMAT_B4G4R4A4_UNORM: return 2;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            return 8;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            return 16;
            
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            return 16;
            
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 8;
            
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
            return 16;

        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
            return 16;

        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return 16;

        default:
            // Some formats such as DXGI_FORMAT_UNKNOWN
            ThrowHR(E_INVALIDARG);
        }
    }


    static void VerifyWellFormedSubrectangle(D2D1_RECT_U subRectangle, D2D1_SIZE_U targetSize)
    {
        if (subRectangle.right <= subRectangle.left ||
            subRectangle.bottom <= subRectangle.top)
        {
            ThrowHR(E_INVALIDARG);
        }

        if (subRectangle.right > targetSize.width || subRectangle.bottom > targetSize.height)
        {
            ThrowHR(E_INVALIDARG);
        }
    }


    static bool IsBlockAligned(unsigned blockSize, D2D1_POINT_2U const& p)
    {
        if (blockSize == 1)
            return true;

        auto IsAligned = [=] (uint32_t coord) { return (coord % blockSize) == 0; };

        return IsAligned(p.x) && IsAligned(p.y);
    }


    static bool IsBlockAligned(unsigned blockSize, D2D1_RECT_U const& r)
    {
        return IsBlockAligned(blockSize, D2D1_POINT_2U{ r.left, r.top })
            && IsBlockAligned(blockSize, D2D1_POINT_2U{ r.right, r.bottom });
    }


    class BitmapSubRectangle
    {
        unsigned m_totalBytes;
        unsigned m_bytesPerRow;
        unsigned m_blocksHigh;
        
    public:
        BitmapSubRectangle(ComPtr<ID2D1Bitmap> const& d2dBitmap, D2D1_RECT_U rect)
        {
            VerifyWellFormedSubrectangle(rect, d2dBitmap->GetPixelSize());

            auto format = d2dBitmap->GetPixelFormat().format;
            auto blockSize = GetBlockSize(format);

            if (!IsBlockAligned(blockSize, rect))
                ThrowHR(E_INVALIDARG, Strings::BlockCompressedSubRectangleMustBeAligned);

            auto bytesPerBlock = GetBytesPerBlock(format);

            auto pixelWidth = (rect.right - rect.left);
            auto blocksWide = pixelWidth / blockSize;

            auto pixelHeight = (rect.bottom - rect.top);
            m_blocksHigh = pixelHeight / blockSize;

            auto totalBlocks = blocksWide * m_blocksHigh;

            m_totalBytes = totalBlocks * bytesPerBlock;
            m_bytesPerRow = blocksWide * bytesPerBlock;
        }

        unsigned GetTotalBytes() const { return m_totalBytes; }
        unsigned GetBytesPerRow() const { return m_bytesPerRow; }
        unsigned GetBlocksHigh() const { return m_blocksHigh; }
    };


    GUID GetGUIDForFileFormat(CanvasBitmapFileFormat fileFormat)
    {
        switch (fileFormat)
        {
            case CanvasBitmapFileFormat::Bmp: return GUID_ContainerFormatBmp;
            case CanvasBitmapFileFormat::Png: return GUID_ContainerFormatPng;
            case CanvasBitmapFileFormat::Jpeg: return GUID_ContainerFormatJpeg;
            case CanvasBitmapFileFormat::Tiff: return GUID_ContainerFormatTiff;
            case CanvasBitmapFileFormat::Gif: return GUID_ContainerFormatGif;
            case CanvasBitmapFileFormat::JpegXR: return GUID_ContainerFormatWmp;
            default:
            {
                ThrowHR(E_INVALIDARG);
            }
        }
    }

    static GUID GetEncoderFromFileExtension(
        std::wstring const& fileName)
    {
        static const struct ExtensionTable
        {
            wchar_t* Extension;
            GUID Guid;
        } extensionTable[] =
        {
            { L"bmp", GUID_ContainerFormatBmp },
            { L"dib", GUID_ContainerFormatBmp },
            { L"rle", GUID_ContainerFormatBmp },
            { L"png", GUID_ContainerFormatPng },
            { L"jpg", GUID_ContainerFormatJpeg },
            { L"jpe", GUID_ContainerFormatJpeg },
            { L"jpeg", GUID_ContainerFormatJpeg },
            { L"jfif", GUID_ContainerFormatJpeg },
            { L"exif", GUID_ContainerFormatJpeg },
            { L"tif", GUID_ContainerFormatTiff },
            { L"tiff", GUID_ContainerFormatTiff },
            { L"gif", GUID_ContainerFormatGif },
            { L"wdp", GUID_ContainerFormatWmp },
            { L"jxr", GUID_ContainerFormatWmp },
        };

        size_t separatorIndex = fileName.find_last_of('.');
        std::wstring fileExtension(fileName.substr(separatorIndex + 1));
        for (auto const& tableEntry : extensionTable)
        {
            if (_wcsicmp(fileExtension.c_str(), tableEntry.Extension) == 0)
            {
                return tableEntry.Guid;
            }
        }
        ThrowHR(E_INVALIDARG, Strings::UnrecognizedImageFileExtension);
    }

    static void SaveLockedMemoryToNativeStream(
        IWICImagingFactory2* wicFactory,
        IWICStream* nativeStream,
        GUID encoderGuid,
        float quality,
        unsigned int width,
        unsigned int height,
        float dpiX,
        float dpiY,
        ScopedBitmapMappedPixelAccess* bitmapLock)
    {
        ComPtr<IWICBitmapEncoder> encoder;
        ThrowIfFailed(wicFactory->CreateEncoder(encoderGuid, NULL, &encoder));

        ThrowIfFailed(encoder->Initialize(nativeStream, WICBitmapEncoderNoCache));

        ComPtr<IWICBitmapFrameEncode> frameEncode;
        ComPtr<IPropertyBag2> frameProperties;
        encoder->CreateNewFrame(&frameEncode, &frameProperties);

        if (quality < 0.0f || quality > 1.0f)
        {
            ThrowHR(E_INVALIDARG);
        }

        bool supportsQualityOption =
            encoderGuid == GUID_ContainerFormatJpeg ||
            encoderGuid == GUID_ContainerFormatWmp;

        if (supportsQualityOption)
        {
            PROPBAG2 option = { 0 };
            option.pstrName = L"ImageQuality";
            VARIANT value;
            value.vt = VT_R4;
            value.fltVal = quality;
            ThrowIfFailed(frameProperties->Write(1, &option, &value));
        }

        ThrowIfFailed(frameEncode->Initialize(frameProperties.Get()));

        ThrowIfFailed(frameEncode->SetSize(width, height));

        // TODO: #2767 Support saving formats other than 32bppBGRA.
        ComPtr<IWICBitmap> memoryBitmap;
        ThrowIfFailed(wicFactory->CreateBitmapFromMemory(
            width,
            height,
            GUID_WICPixelFormat32bppBGRA,
            bitmapLock->GetStride(),
            bitmapLock->GetLockedBufferSize(),
            bitmapLock->GetLockedData(),
            &memoryBitmap));

        WICPixelFormatGUID pixelFormat;
        ThrowIfFailed(memoryBitmap->GetPixelFormat(&pixelFormat));
        ThrowIfFailed(frameEncode->SetPixelFormat(&pixelFormat));

        ThrowIfFailed(frameEncode->SetResolution(dpiX, dpiY));

        ThrowIfFailed(frameEncode->WriteSource(memoryBitmap.Get(), NULL));
        ThrowIfFailed(frameEncode->Commit());
        ThrowIfFailed(encoder->Commit());
    }

    static bool TryEnableIndexing(ComPtr<IWICBitmapFrameDecode> const& frameDecode)
    {
#if WINVER > _WIN32_WINNT_WINBLUE
        auto jpegDecode = MaybeAs<IWICJpegFrameDecode>(frameDecode);

        if (!jpegDecode)
            return false;

        BOOL supportsIndexing;
        ThrowIfFailed(jpegDecode->DoesSupportIndexing(&supportsIndexing));

        if (!supportsIndexing)
            return false;

        auto const indexingGranularity = 64U;
        ThrowIfFailed(jpegDecode->SetIndexing(WICJpegIndexingOptionsGenerateOnLoad, indexingGranularity));

        return true;
#else
        UNREFERENCED_PARAMETER(frameDecode);
        return false;
#endif
    }

    static UINT GetOrientationFromFrameDecode(ComPtr<IWICBitmapFrameDecode> const& frameDecode)
    {
        ComPtr<IWICMetadataQueryReader> queryReader;

        HRESULT getMetadataHr = frameDecode->GetMetadataQueryReader(&queryReader);
        if (getMetadataHr == WINCODEC_ERR_UNSUPPORTEDOPERATION)
        {
            // This is expected for some formats, such as BMP.
            return PHOTO_ORIENTATION_NORMAL;
        }
        else
            ThrowIfFailed(getMetadataHr);

        PROPVARIANT orientation;
        PropVariantInit(&orientation);
            
        HRESULT lookupHr = queryReader->GetMetadataByName(L"System.Photo.Orientation", &orientation);

        const bool propertyNotFound = 
            lookupHr == WINCODEC_ERR_PROPERTYNOTFOUND ||
            lookupHr == WINCODEC_ERR_PROPERTYNOTSUPPORTED;

        // Anything other than property-not-found is unexpected
        if (!propertyNotFound)
            ThrowIfFailed(lookupHr);

        // The property wasn't specified, or specifies an out-of-range value
        if (propertyNotFound ||
            orientation.vt != VT_UI2 ||
            orientation.uiVal < 1 ||
            orientation.uiVal > 8)
        {                
            return PHOTO_ORIENTATION_NORMAL;
        }
        else
        {
            return orientation.uiVal;
        }
    }

    static WICBitmapTransformOptions GetTransformOptionsFromPhotoOrientation(UINT photoOrientation)
    {
        switch (photoOrientation)
        {
        case PHOTO_ORIENTATION_NORMAL: return WICBitmapTransformRotate0;
        case PHOTO_ORIENTATION_ROTATE90: return WICBitmapTransformRotate270;
        case PHOTO_ORIENTATION_ROTATE180: return WICBitmapTransformRotate180;
        case PHOTO_ORIENTATION_ROTATE270: return WICBitmapTransformRotate90;
        case PHOTO_ORIENTATION_FLIPHORIZONTAL: return WICBitmapTransformFlipHorizontal;
        case PHOTO_ORIENTATION_FLIPVERTICAL: return WICBitmapTransformFlipVertical;
        case PHOTO_ORIENTATION_TRANSPOSE: return static_cast<WICBitmapTransformOptions>(WICBitmapTransformRotate270 | WICBitmapTransformFlipHorizontal);
        case PHOTO_ORIENTATION_TRANSVERSE: return static_cast<WICBitmapTransformOptions>(WICBitmapTransformRotate90 | WICBitmapTransformFlipHorizontal);
        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<typename T>
    static ComPtr<IWICBitmapSource> CreateWicBitmapSourceWithExifTransform(T fileNameOrStream)
    {
        auto adapter = CanvasBitmapAdapter::GetInstance();

        auto source = adapter->CreateWicBitmapSource(fileNameOrStream);

        if (source.Transform == WICBitmapTransformRotate0)
            return source.Source;

        return adapter->CreateFlipRotator(source.Source, source.Transform);        
    }

    
    //
    // DefaultBitmapAdapter
    //

    DefaultBitmapAdapter::DefaultBitmapAdapter()
        : m_wicAdapter(WicAdapter::GetInstance())
    {
    }

    void DefaultBitmapAdapter::SaveLockedMemoryToStream(
        IRandomAccessStream* randomAccessStream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        unsigned int width,
        unsigned int height,
        float dpiX,
        float dpiY,
        ScopedBitmapMappedPixelAccess* bitmapLock)
    {
        ComPtr<IWICStream> wicStream;
        ThrowIfFailed(m_wicAdapter->GetFactory()->CreateStream(&wicStream));

        ComPtr<IStream> iStream;
        ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream, IID_PPV_ARGS(&iStream)));

        ThrowIfFailed(wicStream->InitializeFromIStream(iStream.Get()));

        SaveLockedMemoryToNativeStream(
            m_wicAdapter->GetFactory().Get(), 
            wicStream.Get(), 
            GetGUIDForFileFormat(fileFormat),
            quality, 
            width, 
            height, 
            dpiX,
            dpiY, 
            bitmapLock);
    }

    void DefaultBitmapAdapter::SaveLockedMemoryToFile(
        HSTRING fileName,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        unsigned int width,
        unsigned int height,
        float dpiX,
        float dpiY,
        ScopedBitmapMappedPixelAccess* bitmapLock)
    {
        WinString fileNameString(fileName);

        GUID encoderGuid;
        if (fileFormat == CanvasBitmapFileFormat::Auto)
        {
            encoderGuid = GetEncoderFromFileExtension(static_cast<const wchar_t*>(fileNameString));
        }
        else
        {
            encoderGuid = GetGUIDForFileFormat(fileFormat);
        }

        ComPtr<IWICStream> wicStream;
        ThrowIfFailed(m_wicAdapter->GetFactory()->CreateStream(&wicStream));

        ThrowIfFailed(wicStream->InitializeFromFilename(static_cast<const wchar_t*>(fileNameString), GENERIC_WRITE));

        SaveLockedMemoryToNativeStream(
            m_wicAdapter->GetFactory().Get(),
            wicStream.Get(), 
            encoderGuid, 
            quality, 
            width, 
            height, 
            dpiX, 
            dpiY, 
            bitmapLock);
    }

    WicBitmapSource DefaultBitmapAdapter::CreateWicBitmapSource(HSTRING fileName, bool tryEnableIndexing)
    {
        ComPtr<IWICStream> stream;
        ThrowIfFailed(m_wicAdapter->GetFactory()->CreateStream(&stream));

        WinString fileNameString(fileName);
        ThrowIfFailed(stream->InitializeFromFilename(static_cast<const wchar_t*>(fileNameString), GENERIC_READ));

        return CreateWicBitmapSource(stream.Get(), tryEnableIndexing);
    }

    WicBitmapSource DefaultBitmapAdapter::CreateWicBitmapSource(IStream* fileStream, bool tryEnableIndexing)
    {
        ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
        ThrowIfFailed(m_wicAdapter->GetFactory()->CreateDecoderFromStream(
            fileStream,
            nullptr,
            WICDecodeMetadataCacheOnDemand,
            &wicBitmapDecoder));

        auto ddsDecoder = MaybeAs<IWICDdsDecoder>(wicBitmapDecoder);
        if (ddsDecoder)
        {
            // If it's a block compressed DDS file then we want to pass the
            // frame back unmodified.  This allows us to directly copy the
            // compressed data from the file into the bitmap.
            
            WICDdsParameters ddsParameters;
            ThrowIfFailed(ddsDecoder->GetParameters(&ddsParameters));

            if (ddsParameters.DxgiFormat == DXGI_FORMAT_BC1_UNORM ||
                ddsParameters.DxgiFormat == DXGI_FORMAT_BC2_UNORM ||
                ddsParameters.DxgiFormat == DXGI_FORMAT_BC3_UNORM)
            {
                ComPtr<IWICBitmapFrameDecode> ddsFrame;
                ThrowIfFailed(ddsDecoder->GetFrame(0, 0, 0, &ddsFrame));

                // DDS files never need a transform and are not indexed
                return WicBitmapSource{ ddsFrame, WICBitmapTransformRotate0, false };
            }
        }

        ComPtr<IWICBitmapFrameDecode> wicBitmapFrameDecode;
        ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBitmapFrameDecode));

        bool isIndexed = tryEnableIndexing ? TryEnableIndexing(wicBitmapFrameDecode) : false;

        auto orientation = GetOrientationFromFrameDecode(wicBitmapFrameDecode);
        auto transformOptions = GetTransformOptionsFromPhotoOrientation(orientation);

        ComPtr<IWICFormatConverter> wicFormatConverter;
        ThrowIfFailed(m_wicAdapter->GetFactory()->CreateFormatConverter(&wicFormatConverter));

        ThrowIfFailed(wicFormatConverter->Initialize(
            wicBitmapFrameDecode.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0,
            WICBitmapPaletteTypeMedianCut));

        return WicBitmapSource{ wicFormatConverter, transformOptions, isIndexed };
    }

    ComPtr<IWICBitmapSource> DefaultBitmapAdapter::CreateFlipRotator(
        ComPtr<IWICBitmapSource> const& source,
        WICBitmapTransformOptions transformOptions)
    {
        ComPtr<IWICBitmapFlipRotator> bitmapFlipRotator;

        ThrowIfFailed(m_wicAdapter->GetFactory()->CreateBitmapFlipRotator(&bitmapFlipRotator));
        ThrowIfFailed(bitmapFlipRotator->Initialize(source.Get(), transformOptions));

        return bitmapFlipRotator;
    }


    ComPtr<CanvasBitmap> CanvasBitmap::CreateNew(
        ICanvasDevice* canvasDevice,
        HSTRING fileName,
        float dpi,
        CanvasAlphaMode alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto wicBitmapSource = CreateWicBitmapSourceWithExifTransform(fileName);

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(wicBitmapSource.Get(), dpi, alpha);

        auto bitmap = Make<CanvasBitmap>(
            canvasDevice,
            d2dBitmap.Get());
        CheckMakeResult(bitmap);
        
        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmap::CreateNew(
        ICanvasDevice* canvasDevice,
        IStream* fileStream,
        float dpi,
        CanvasAlphaMode alpha)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto wicBitmapSource = CreateWicBitmapSourceWithExifTransform(fileStream);

        auto d2dBitmap = canvasDeviceInternal->CreateBitmapFromWicResource(wicBitmapSource.Get(), dpi, alpha);

        auto bitmap = Make<CanvasBitmap>(
            canvasDevice,
            d2dBitmap.Get());
        CheckMakeResult(bitmap);
        
        return bitmap;
    }


    ComPtr<CanvasBitmap> CanvasBitmap::CreateNew(
        ICanvasDevice* device,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        DirectXPixelFormat format,
        CanvasAlphaMode alpha)
    {
        auto dxgiFormat = static_cast<DXGI_FORMAT>(format);
        auto blockSize = GetBlockSize(dxgiFormat);

        if ((widthInPixels % blockSize) != 0 || (heightInPixels % blockSize) != 0)
            ThrowHR(E_INVALIDARG, Strings::BlockCompressedDimensionsMustBeMultipleOf4);
        
        auto blocksWide = widthInPixels / blockSize;
        auto pitch = GetBytesPerBlock(dxgiFormat) * blocksWide;

        // D2D does not fail attempts to create zero-sized bitmaps. Neither does this.
        if (pitch == 0)
            pitch = byteCount;

        // Validate that the buffer is large enough
        auto blocksHigh = heightInPixels / blockSize;

        if (byteCount < blocksHigh * pitch)
            ThrowHR(E_INVALIDARG);

        auto d2dBitmap = As<ICanvasDeviceInternal>(device)->CreateBitmapFromBytes(
            bytes,
            pitch,
            widthInPixels,
            heightInPixels,
            dpi,
            format,
            alpha);

        auto bitmap = Make<CanvasBitmap>(
            device,
            d2dBitmap.Get());
        CheckMakeResult(bitmap);

        return bitmap;
    }

    
    ComPtr<CanvasBitmap> CanvasBitmap::CreateNew(
        ICanvasDevice* device,
        uint32_t colorCount,
        Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        CanvasAlphaMode alpha)
    {
        // Convert color array to bytes according to the default format, B8G8R8A8_UNORM.
        std::vector<uint8_t> convertedBytes;
        convertedBytes.resize(colorCount * 4);

        for (uint32_t i = 0; i < colorCount; i++)
        {
            convertedBytes[i * 4 + 0] = colors[i].B;
            convertedBytes[i * 4 + 1] = colors[i].G;
            convertedBytes[i * 4 + 2] = colors[i].R;
            convertedBytes[i * 4 + 3] = colors[i].A;
        }

        assert(convertedBytes.size() <= UINT_MAX);

        return CreateNew(
            device,
            static_cast<uint32_t>(convertedBytes.size()),
            convertedBytes.empty() ? nullptr : &convertedBytes[0],
            widthInPixels,
            heightInPixels,
            dpi,
            PIXEL_FORMAT(B8G8R8A8UIntNormalized),
            alpha);
    }


#if WINVER > _WIN32_WINNT_WINBLUE

    //
    // BitmapPixelFormat's values are intended to be reinterpreted as
    // DXGI_FORMAT values.  Unfortunately, some of the chosen color values map
    // to UINT rather than UNORM formats.  The intended use is UNORM, and so we
    // fudge the BitmapPixelFormat values in these cases.
    //
    static DirectXPixelFormat GetFudgedFormat(BitmapPixelFormat format)
    {
        using namespace ABI::Windows::Graphics::Imaging;
        
        switch (format)
        {
        case BitmapPixelFormat_Rgba16: return PIXEL_FORMAT(R16G16B16A16UIntNormalized);
        case BitmapPixelFormat_Rgba8:  return PIXEL_FORMAT(R8G8B8A8UIntNormalized);
        case BitmapPixelFormat_Gray8:  return PIXEL_FORMAT(A8UIntNormalized);

        case BitmapPixelFormat_Bgra8: // BitmapPixelFormat already uses the UNORM value here
        default:
            return static_cast<DirectXPixelFormat>(format);
        }        
    }


    ComPtr<CanvasBitmap> CanvasBitmap::CreateNew(
        ICanvasDevice* device,
        ISoftwareBitmap* sourceBitmap)
    {
        using namespace ABI::Windows::Graphics::Imaging;
        using ::Windows::Foundation::IMemoryBufferByteAccess;

        //
        // Extract information from the SoftwareBitmap
        //

        BitmapPixelFormat bitmapPixelFormat;
        ThrowIfFailed(sourceBitmap->get_BitmapPixelFormat(&bitmapPixelFormat));

        BitmapAlphaMode bitmapAlphaMode;
        ThrowIfFailed(sourceBitmap->get_BitmapAlphaMode(&bitmapAlphaMode));

        double dpiX;
        ThrowIfFailed(sourceBitmap->get_DpiX(&dpiX));

        ComPtr<IBitmapBuffer> bitmapBuffer;
        ThrowIfFailed(sourceBitmap->LockBuffer(BitmapBufferAccessMode_Read, &bitmapBuffer));

        ComPtr<IMemoryBufferReference> memoryBuffer;
        ThrowIfFailed(As<IMemoryBuffer>(bitmapBuffer)->CreateReference(&memoryBuffer));

        uint32_t bufferSize;
        uint8_t* buffer;
        ThrowIfFailed(As<IMemoryBufferByteAccess>(memoryBuffer)->GetBuffer(&buffer, &bufferSize));

        BitmapPlaneDescription bitmapPlaneDescription;
        ThrowIfFailed(bitmapBuffer->GetPlaneDescription(0, &bitmapPlaneDescription));

        auto pixelWidth = static_cast<uint32_t>(bitmapPlaneDescription.Width);
        auto pixelHeight = static_cast<uint32_t>(bitmapPlaneDescription.Height);

        //
        // Create an ID2D1Bitmap1 from the source bitmap's data
        //

        auto d2dBitmap = As<ICanvasDeviceInternal>(device)->CreateBitmapFromBytes(
            buffer + bitmapPlaneDescription.StartIndex,
            bitmapPlaneDescription.Stride,
            pixelWidth,
            pixelHeight,
            static_cast<float>(dpiX),
            GetFudgedFormat(bitmapPixelFormat),
            ToCanvasAlphaMode(bitmapAlphaMode));

        //
        // Wrap a CanvasBitmap around it
        //

        auto bitmap = Make<CanvasBitmap>(
            device,
            d2dBitmap.Get());
        CheckMakeResult(bitmap);

        return bitmap;
    }

#endif

    
    //
    // ICanvasBitmapStatics
    //

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11Surface(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceImpl(
            resourceCreator,
            surface,
            DEFAULT_DPI,
            nullptr,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceImpl(
            resourceCreator,
            surface,
            dpi,
            nullptr,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromDirect3D11SurfaceWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromDirect3D11SurfaceImpl(
            resourceCreator,
            surface,
            dpi,
            &alpha,
            canvasBitmap);
    }

    static bool DoesSurfaceSupportAlpha(IDirect3DSurface* surface)
    {
        Direct3DSurfaceDescription surfaceDescription;
        ThrowIfFailed(surface->get_Description(&surfaceDescription));

        return surfaceDescription.Format != PIXEL_FORMAT(B8G8R8X8UIntNormalized)
            && surfaceDescription.Format != PIXEL_FORMAT(R8G8UIntNormalized)
            && surfaceDescription.Format != PIXEL_FORMAT(R8UIntNormalized);
    }

    HRESULT CanvasBitmapFactory::CreateFromDirect3D11SurfaceImpl(
        ICanvasResourceCreator* resourceCreator,
        IDirect3DSurface* surface,
        float dpi,
        CanvasAlphaMode const* requestedAlpha,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(surface);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                CanvasAlphaMode alpha;
                if (requestedAlpha)
                {
                    alpha = *requestedAlpha;
                }
                else
                {
                    alpha = DoesSurfaceSupportAlpha(surface) ? CanvasAlphaMode::Premultiplied : CanvasAlphaMode::Ignore;
                }

                auto d2dBitmap = As<ICanvasDeviceInternal>(canvasDevice)->CreateBitmapFromSurface(surface, dpi, alpha);

                auto newBitmap = ResourceManager::GetOrCreate<ICanvasBitmap>(canvasDevice.Get(), d2dBitmap.Get());

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytes(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromBytesWithDpiAndAlpha(
            resourceCreator,
            byteCount,
            bytes,
            widthInPixels,
            heightInPixels,
            format,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytesWithDpi(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromBytesWithDpiAndAlpha(
            resourceCreator,
            byteCount,
            bytes,
            widthInPixels,
            heightInPixels,
            format,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromBytesWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        uint32_t byteCount,
        BYTE* bytes,
        int32_t widthInPixels,
        int32_t heightInPixels,
        DirectXPixelFormat format,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                if (byteCount)
                    CheckInPointer(bytes);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = CanvasBitmap::CreateNew(
                    canvasDevice.Get(), 
                    byteCount, 
                    bytes, 
                    widthInPixels,
                    heightInPixels,
                    dpi,
                    format, 
                    alpha);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColors(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromColorsWithDpiAndAlpha(
            resourceCreator,
            colorCount,
            colors,
            widthInPixels,
            heightInPixels,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColorsWithDpi(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        ICanvasBitmap** canvasBitmap)
    {
        return CreateFromColorsWithDpiAndAlpha(
            resourceCreator,
            colorCount,
            colors,
            widthInPixels,
            heightInPixels,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmap);
    }

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromColorsWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        uint32_t colorCount,
        ABI::Windows::UI::Color* colors,
        int32_t widthInPixels,
        int32_t heightInPixels,
        float dpi,
        CanvasAlphaMode alpha,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                if (colorCount)
                    CheckInPointer(colors);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = CanvasBitmap::CreateNew(
                    canvasDevice.Get(), 
                    colorCount,
                    colors,
                    widthInPixels,
                    heightInPixels,
                    dpi,
                    alpha);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

#if WINVER > _WIN32_WINNT_WINBLUE

    IFACEMETHODIMP CanvasBitmapFactory::CreateFromSoftwareBitmap(
        ICanvasResourceCreator* resourceCreator,
        ISoftwareBitmap* sourceBitmap,
        ICanvasBitmap** canvasBitmap)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(sourceBitmap);
                CheckAndClearOutPointer(canvasBitmap);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                auto newBitmap = CanvasBitmap::CreateNew(
                    canvasDevice.Get(),
                    sourceBitmap);

                ThrowIfFailed(newBitmap.CopyTo(canvasBitmap));
            });
    }

#endif

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstring(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileName,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromHstringWithDpiAndAlpha(
            resourceCreator,
            fileName,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstringWithDpi(
        ICanvasResourceCreator* resourceCreator,
        HSTRING fileName,
        float dpi,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromHstringWithDpiAndAlpha(
            resourceCreator,
            fileName,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromHstringWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        HSTRING rawFileName,
        float dpi,
        CanvasAlphaMode alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(rawFileName);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                WinString fileName(rawFileName);

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                    [=]
                    {
                        return CanvasBitmap::CreateNew(canvasDevice.Get(), fileName, dpi, alpha);
                    });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUri(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* uri,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromUriWithDpiAndAlpha(
            resourceCreator, 
            uri, 
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUriWithDpi(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* uri,
        float dpi,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromUriWithDpiAndAlpha(
            resourceCreator,
            uri,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }
     
    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromUriWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::Foundation::IUriRuntimeClass* uri,
        float dpi,
        CanvasAlphaMode alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(uri);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                ComPtr<IRandomAccessStreamReferenceStatics> streamReferenceStatics;
                ThrowIfFailed(GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &streamReferenceStatics));

                ComPtr<IRandomAccessStreamReference> streamReference;
                ThrowIfFailed(streamReferenceStatics->CreateFromUri(uri, &streamReference));

                // Start opening the file.
                ComPtr<IAsyncOperation<IRandomAccessStreamWithContentType*>> openOperation;
                ThrowIfFailed(streamReference->OpenReadAsync(&openOperation));

                // Our main async operation is constructed as a continuation of the file open,
                // so the lambda will only start executing once the file is ready.
                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(openOperation, [=]
                {
                    ComPtr<IRandomAccessStreamWithContentType> randomAccessStream;
                    ThrowIfFailed(openOperation->GetResults(&randomAccessStream));

                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    return CanvasBitmap::CreateNew(canvasDevice.Get(), stream.Get(), dpi, alpha);
                });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromStream(
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromStreamWithDpiAndAlpha(
            resourceCreator,
            stream,
            DEFAULT_DPI,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromStreamWithDpi(
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* stream,
        float dpi,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return LoadAsyncFromStreamWithDpiAndAlpha(
            resourceCreator,
            stream,
            dpi,
            CanvasAlphaMode::Premultiplied,
            canvasBitmapAsyncOperation);
    }

    IFACEMETHODIMP CanvasBitmapFactory::LoadAsyncFromStreamWithDpiAndAlpha(
        ICanvasResourceCreator* resourceCreator,
        IRandomAccessStream* rawStream,
        float dpi,
        CanvasAlphaMode alpha,
        ABI::Windows::Foundation::IAsyncOperation<CanvasBitmap*>** canvasBitmapAsyncOperation)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckInPointer(rawStream);
                CheckAndClearOutPointer(canvasBitmapAsyncOperation);

                ComPtr<ICanvasDevice> canvasDevice;
                ThrowIfFailed(resourceCreator->get_Device(&canvasDevice));

                ComPtr<IRandomAccessStream> stream = rawStream;

                auto asyncOperation = Make<AsyncOperation<CanvasBitmap>>(
                [=]
                {
                    ComPtr<IStream> nativeStream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(stream.Get(), IID_PPV_ARGS(&nativeStream)));

                    return CanvasBitmap::CreateNew(canvasDevice.Get(), nativeStream.Get(), dpi, alpha);
                });

                CheckMakeResult(asyncOperation);
                ThrowIfFailed(asyncOperation.CopyTo(canvasBitmapAsyncOperation));
            });
    }


    //
    // CanvasBitmap
    //

    CanvasBitmap::CanvasBitmap(
        ICanvasDevice* device,
        ID2D1Bitmap1* d2dBitmap)
        : CanvasBitmapImpl(device, d2dBitmap)
    {
        assert(!IsRenderTargetBitmap(d2dBitmap) 
            && "CanvasBitmap should never be constructed with a render-target bitmap.  This should have been validated before construction.");
    }

    void GetPixelBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        uint8_t** valueElements)
    {
        CheckInPointer(valueCount);
        CheckAndClearOutPointer(valueElements);

        BitmapSubRectangle r(d2dBitmap, subRectangle);

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ, &subRectangle);

        ComArray<BYTE> array(r.GetTotalBytes());

        auto destination = begin(array);
        auto source = begin(bitmapPixelAccess);

        for (auto i = 0u; i < r.GetBlocksHigh(); ++i)
        {
            destination = std::copy(source, source + r.GetBytesPerRow(), destination);
            source += bitmapPixelAccess.GetStride();
        }

        array.Detach(valueCount, valueElements);
    }

    void GetPixelColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t* valueCount,
        Color **valueElements)
    {
        CheckInPointer(valueCount);
        CheckAndClearOutPointer(valueElements);

        VerifyWellFormedSubrectangle(subRectangle, d2dBitmap->GetPixelSize());

        if (d2dBitmap->GetPixelFormat().format != DXGI_FORMAT_B8G8R8A8_UNORM)
        {
            ThrowHR(E_INVALIDARG, Strings::PixelColorsFormatRestriction);
        }

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ, &subRectangle);

        const unsigned int subRectangleWidth = subRectangle.right - subRectangle.left;
        const unsigned int subRectangleHeight = subRectangle.bottom - subRectangle.top;
        const unsigned int destSizeInPixels = subRectangleWidth * subRectangleHeight;
        ComArray<Color> array(destSizeInPixels);

        byte* sourceRowStart = bitmapPixelAccess.GetLockedData();

        for (unsigned int y = 0; y < subRectangleHeight; y++)
        {
            for (unsigned int x = 0; x < subRectangleWidth; x++)
            {
                uint32_t sourcePixel = *(reinterpret_cast<uint32_t*>(&sourceRowStart[x * 4]));
                Color& destColor = array[y * subRectangleWidth + x];
                destColor.B = (sourcePixel >> 0) & 0xFF;
                destColor.G = (sourcePixel >> 8) & 0xFF;
                destColor.R = (sourcePixel >> 16) & 0xFF;
                destColor.A = (sourcePixel >> 24) & 0xFF;
            }
            sourceRowStart += bitmapPixelAccess.GetStride();
        }

        array.Detach(valueCount, valueElements);
    }

    void SaveBitmapToFileImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        HSTRING rawfileName,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction)
    {
        WinString fileName(rawfileName);

        auto asyncAction = Make<AsyncAction>(
            [=]
            {
                const D2D1_SIZE_U size = d2dBitmap->GetPixelSize();
                float dpiX, dpiY;
                d2dBitmap->GetDpi(&dpiX, &dpiY);

                ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ);

                CanvasBitmapAdapter::GetInstance()->SaveLockedMemoryToFile(
                    fileName,
                    fileFormat,
                    quality,
                    size.width,
                    size.height,
                    dpiX,
                    dpiY,
                    &bitmapPixelAccess);
            });

        CheckMakeResult(asyncAction);
        ThrowIfFailed(asyncAction.CopyTo(resultAsyncAction));
    }

    void SaveBitmapToStreamImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        ComPtr<IRandomAccessStream> const& stream,
        CanvasBitmapFileFormat fileFormat,
        float quality,
        IAsyncAction **resultAsyncAction)
    {
        if (fileFormat == CanvasBitmapFileFormat::Auto)
        {
            ThrowHR(E_INVALIDARG, Strings::AutoFileFormatNotAllowed);
        }

        auto asyncAction = Make<AsyncAction>(
            [=]
            {
                const D2D1_SIZE_U size = d2dBitmap->GetPixelSize();
                float dpiX, dpiY;
                d2dBitmap->GetDpi(&dpiX, &dpiY);

                ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_READ);

                CanvasBitmapAdapter::GetInstance()->SaveLockedMemoryToStream(
                    stream.Get(),
                    fileFormat,
                    quality,
                    size.width,
                    size.height,
                    dpiX,
                    dpiY,
                    &bitmapPixelAccess);
            });

        CheckMakeResult(asyncAction);
        ThrowIfFailed(asyncAction.CopyTo(resultAsyncAction));
    }

    void SetPixelBytesImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        uint8_t* valueElements)
    {
        CheckInPointer(valueElements);

        BitmapSubRectangle r(d2dBitmap, subRectangle);

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_WRITE, &subRectangle);

        if (valueCount != r.GetTotalBytes())
        {
            WinStringBuilder message;
            message.Format(Strings::WrongArrayLength, r.GetTotalBytes(), valueCount);
            ThrowHR(E_INVALIDARG, message.Get());
        }

        auto destination = begin(bitmapPixelAccess);
        auto source = stdext::make_checked_array_iterator(valueElements, valueCount);

        for (auto i = 0u; i < r.GetBlocksHigh(); ++i)
        {
            std::copy(source, source + r.GetBytesPerRow(), destination);
            source += r.GetBytesPerRow();
            destination += bitmapPixelAccess.GetStride();
        }
    }

    void SetPixelColorsImpl(
        ComPtr<ID2D1Bitmap1> const& d2dBitmap,
        D2D1_RECT_U const& subRectangle,
        uint32_t valueCount,
        Color *valueElements)
    {
        CheckInPointer(valueElements);

        VerifyWellFormedSubrectangle(subRectangle, d2dBitmap->GetPixelSize());

        const unsigned int subRectangleWidth = subRectangle.right - subRectangle.left;
        const unsigned int subRectangleHeight = subRectangle.bottom - subRectangle.top;

        const uint32_t expectedArraySize = subRectangleWidth * subRectangleHeight;
        if (valueCount != expectedArraySize)
        {
            WinStringBuilder message;
            message.Format(Strings::WrongArrayLength, expectedArraySize, valueCount);
            ThrowHR(E_INVALIDARG, message.Get());
        }

        if (d2dBitmap->GetPixelFormat().format != DXGI_FORMAT_B8G8R8A8_UNORM)
        {
            ThrowHR(E_INVALIDARG, Strings::PixelColorsFormatRestriction);
        }

        ScopedBitmapMappedPixelAccess bitmapPixelAccess(d2dBitmap.Get(), D3D11_MAP_WRITE, &subRectangle);

        const unsigned int destSizeInPixels = subRectangleWidth * subRectangleHeight;
        ComArray<Color> array(destSizeInPixels);

        byte* destRowStart = bitmapPixelAccess.GetLockedData();

        for (unsigned int y = 0; y < subRectangleHeight; y++)
        {
            for (unsigned int x = 0; x < subRectangleWidth; x++)
            {
                uint32_t* destPixel = reinterpret_cast<uint32_t*>(&destRowStart[x * 4]);
                Color& sourceColor = valueElements[y * subRectangleWidth + x];
                *destPixel = 
                    (static_cast<uint32_t>(sourceColor.B) << 0) | 
                    (static_cast<uint32_t>(sourceColor.G) << 8) |
                    (static_cast<uint32_t>(sourceColor.R) << 16) |
                    (static_cast<uint32_t>(sourceColor.A) << 24);
            }
            destRowStart += bitmapPixelAccess.GetStride();
        }
    }


    HRESULT CopyPixelsFromBitmapImpl(
        ICanvasBitmap* to,
        ICanvasBitmap* from,
        int32_t* destX,
        int32_t* destY,
        int32_t* sourceRectLeft,
        int32_t* sourceRectTop,
        int32_t* sourceRectWidth,
        int32_t* sourceRectHeight)
    {
        return ExceptionBoundary(
            [&]
            {
                assert(to);
                CheckInPointer(from);

                auto toBitmapInternal = As<ICanvasBitmapInternal>(to);
                auto toD2dBitmap = toBitmapInternal->GetD2DBitmap();

                auto fromBitmapInternal = As<ICanvasBitmapInternal>(from);
                auto fromD2dBitmap = fromBitmapInternal->GetD2DBitmap();

                bool useDestPt = false;
                D2D1_POINT_2U destPoint;                
                if (destX)
                {
                    assert(destY);
                    useDestPt = true;
                    destPoint = ToD2DPointU(*destX, *destY);

                    // D2D validates this internally, but this allows us to
                    // provide a specific error message
                    if (!IsBlockAligned(GetBlockSize(toD2dBitmap->GetPixelFormat().format), destPoint))
                        ThrowHR(E_INVALIDARG, Strings::BlockCompressedSubRectangleMustBeAligned);
                }

                bool useSourceRect = false;
                D2D1_RECT_U sourceRect;
                if (sourceRectLeft)
                {
                    assert(sourceRectTop && sourceRectWidth && sourceRectHeight && useDestPt);
                    useSourceRect = true;
                    sourceRect = ToD2DRectU(*sourceRectLeft, *sourceRectTop, *sourceRectWidth, *sourceRectHeight);

                    // D2D validates this internally, but this allows us to
                    // provide a specific error message
                    if (!IsBlockAligned(GetBlockSize(fromD2dBitmap->GetPixelFormat().format), sourceRect))
                        ThrowHR(E_INVALIDARG, Strings::BlockCompressedSubRectangleMustBeAligned);
                }

                ThrowIfFailed(toD2dBitmap->CopyFromBitmap(
                    useDestPt? &destPoint : nullptr,
                    fromD2dBitmap.Get(),
                    useSourceRect? &sourceRect : nullptr));
            });

    }

    ActivatableClassWithFactory(CanvasBitmap, CanvasBitmapFactory);
}}}}
