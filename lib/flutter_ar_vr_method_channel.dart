import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'flutter_ar_vr_platform_interface.dart';

/// An implementation of [FlutterArVrPlatform] that uses method channels.
class MethodChannelFlutterArVr extends FlutterArVrPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('flutter_ar_vr');

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<String?> initializeVr() async {
    try {
      final result = await methodChannel.invokeMethod<String>('initialize');
      return result ?? 'Unknown VR status';
    } on PlatformException catch (e) {
      throw PlatformException(
        code: 'VR_INITIALIZATION_ERROR',
        message: e.message,
      );
    }
  }

  @override
  Future<String?> createVrScene() async {
    try {
      return await methodChannel.invokeMethod<String>('createVrScene');
    } on PlatformException catch (e) {
      throw PlatformException(code: 'VR_SCENE_ERROR', message: e.message);
    }
  }

  @override
  Future<String?> startVrRendering() async {
    try {
      return await methodChannel.invokeMethod<String>('startVrRendering');
    } on PlatformException catch (e) {
      throw PlatformException(code: 'VR_RENDERING_ERROR', message: e.message);
    }
  }
}
