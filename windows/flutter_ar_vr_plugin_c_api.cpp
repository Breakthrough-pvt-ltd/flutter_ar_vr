#include "include/flutter_ar_vr/flutter_ar_vr_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "flutter_ar_vr_plugin.h"

void FlutterArVrPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  flutter_ar_vr::FlutterArVrPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
