import 'package:flutter_test/flutter_test.dart';
import 'package:file_icon_and_metadata/file_icon_and_metadata.dart';
import 'package:file_icon_and_metadata/file_icon_and_metadata_platform_interface.dart';
import 'package:file_icon_and_metadata/file_icon_and_metadata_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockFileIconAndMetadataPlatform
    with MockPlatformInterfaceMixin
    implements FileIconAndMetadataPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final FileIconAndMetadataPlatform initialPlatform = FileIconAndMetadataPlatform.instance;

  test('$MethodChannelFileIconAndMetadata is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFileIconAndMetadata>());
  });

  test('getPlatformVersion', () async {
    FileIconAndMetadata fileIconAndMetadataPlugin = FileIconAndMetadata();
    MockFileIconAndMetadataPlatform fakePlatform = MockFileIconAndMetadataPlatform();
    FileIconAndMetadataPlatform.instance = fakePlatform;

    expect(await fileIconAndMetadataPlugin.getPlatformVersion(), '42');
  });
}
