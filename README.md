# OSG Flutter Demo (macOS)

This project demonstrates how to integrate a native C++ renderer (simulating OpenSceneGraph) into a Flutter macOS application using `FlutterTexture` and `CVPixelBuffer`.

## Features

*   **Native Rendering**: Renders content in C++ and displays it efficiently in Flutter.
*   **Interactive Input**:
    *   **Mouse**: Tracks mouse movement, clicks, and scrolling.
    *   **Keyboard**: Captures and displays key events.
*   **Real-time Debugging**:
    *   **FPS Counter**: Displays real-time frames per second.
    *   **Debug Info**: Shows current window size, mouse coordinates, and key codes.
*   **Responsive Layout**: The native renderer automatically resizes to fit the Flutter window.

## Technical Implementation

*   **Flutter Side**: Uses `Texture` widget backed by a native texture ID. Handles input events via `Listener` and `KeyboardListener` and sends them to native code over `MethodChannel`.
*   **Native Side (macOS)**:
    *   Uses `CVPixelBuffer` for high-performance pixel buffer sharing.
    *   Implements a software renderer (for demonstration) that draws directly to the pixel buffer.
    *   Handles RGBA to BGRA conversion and coordinate system adjustments (Y-flip).

## Getting Started

1.  Ensure you have Flutter installed and set up for macOS development.
2.  Run the application:
    ```bash
    flutter run -d macos
    ```

## Controls

*   **Mouse Move**: Move the crosshair.
*   **Mouse Click**: Draw a circle at the cursor location.
*   **Keyboard**: Press keys to see their key codes displayed.
*   **Resize**: Resize the window to see the renderer adapt.
