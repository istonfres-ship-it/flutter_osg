import 'package:flutter/gestures.dart';
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
  Size? _currentSize;
  final FocusNode _focusNode = FocusNode();

  @override
  void initState() {
    super.initState();
  }
  
  @override
  void dispose() {
    _focusNode.dispose();
    super.dispose();
  }

  Future<void> _initializeRenderer(int width, int height) async {
    if (_isInitialized) return;
    
    try {
      final int textureId = await _channel.invokeMethod('createRenderer', {
        'width': width,
        'height': height,
      });
      setState(() {
        _textureId = textureId;
        _isInitialized = true;
        _currentSize = Size(width.toDouble(), height.toDouble());
      });
      _startRenderingLoop();
    } on PlatformException catch (e) {
      debugPrint("Failed to initialize renderer: '${e.message}'.");
    }
  }
  
  Future<void> _resize(int width, int height) async {
    if (!_isInitialized) return;
    if (_currentSize?.width.toInt() == width && _currentSize?.height.toInt() == height) return;
    
    try {
      await _channel.invokeMethod('resize', {
        'width': width,
        'height': height,
      });
      setState(() {
        _currentSize = Size(width.toDouble(), height.toDouble());
      });
    } on PlatformException catch (e) {
      debugPrint("Failed to resize: '${e.message}'.");
    }
  }

  void _startRenderingLoop() {
    if (!_isInitialized) return;
    
    Future.doWhile(() async {
      if (!mounted) return false;
      await _channel.invokeMethod('renderFrame');
      await Future.delayed(const Duration(milliseconds: 16));
      return true;
    });
  }
  
  void _onMouseMove(PointerEvent event) {
    if (!_isInitialized) return;
    _channel.invokeMethod('mouseMove', {
      'x': event.localPosition.dx.toInt(),
      'y': event.localPosition.dy.toInt(),
    });
  }
  
  void _onMouseDown(PointerEvent event) {
    if (!_isInitialized) return;
    _focusNode.requestFocus();
    _channel.invokeMethod('mouseButton', {
      'button': event.buttons,
      'pressed': true,
      'x': event.localPosition.dx.toInt(),
      'y': event.localPosition.dy.toInt(),
    });
  }
  
  void _onMouseUp(PointerEvent event) {
    if (!_isInitialized) return;
    _channel.invokeMethod('mouseButton', {
      'button': 0,
      'pressed': false,
      'x': event.localPosition.dx.toInt(),
      'y': event.localPosition.dy.toInt(),
    });
  }
  
  void _onScroll(PointerScrollEvent event) {
    if (!_isInitialized) return;
    _channel.invokeMethod('scroll', {
      'deltaX': event.scrollDelta.dx,
      'deltaY': event.scrollDelta.dy,
    });
  }
  
  void _onKeyEvent(KeyEvent event) {
    if (!_isInitialized) return;
    _channel.invokeMethod('keyEvent', {
      'keyCode': event.logicalKey.keyId,
      'pressed': event is KeyDownEvent,
    });
  }

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
      builder: (context, constraints) {
        final width = constraints.maxWidth.toInt();
        final height = constraints.maxHeight.toInt();
        
        if (width > 0 && height > 0) {
          if (!_isInitialized) {
            WidgetsBinding.instance.addPostFrameCallback((_) {
              _initializeRenderer(width, height);
            });
          } else if (_currentSize?.width.toInt() != width || 
                     _currentSize?.height.toInt() != height) {
            WidgetsBinding.instance.addPostFrameCallback((_) {
              _resize(width, height);
            });
          }
        }
        
        if (!_isInitialized || _textureId == null) {
          return const Center(child: CircularProgressIndicator());
        }
        
        return KeyboardListener(
          focusNode: _focusNode,
          autofocus: true,
          onKeyEvent: _onKeyEvent,
          child: Listener(
            onPointerHover: _onMouseMove,
            onPointerMove: _onMouseMove,
            onPointerDown: _onMouseDown,
            onPointerUp: _onMouseUp,
            onPointerSignal: (event) {
              if (event is PointerScrollEvent) {
                _onScroll(event);
              }
            },
            child: SizedBox(
              width: constraints.maxWidth,
              height: constraints.maxHeight,
              child: Texture(textureId: _textureId!),
            ),
          ),
        );
      },
    );
  }
}
