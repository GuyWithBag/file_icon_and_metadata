import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'file_icon_and_metadata_platform_interface.dart';

/// An implementation of [FileIconAndMetadataPlatform] that uses method channels.
class MethodChannelFileIconAndMetadata extends FileIconAndMetadataPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('file_icon_and_metadata');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
