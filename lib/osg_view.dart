import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class OsgView extends StatefulWidget {
  const OsgView({super.key});

  @override
  State<OsgView> createState() => _OsgViewState();
}

class _OsgViewState extends State<OsgView> {
  static const MethodChannel _channel = MethodChannel('com.example.osg/renderer');
  int? _textureId;
  bool _isInitialized = false;

  @override
  void initState() {
    super.initState();
    _initializeRenderer();
  }

  Future<void> _initializeRenderer() async {
    try {
      final int textureId = await _channel.invokeMethod('createRenderer');
      setState(() {
        _textureId = textureId;
        _isInitialized = true;
      });
      _startRenderingLoop();
    } on PlatformException catch (e) {
      debugPrint("Failed to initialize renderer: '${e.message}'.");
    }
  }

  void _startRenderingLoop() {
    // In a real app, you might drive this with Ticker or native vsync
    // For this example, we'll just ask the native side to render continuously
    // or rely on the native side to drive the texture updates.
    // Here we assume native side pushes updates or we trigger them.
    // Let's trigger a frame update periodically for demonstration.
    if (!_isInitialized) return;
    
    // Simple loop to request frames
    Future.doWhile(() async {
      if (!mounted) return false;
      await _channel.invokeMethod('renderFrame');
      await Future.delayed(const Duration(milliseconds: 16)); // ~60fps target
      return true;
    });
  }

  @override
  Widget build(BuildContext context) {
    return _isInitialized && _textureId != null
        ? Texture(textureId: _textureId!)
        : const Center(child: CircularProgressIndicator());
  }
}
