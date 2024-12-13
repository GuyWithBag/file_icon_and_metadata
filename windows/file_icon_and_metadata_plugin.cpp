#include "file_icon_and_metadata_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>
#include <VersionHelpers.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <objbase.h>
#include <wincodec.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <vector>
#include <string>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "WindowsCodecs.lib")

namespace file_icon_and_metadata {

// Utility to convert wide string to UTF-8
inline std::string WideToUtf8(const std::wstring &wstr) {
  if (wstr.empty())
    return std::string();
  int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(),
                                       (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(sizeNeeded, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.data(),
                      (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
  return strTo;
}

// Simple Base64 encoder for demonstration purposes
static const char encodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string Base64Encode(const std::vector<BYTE> &data) {
  std::string encoded;
  encoded.reserve(((data.size() + 2) / 3) * 4);

  size_t i = 0;
  while (i < data.size()) {
    uint32_t octet_a = i < data.size() ? data[i++] : 0;
    uint32_t octet_b = i < data.size() ? data[i++] : 0;
    uint32_t octet_c = i < data.size() ? data[i++] : 0;

    uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

    encoded.push_back(encodeTable[(triple >> 18) & 0x3F]);
    encoded.push_back(encodeTable[(triple >> 12) & 0x3F]);
    encoded.push_back((i > data.size() + 1) ? '=' : encodeTable[(triple >> 6) & 0x3F]);
    encoded.push_back((i > data.size()) ? '=' : encodeTable[triple & 0x3F]);
  }

  return encoded;
}

// Convert HICON to PNG bytes using WIC
std::vector<BYTE> IconToPNGBytes(HICON hIcon) {
  std::vector<BYTE> pngData;
  if (!hIcon)
    return pngData;

  ICONINFO iconInfo = {0};
  if (!GetIconInfo(hIcon, &iconInfo)) {
    return pngData;
  }

  BITMAP bmColor = {0};
  if (iconInfo.hbmColor) {
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmColor);
  } else {
    // Fallback to mask if no color bitmap
    GetObject(iconInfo.hbmMask, sizeof(BITMAP), &bmColor);
    bmColor.bmHeight /= 2; // The mask is double height
  }

  int width = bmColor.bmWidth;
  int height = bmColor.bmHeight;

  HDC hDC = GetDC(NULL);
  if (!hDC) {
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    return pngData;
  }

  BITMAPINFO bi = {0};
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = width;
  bi.bmiHeader.biHeight = -height; // top-down DIB
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;

  BYTE *bits = nullptr;
  HBITMAP hDib = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, (void **)&bits, NULL, 0);
  if (!hDib) {
    ReleaseDC(NULL, hDC);
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    return pngData;
  }

  HDC hMemDC = CreateCompatibleDC(hDC);
  HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hDib);

  DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);

  SelectObject(hMemDC, hOldBmp);
  DeleteDC(hMemDC);
  ReleaseDC(NULL, hDC);

  DeleteObject(iconInfo.hbmColor);
  DeleteObject(iconInfo.hbmMask);

  // Initialize COM for WIC
  CoInitialize(NULL);

  IWICImagingFactory *pFactory = nullptr;
  HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pFactory));
  if (FAILED(hr) || !pFactory) {
    DeleteObject(hDib);
    CoUninitialize();
    return pngData;
  }

  IWICBitmap *pBitmap = nullptr;
  hr = pFactory->CreateBitmapFromMemory(
      width, height, GUID_WICPixelFormat32bppBGRA,
      width * 4, height * width * 4, bits, &pBitmap);
  if (FAILED(hr) || !pBitmap) {
    pFactory->Release();
    DeleteObject(hDib);
    CoUninitialize();
    return pngData;
  }

  IWICStream *pStream = nullptr;
  hr = pFactory->CreateStream(&pStream);
  if (FAILED(hr) || !pStream) {
    pBitmap->Release();
    pFactory->Release();
    DeleteObject(hDib);
    CoUninitialize();
    return pngData;
  }

  hr = pStream->InitializeFromMemory(nullptr, 0);
  IWICBitmapEncoder *pEncoder = nullptr;
  hr = pFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
  if (FAILED(hr) || !pEncoder) {
    pStream->Release();
    pBitmap->Release();
    pFactory->Release();
    DeleteObject(hDib);
    CoUninitialize();
    return pngData;
  }

  pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);

  IWICBitmapFrameEncode *pFrame = nullptr;
  IPropertyBag2 *pProps = nullptr;
  hr = pEncoder->CreateNewFrame(&pFrame, &pProps);
  if (FAILED(hr) || !pFrame) {
    pEncoder->Release();
    pStream->Release();
    pBitmap->Release();
    pFactory->Release();
    DeleteObject(hDib);
    CoUninitialize();
    return pngData;
  }

  pFrame->Initialize(pProps);
  pFrame->SetSize(width, height);
  WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
  pFrame->SetPixelFormat(&format);

  pFrame->WritePixels(height, width * 4, height * width * 4, bits);
  pFrame->Commit();
  pEncoder->Commit();

  STATSTG stat;
  pStream->Stat(&stat, STATFLAG_NONAME);
  ULONG cbSize = (ULONG)stat.cbSize.QuadPart;
  pngData.resize(cbSize);

  LARGE_INTEGER liZero = {};
  pStream->Seek(liZero, STREAM_SEEK_SET, NULL);
  ULONG cbRead = 0;
  pStream->Read(pngData.data(), cbSize, &cbRead);

  pFrame->Release();
  pEncoder->Release();
  pStream->Release();
  pBitmap->Release();
  pFactory->Release();
  DeleteObject(hDib);
  CoUninitialize();

  return pngData;
}

// Extract icon and return as base64 PNG string
flutter::EncodableMap GetFileIconData(const std::string &path) {
  flutter::EncodableMap result;
  std::wstring wpath(path.begin(), path.end());

  SHFILEINFO sfi = {0};
  if (!SHGetFileInfoW(wpath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
    return result; // empty map
  }

  auto pngData = IconToPNGBytes(sfi.hIcon);
  DestroyIcon(sfi.hIcon);

  if (pngData.empty()) {
    return result; // empty map
  }

  std::string base64Icon = Base64Encode(pngData);
  // Use EncodableValue for the key and value
  result[flutter::EncodableValue("icon")] = flutter::EncodableValue(base64Icon);
  return result;
}

// static
void FileIconAndMetadataPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "file_icon_and_metadata",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FileIconAndMetadataPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

FileIconAndMetadataPlugin::FileIconAndMetadataPlugin() {}

FileIconAndMetadataPlugin::~FileIconAndMetadataPlugin() {}

void FileIconAndMetadataPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  const std::string &method_name = method_call.method_name();

  if (method_name == "getPlatformVersion") {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else if (method_name == "getFileIcon") {
    // Expect a map with { "path": "<file_path>" }
    const auto *args = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (!args) {
      result->Error("BadArguments", "Argument map expected.");
      return;
    }

    auto path_it = args->find(flutter::EncodableValue("path"));
    if (path_it == args->end() || !std::holds_alternative<std::string>(path_it->second)) {
      result->Error("BadArguments", "Path string missing.");
      return;
    }

    std::string path = std::get<std::string>(path_it->second);
    auto info = GetFileIconData(path);
    if (info.empty()) {
      result->Error("FileError", "Unable to retrieve file icon.");
    } else {
      result->Success(flutter::EncodableValue(info));
    }
  } else {
    result->NotImplemented();
  }
}

} // namespace file_icon_and_metadata
