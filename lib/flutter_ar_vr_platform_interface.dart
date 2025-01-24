import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'flutter_ar_vr_method_channel.dart';

abstract class FlutterArVrPlatform extends PlatformInterface {
  /// Constructs a FlutterArVrPlatform.
  FlutterArVrPlatform() : super(token: _token);

  static final Object _token = Object();

  static FlutterArVrPlatform _instance = MethodChannelFlutterArVr();

  /// The default instance of [FlutterArVrPlatform] to use.
  ///
  /// Defaults to [MethodChannelFlutterArVr].
  static FlutterArVrPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FlutterArVrPlatform] when
  /// they register themselves.
  static set instance(FlutterArVrPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
