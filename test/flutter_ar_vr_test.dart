import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_ar_vr/flutter_ar_vr.dart';
import 'package:flutter_ar_vr/flutter_ar_vr_platform_interface.dart';
import 'package:flutter_ar_vr/flutter_ar_vr_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockFlutterArVrPlatform
    with MockPlatformInterfaceMixin
    implements FlutterArVrPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  Future<String?> initializeVr() {
    // TODO: implement initializeVr
    throw UnimplementedError();
  }
}

void main() {
  final FlutterArVrPlatform initialPlatform = FlutterArVrPlatform.instance;

  test('$MethodChannelFlutterArVr is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFlutterArVr>());
  });

  test('getPlatformVersion', () async {
    FlutterArVr flutterArVrPlugin = FlutterArVr();
    MockFlutterArVrPlatform fakePlatform = MockFlutterArVrPlatform();
    FlutterArVrPlatform.instance = fakePlatform;

    expect(await flutterArVrPlugin.getPlatformVersion(), '42');
  });
}
