library;

import 'package:flutter/services.dart';

class OsgPlugin {
  static const MethodChannel _channel = MethodChannel('com.example.osg/renderer');

  static Future<int?> createRenderer() async {
    final int? textureId = await _channel.invokeMethod('createRenderer');
    return textureId;
  }

  static Future<void> renderFrame() async {
    await _channel.invokeMethod('renderFrame');
  }
}
