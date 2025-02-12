#ifndef FLUTTER_PLUGIN_FLUTTER_AR_VR_PLUGIN_H_
#define FLUTTER_PLUGIN_FLUTTER_AR_VR_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace flutter_ar_vr {

class FlutterArVrPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FlutterArVrPlugin(flutter::PluginRegistrarWindows* registrar);  // Constructor updated

  virtual ~FlutterArVrPlugin();

  void InitializeVr(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> &result);

  // New method to initialize VR SDK (e.g., OpenXR or another VR SDK)
  bool InitVrSdk();

  // Disallow copy and assign.
  FlutterArVrPlugin(const FlutterArVrPlugin&) = delete;
  FlutterArVrPlugin& operator=(const FlutterArVrPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  void CreateVrScene(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> &result);
  void StartVrRendering(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> &result);
  void CreateBasicCamera();
  void CreateBasicObjects();
  
  void StartRenderingLoop();
  bool IsVrDeviceConnected();
  void HandleVrDisconnection();
  
 private:
  flutter::PluginRegistrarWindows* registrar_;  // Store the registrar
};

}  // namespace flutter_ar_vr

#endif  // FLUTTER_PLUGIN_FLUTTER_AR_VR_PLUGIN_H_
