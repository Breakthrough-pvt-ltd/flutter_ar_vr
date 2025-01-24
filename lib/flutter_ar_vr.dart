
import 'flutter_ar_vr_platform_interface.dart';

class FlutterArVr {
  Future<String?> getPlatformVersion() {
    return FlutterArVrPlatform.instance.getPlatformVersion();
  }
}
