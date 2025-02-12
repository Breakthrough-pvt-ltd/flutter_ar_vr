#include "flutter_ar_vr_plugin.h"

// Include necessary Windows and OpenXR headers.
#include <windows.h>
#include <VersionHelpers.h>

// OpenXR SDK header
#include <openxr/openxr.h>  
#include <openxr/openxr_platform.h>

// Include Flutter plugin headers.
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <future>  // Include for async functionality
#include <string>

#include <tchar.h>  // Include for safe C string functions

namespace flutter_ar_vr {

// Constructor to initialize the registrar.
FlutterArVrPlugin::FlutterArVrPlugin(flutter::PluginRegistrarWindows* registrar)
    : registrar_(registrar) {}

FlutterArVrPlugin::~FlutterArVrPlugin() {}

void FlutterArVrPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows* registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "flutter_ar_vr",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FlutterArVrPlugin>(registrar);  // Pass registrar to constructor.

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto& call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

bool isOpenXRInstalled() {
    HKEY hKey;
    const std::string registryPath = "SOFTWARE\\Khronos\\OpenXR\\1";
    const std::string registryKey = "ActiveRuntime";

    // Open the registry key
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key." << std::endl;
        return false;
    }

    // Query the "ActiveRuntime" value
    char runtimePath[MAX_PATH];
    DWORD bufferSize = sizeof(runtimePath);
    if (RegQueryValueExA(hKey, registryKey.c_str(), NULL, NULL, (LPBYTE)runtimePath, &bufferSize) != ERROR_SUCCESS) {
        std::cerr << "Error reading registry value." << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    // Check if a runtime path is found
    if (strlen(runtimePath) == 0) {
        std::cerr << "OpenXR runtime not found." << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    std::cout << "OpenXR runtime found at: " << runtimePath << std::endl;
    RegCloseKey(hKey);
    return true;
}

bool initializeOpenXR() {
    // Step 1: Check if OpenXR is installed
    if (!isOpenXRInstalled()) {
        std::cout << "OpenXR not installed. Attempting to install..." << std::endl;
        // installOpenXR();
        
        // You may need to restart the application or prompt the user to restart
        return false;
    }

    // Step 2: Initialize OpenXR functionality (add your existing setup here)
    std::cout << "OpenXR is installed. Proceeding with initialization..." << std::endl;

    // Proceed with your AR/VR setup...
    return true;
}

bool FlutterArVrPlugin::IsVrDeviceConnected() {
  XrInstance xr_instance = XR_NULL_HANDLE;  // Ensure null initialization

  // OpenXR instance creation
  XrInstanceCreateInfo instance_create_info = {};
  instance_create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.next = nullptr;
  instance_create_info.createFlags = 0;
  
  strncpy_s(instance_create_info.applicationInfo.applicationName, "Flutter AR VR Plugin", XR_MAX_APPLICATION_NAME_SIZE);
  instance_create_info.applicationInfo.applicationVersion = 1;
  strncpy_s(instance_create_info.applicationInfo.engineName, "Custom Engine", XR_MAX_ENGINE_NAME_SIZE);
  instance_create_info.applicationInfo.engineVersion = 1;
  instance_create_info.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);

  XrResult result = xrCreateInstance(&instance_create_info, &xr_instance);
  if (XR_FAILED(result)) {
    std::cerr << "Failed to initialize OpenXR instance: " << result << std::endl;
    return false;
  }

  // VR system detection
  XrSystemId system_id = XR_NULL_SYSTEM_ID;
  XrSystemGetInfo system_info = {};
  system_info.type = XR_TYPE_SYSTEM_GET_INFO;
  system_info.next = nullptr;
  system_info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

  result = xrGetSystem(xr_instance, &system_info, &system_id);
  if (XR_FAILED(result)) {
    std::cerr << "Failed to detect VR system: " << result << std::endl;
    xrDestroyInstance(xr_instance);  // Clean up before returning
    return false;
  }

  std::cout << "VR Device detected!" << std::endl;

  // Clean up
  xrDestroyInstance(xr_instance);
  return true;
}

bool FlutterArVrPlugin::InitVrSdk() {
  std::cout << "Initializing OpenXR SDK..." << std::endl;

  // Instance creation and system selection logic remains unchanged.
  XrInstanceCreateInfo instance_create_info = {};
  instance_create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
  strncpy_s(instance_create_info.applicationInfo.applicationName, sizeof(instance_create_info.applicationInfo.applicationName), "Flutter AR VR Plugin", _TRUNCATE);

  XrInstance instance = XR_NULL_HANDLE;

  XrResult result = xrCreateInstance(&instance_create_info, &instance);
  if (XR_FAILED(result)) {
    std::cerr << "Failed to initialize OpenXR instance: " << result << std::endl;
    return false;
  }

  XrSystemId system_id;
  XrSystemGetInfo system_info = {};
  system_info.type = XR_TYPE_SYSTEM_GET_INFO;
  system_info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

  result = xrGetSystem(instance, &system_info, &system_id);
  if (XR_FAILED(result)) {
    std::cerr << "Failed to get OpenXR system: " << result << std::endl;
    return false;
  }

  XrSessionCreateInfo session_create_info = {};
  session_create_info.type = XR_TYPE_SESSION_CREATE_INFO;
  session_create_info.systemId = system_id;

  XrSession session = XR_NULL_HANDLE;
  result = xrCreateSession(instance, &session_create_info, &session);
  if (XR_FAILED(result)) {
    std::cerr << "Failed to create OpenXR session: " << result << std::endl;
    return false;
  }

  XrReferenceSpaceCreateInfo reference_space_create_info = {};
  reference_space_create_info.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
  reference_space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;

  XrSpace reference_space = XR_NULL_HANDLE;
  result = xrCreateReferenceSpace(session, &reference_space_create_info, &reference_space);
  if (XR_FAILED(result)) {
    std::cerr << "Failed to create reference space: " << result << std::endl;
    return false;
  }

  return true;
}

void FlutterArVrPlugin::InitializeVr(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>>& result) {
  auto async_task = std::async(std::launch::async, [this, result = std::move(result)] () mutable {
    try {
      // Removed the initial success call
      if (!initializeOpenXR())
      {
        throw std::runtime_error("OpenXR not installed.");
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
    }
  });
}

void FlutterArVrPlugin::StartVrRendering(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>>& result) {
  // Asynchronous rendering loop
  auto async_task = std::async(std::launch::async, [this, result = std::move(result)] () mutable {
    try {
      result->Success(flutter::EncodableValue("Starting VR Rendering..."));

      StartRenderingLoop();  // Start rendering asynchronously.

      result->Success(flutter::EncodableValue("VR Rendering Started Successfully"));
    } catch (const std::exception& e) {
      result->Error("VR_RENDERING_ERROR", e.what());
    }
  });
}

void FlutterArVrPlugin::CreateVrScene(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>>& result) {
  try {
    // Add logic to create VR scene (e.g., loading models, setting up camera).
    std::cout << "Creating VR Scene..." << std::endl;
    result->Success(flutter::EncodableValue("VR Scene Created Successfully"));
  } catch (const std::exception& e) {
    result->Error("VR_SCENE_CREATION_ERROR", e.what());
  }
}

void FlutterArVrPlugin::StartRenderingLoop() {
  try {
    std::cout << "Starting VR Rendering Loop..." << std::endl;
    // Start rendering loop for VR (e.g., continuously updating the scene and rendering).
    // Add your VR rendering logic here (OpenXR rendering, etc.).
  } catch (const std::exception& e) {
    std::cerr << "Error in rendering loop: " << e.what() << std::endl;
  }
}

void FlutterArVrPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue>& method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {

  if (method_call.method_name().compare("initialize") == 0) {
    InitializeVr(result);
  } else if (method_call.method_name().compare("createVrScene") == 0) {
    CreateVrScene(result);
  } else if (method_call.method_name().compare("startVrRendering") == 0) {
    StartVrRendering(result);
  } else if (method_call.method_name().compare("getPlatformVersion") == 0) {
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
  } else {
    result->NotImplemented();
  }
}

}  // namespace flutter_ar_vr
