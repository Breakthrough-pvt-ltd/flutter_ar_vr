#include "flutter_ar_vr_plugin.h"

// Include Windows headers.
#include <windows.h>
#include <VersionHelpers.h>

// Include Flutter plugin headers.
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <stdexcept>

namespace flutter_ar_vr {

// Static: Register the plugin with the registrar.
void FlutterArVrPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "flutter_ar_vr",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<FlutterArVrPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

FlutterArVrPlugin::FlutterArVrPlugin() {}

FlutterArVrPlugin::~FlutterArVrPlugin() {}

void FlutterArVrPlugin::InitializeVr(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> &result) {
  try {
    // Check if required packages or runtime is available.
    if (!IsWindows10OrGreater()) {
      throw std::runtime_error("Windows 10 or higher is required for VR functionality.");
    }

    // Add initialization logic for the VR SDK (OpenXR, OpenVR, etc.).
    bool vr_initialized = false;
    // Example VR initialization placeholder:
    // vr_initialized = InitVrSdk();

    if (!vr_initialized) {
      throw std::runtime_error("Failed to initialize the VR SDK. Ensure the required packages are installed.");
    }

    result->Success(flutter::EncodableValue("VR Initialized Successfully"));
  } catch (const std::exception &e) {
    result->Error("VR_INITIALIZATION_ERROR", e.what());
  }
}

void FlutterArVrPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("initialize") == 0) {
    InitializeVr(result);
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
