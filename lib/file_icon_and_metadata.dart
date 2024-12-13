
import 'file_icon_and_metadata_platform_interface.dart';

class FileIconAndMetadata {
  Future<String?> getPlatformVersion() {
    return FileIconAndMetadataPlatform.instance.getPlatformVersion();
  }
}
