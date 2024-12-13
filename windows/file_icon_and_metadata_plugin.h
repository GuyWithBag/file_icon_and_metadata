#ifndef FLUTTER_PLUGIN_FILE_ICON_AND_METADATA_PLUGIN_H_
#define FLUTTER_PLUGIN_FILE_ICON_AND_METADATA_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace file_icon_and_metadata {

class FileIconAndMetadataPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FileIconAndMetadataPlugin();

  virtual ~FileIconAndMetadataPlugin();

  // Disallow copy and assign.
  FileIconAndMetadataPlugin(const FileIconAndMetadataPlugin&) = delete;
  FileIconAndMetadataPlugin& operator=(const FileIconAndMetadataPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace file_icon_and_metadata

#endif  // FLUTTER_PLUGIN_FILE_ICON_AND_METADATA_PLUGIN_H_
