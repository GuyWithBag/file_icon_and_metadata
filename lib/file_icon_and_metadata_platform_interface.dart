import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'file_icon_and_metadata_method_channel.dart';

abstract class FileIconAndMetadataPlatform extends PlatformInterface {
  /// Constructs a FileIconAndMetadataPlatform.
  FileIconAndMetadataPlatform() : super(token: _token);

  static final Object _token = Object();

  static FileIconAndMetadataPlatform _instance = MethodChannelFileIconAndMetadata();

  /// The default instance of [FileIconAndMetadataPlatform] to use.
  ///
  /// Defaults to [MethodChannelFileIconAndMetadata].
  static FileIconAndMetadataPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FileIconAndMetadataPlatform] when
  /// they register themselves.
  static set instance(FileIconAndMetadataPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
