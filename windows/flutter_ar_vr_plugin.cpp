#include "flutter_ar_vr_plugin.h"

// Include necessary Windows and OpenXR headers.
#include <windows.h>
#include <VersionHelpers.h>

// OpenXR SDK header
#define XrGraphicsBindingD3D11KHR
#define XrGraphicsBindingD3D12KHR
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>

// Include Flutter plugin headers.
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <future> // Include for async functionality
#include <string>

#include <tchar.h> // Include for safe C string functions

namespace flutter_ar_vr
{
  typedef HRESULT(WINAPI *D3D11CreateDeviceFunc)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT,
                                                 const D3D_FEATURE_LEVEL *, UINT, UINT,
                                                 ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);

  typedef HRESULT(WINAPI *D3D12CreateDeviceFunc)(IUnknown *, D3D_FEATURE_LEVEL, REFIID, void **);

  // Constructor to initialize the registrar.
  FlutterArVrPlugin::FlutterArVrPlugin(flutter::PluginRegistrarWindows *registrar)
      : registrar_(registrar) {}

  FlutterArVrPlugin::~FlutterArVrPlugin() {}

  void FlutterArVrPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "flutter_ar_vr",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<FlutterArVrPlugin>(registrar); // Pass registrar to constructor.

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  bool isOpenXRInstalled()
  {
    HKEY hKey;
    const std::wstring registryPath = L"SOFTWARE\\Khronos\\OpenXR\\1";
    const std::wstring registryKey = L"ActiveRuntime";

    // Open the registry key
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
      std::cerr << "Error opening registry key." << std::endl;
      return false;
    }

    // Query the "ActiveRuntime" value
    wchar_t runtimePath[MAX_PATH];
    DWORD bufferSize = sizeof(runtimePath);
    if (RegQueryValueExW(hKey, registryKey.c_str(), NULL, NULL, (LPBYTE)runtimePath, &bufferSize) != ERROR_SUCCESS)
    {
      std::cerr << "Error reading registry value." << std::endl;
      RegCloseKey(hKey);
      return false;
    }

    // Check if a runtime path is found
    if (wcslen(runtimePath) == 0)
    {
      std::cerr << "OpenXR runtime not found." << std::endl;
      RegCloseKey(hKey);
      return false;
    }

    std::wcout << L"OpenXR runtime found at: " << runtimePath << std::endl;
    RegCloseKey(hKey);
    return true;
  }

  bool IsDirectX12Available()
  {
    HMODULE d3d12 = LoadLibraryA("d3d12.dll");
    if (d3d12)
    {
      FreeLibrary(d3d12);
      return true;
    }
    return false;
  }

  bool IsDirectX11Available()
  {
    HMODULE d3d11 = LoadLibraryA("d3d11.dll");
    if (d3d11)
    {
      FreeLibrary(d3d11);
      return true;
    }
    return false;
  }

  void InstallDirectX()
  {
    std::wstring url = L"https://www.microsoft.com/en-us/download/details.aspx?id=35";
    ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    std::wcout << L"Opening DirectX download page..." << std::endl;
  }

  bool InitializeDirectX()
  {
    if (IsDirectX12Available())
    {
      HMODULE d3d12Lib = LoadLibraryA("d3d12.dll");
      if (d3d12Lib)
      {
        D3D12CreateDeviceFunc D3D12CreateDevice = (D3D12CreateDeviceFunc)GetProcAddress(d3d12Lib, "D3D12CreateDevice");
        if (D3D12CreateDevice)
        {
          std::cout << "Successfully loaded DirectX 12." << std::endl;
        }
        FreeLibrary(d3d12Lib);
      }
      return true;
    }
    else if (IsDirectX11Available())
    {
      HMODULE d3d11Lib = LoadLibraryA("d3d11.dll");
      if (d3d11Lib)
      {
        D3D11CreateDeviceFunc D3D11CreateDevice = (D3D11CreateDeviceFunc)GetProcAddress(d3d11Lib, "D3D11CreateDevice");
        if (D3D11CreateDevice)
        {
          std::cout << "Successfully loaded DirectX 11." << std::endl;
        }
        FreeLibrary(d3d11Lib);
      }
      return true;
    }
    else
    {
      std::cerr << "DirectX is not installed. Please install DirectX 11 or 12." << std::endl;
      InstallDirectX();
      return false;
    }
  }

  bool initializeOpenXR()
  {
    // Step 1: Check if OpenXR is installed
    if (!isOpenXRInstalled())
    {
      std::cout << "OpenXR not installed. Attempting to install..." << std::endl;
      return false; // We can add installation logic here if needed
    }

    std::cout << "OpenXR is installed. Proceeding with initialization..." << std::endl;
    return true; // OpenXR initialization code would go here.
  }

  bool FlutterArVrPlugin::IsVrDeviceConnected()
  {
    XrInstance xr_instance = XR_NULL_HANDLE;

    XrInstanceCreateInfo instance_create_info = {};
    instance_create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
    strncpy_s(instance_create_info.applicationInfo.applicationName, "Flutter AR VR Plugin", XR_MAX_APPLICATION_NAME_SIZE);
    instance_create_info.applicationInfo.applicationVersion = 1;
    strncpy_s(instance_create_info.applicationInfo.engineName, "Custom Engine", XR_MAX_ENGINE_NAME_SIZE);
    instance_create_info.applicationInfo.engineVersion = 1;
    instance_create_info.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);

    XrResult result = xrCreateInstance(&instance_create_info, &xr_instance);
    if (XR_FAILED(result))
    {
      std::cerr << "Failed to initialize OpenXR instance: " << result << std::endl;
      return false;
    }

    // Check VR system connected
    XrSystemId system_id = XR_NULL_SYSTEM_ID;
    XrSystemGetInfo system_info = {};
    system_info.type = XR_TYPE_SYSTEM_GET_INFO;
    system_info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(xr_instance, &system_info, &system_id);
    if (XR_FAILED(result))
    {
      std::cerr << "Failed to detect VR system: " << result << std::endl;
      xrDestroyInstance(xr_instance);
      return false;
    }

    std::cout << "VR Device detected!" << std::endl;
    xrDestroyInstance(xr_instance);
    return true;
  }

  bool FlutterArVrPlugin::InitVrSdk()
  {
    std::cout << "Initializing OpenXR and DirectX..." << std::endl;

    XrInstance instance = XR_NULL_HANDLE;

    XrInstanceCreateInfo instance_create_info = {};
    instance_create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
    strncpy_s(instance_create_info.applicationInfo.applicationName, "Flutter AR VR Plugin", XR_MAX_APPLICATION_NAME_SIZE);
    instance_create_info.applicationInfo.applicationVersion = 1;
    strncpy_s(instance_create_info.applicationInfo.engineName, "Custom Engine", XR_MAX_ENGINE_NAME_SIZE);
    instance_create_info.applicationInfo.engineVersion = 1;
    instance_create_info.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);

    XrResult result = xrCreateInstance(&instance_create_info, &instance);
    if (XR_FAILED(result))
    {
      std::cerr << "Failed to initialize OpenXR instance: " << result << std::endl;
      return false;
    }

    XrSystemId system_id = XR_NULL_SYSTEM_ID;
    XrSystemGetInfo system_info = {};
    system_info.type = XR_TYPE_SYSTEM_GET_INFO;
    system_info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    result = xrGetSystem(instance, &system_info, &system_id);
    if (XR_FAILED(result))
    {
      std::cerr << "Failed to detect VR system: " << result << std::endl;
      xrDestroyInstance(instance);
      return false;
    }

    XrSessionCreateInfo session_create_info = {};
    session_create_info.type = XR_TYPE_SESSION_CREATE_INFO;
    session_create_info.systemId = system_id;

    XrSession session = XR_NULL_HANDLE;
    result = xrCreateSession(instance, &session_create_info, &session);
    if (XR_FAILED(result))
    {
      std::cerr << "Failed to create OpenXR session: " << result << std::endl;
      xrDestroyInstance(instance);
      return false;
    }

    XrReferenceSpaceCreateInfo reference_space_create_info = {};
    reference_space_create_info.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
    reference_space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;

    XrSpace reference_space = XR_NULL_HANDLE;
    result = xrCreateReferenceSpace(session, &reference_space_create_info, &reference_space);
    if (XR_FAILED(result))
    {
      std::cerr << "Failed to create reference space: " << result << std::endl;
      xrDestroySession(session);
      xrDestroyInstance(instance);
      return false;
    }

    xrDestroySession(session);
    xrDestroyInstance(instance);
    return true;
  }

  void FlutterArVrPlugin::InitializeVr(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    auto async_task = std::async(std::launch::async, [this, result = std::move(result)]() mutable
                                 {
      try {
        if (!initializeOpenXR())
        {
          throw std::runtime_error("OpenXR not installed.");
        }
        if (!InitializeDirectX())
        {
          throw std::runtime_error("DirectX is not installed.");
        }
        if (!IsWindows10OrGreater()) {
          throw std::runtime_error("Windows 10 or higher is required for VR functionality.");
        }
        if (!IsVrDeviceConnected()) {
          throw std::runtime_error("No VR devices detected. Please connect a VR headset.");
        }

        bool vr_initialized = InitVrSdk();
        if (!vr_initialized) {
          throw std::runtime_error("Failed to initialize the VR SDK. Ensure the required packages are installed.");
        }

        result->Success(flutter::EncodableValue("VR Initialized Successfully"));
      } catch (const std::exception& e) {
        result->Error("VR_INITIALIZATION_ERROR", e.what());
      } });

    async_task.wait(); // Ensure the task completes.
  }

  void FlutterArVrPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &call,
                                           std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (call.method_name().compare("initialize") == 0)
    {
      InitializeVr(std::move(result));
    }
    else
    {
      result->NotImplemented();
    }
  }
} // namespace flutter_ar_vr