# Game UI Components

A collection of custom UI components and animation systems for game development, built on top of ImGui.

## 📁 Project Structure

```
ui/
├── animations/               # Loading animation system
│   ├── injection_anim.h   # Animation state definitions
│   └── injection_anim.cpp # Animation implementation
├── components/                 # UI component library
│   ├── Custom.h           # Custom ImGui widgets (buttons, switches, tabs, etc.)
│   ├── MenuRingLogo.cpp   # Gradient ring logo renderer
│   └── embedded_overlay_dex.h # Embedded DEX bytecode data
└── README.md              # This file
```

## 🎯 Features

### Loading Animation System
- Multi-stage cinematic animation with smooth easing functions
- Particle effects, ring animations, and text transitions
- Configurable timing and visual parameters

### UI Component Library
- **Winter Client-style widgets**: Buttons, switches, tabs with smooth animations
- **Notification system**: Toast notifications with auto-dismiss
- **Capsule radio controls**: Sleek segmented selection controls
- **Custom child windows**: Styled containers with gradient headers

## 🚀 Usage

### Prerequisites
- ImGui library
- C++17 or later
- Graphics API (OpenGL, DirectX, Vulkan, etc.)

### Integration
1. Include the header files in your project
2. Link against ImGui
3. Call the rendering functions in your render loop

```cpp
// Example: Drawing the ring logo
#include "components/MenuRingLogo.h"

void RenderUI() {
    // ... setup code ...
    DrawRingLogo(draw_list, center, radius, scale, alpha, bg_color, aspect_ratio, rotation);
}
```

## 📝 Code Style

- Chinese comments explain complex algorithms and visual effects
- Consistent formatting with 4-space indentation
- Descriptive variable names for animation parameters
- Mathematical formulas use LaTeX-style notation where helpful

## 📄 License

This project is open source. Please check individual files for specific licensing information.

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

For major changes, please open an issue first to discuss what you would like to change.