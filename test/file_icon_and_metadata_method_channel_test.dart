import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:file_icon_and_metadata/file_icon_and_metadata_method_channel.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  MethodChannelFileIconAndMetadata platform = MethodChannelFileIconAndMetadata();
  const MethodChannel channel = MethodChannel('file_icon_and_metadata');

  setUp(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(
      channel,
      (MethodCall methodCall) async {
        return '42';
      },
    );
  });

  tearDown(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(channel, null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
