import 'flutter_ar_vr_platform_interface.dart';

class FlutterArVr {
  Future<String?> getPlatformVersion() {
    return FlutterArVrPlatform.instance.getPlatformVersion();
  }

  Future<String?> initialize() {
    return FlutterArVrPlatform.instance.initializeVr();
  }

  Future<String?> createVRScene() {
    return FlutterArVrPlatform.instance.createVrScene();
  }

  Future<String?> startVRRendering() {
    return FlutterArVrPlatform.instance.startVrRendering();
  }
}
