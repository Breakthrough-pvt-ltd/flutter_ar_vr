#ifndef FLUTTER_PLUGIN_FLUTTER_AR_VR_PLUGIN_H_
#define FLUTTER_PLUGIN_FLUTTER_AR_VR_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace flutter_ar_vr {

class FlutterArVrPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FlutterArVrPlugin();

  virtual ~FlutterArVrPlugin();

  void InitializeVr(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> &result);

  // Disallow copy and assign.
  FlutterArVrPlugin(const FlutterArVrPlugin&) = delete;
  FlutterArVrPlugin& operator=(const FlutterArVrPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace flutter_ar_vr

#endif  // FLUTTER_PLUGIN_FLUTTER_AR_VR_PLUGIN_H_
