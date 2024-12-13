//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <file_icon_and_metadata/file_icon_and_metadata_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) file_icon_and_metadata_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "FileIconAndMetadataPlugin");
  file_icon_and_metadata_plugin_register_with_registrar(file_icon_and_metadata_registrar);
}
