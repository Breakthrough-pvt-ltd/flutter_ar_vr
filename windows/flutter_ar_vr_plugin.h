#ifndef FLUTTER_AR_VR_PLUGIN_H_
#define FLUTTER_AR_VR_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>

namespace flutter_ar_vr
{

  class FlutterArVrPlugin : public flutter::Plugin
  {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    FlutterArVrPlugin(flutter::PluginRegistrarWindows *registrar);
    ~FlutterArVrPlugin();

  private:
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    void InitializeVr(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    bool IsVrDeviceConnected();
    bool InitVrSdk();

    flutter::PluginRegistrarWindows *registrar_;
  };

} // namespace flutter_ar_vr

#endif // FLUTTER_AR_VR_PLUGIN_H_