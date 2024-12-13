import 'dart:convert';
import 'dart:developer';

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
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<Uint8List?> getFileIcon(String path) async {
    // Use invokeMapMethod when expecting a map to get proper typing.
    final result = await methodChannel
        .invokeMapMethod<String, dynamic>('getFileIcon', {'path': path});

    if (result == null || !result.containsKey('icon')) return null;

    final base64Icon = result['icon'];
    if (base64Icon is! String) return null; // Ensure it's a string

    final iconBytes = base64Decode(base64Icon);
    return iconBytes;
  }
}
