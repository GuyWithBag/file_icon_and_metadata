#include "include/file_icon_and_metadata/file_icon_and_metadata_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "file_icon_and_metadata_plugin.h"

void FileIconAndMetadataPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  file_icon_and_metadata::FileIconAndMetadataPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
