#include "include/osg_plugin/osg_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "osg_plugin.h"

void OsgPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  osg_plugin::OsgPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
