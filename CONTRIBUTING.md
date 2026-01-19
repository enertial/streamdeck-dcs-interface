# Contributing to DCS Interface

Thank you for your interest in contributing to this project!

## Fork Information

This fork adds Stream Deck Plus rotary encoder support to the original project.

### Original Project

- Original Repository: [charlestytler/streamdeck-dcs-interface](https://github.com/charlestytler/streamdeck-dcs-interface)
- License: GNU General Public License v3.0

## How to Contribute

1. **Fork the repository** on GitHub
2. **Clone your fork** locally
3. **Create a feature branch** (`git checkout -b feature/my-new-feature`)
4. **Make your changes** and commit them with clear messages
5. **Test your changes** thoroughly
6. **Push to your fork** (`git push origin feature/my-new-feature`)
7. **Submit a Pull Request** to the original repository

## Development Guidelines

### Building from Source

Before building, ensure you have:
- Microsoft Visual Studio 2022 (or Build Tools with Platform Toolset v145)
- MSBuild added to your PATH
- npm for Windows

Run the build script:
```batch
cd Tools
.\build_plugin.bat
```

### Code Style

- C++: Follow existing code conventions in the project
- JavaScript/HTML: Use consistent indentation and formatting
- Add comments for complex logic

### Testing

- Test all functionality with actual Stream Deck hardware
- Verify DCS integration with multiple aircraft modules
- Run unit tests before submitting (`Test.exe` generated during build)

## Reporting Issues

When reporting issues, please include:
- Stream Deck model and firmware version
- DCS World version
- Steps to reproduce the issue
- Expected vs actual behavior
- Any error messages or logs

## License

By contributing, you agree that your contributions will be licensed under the GNU General Public License v3.0, consistent with the original project.

## Acknowledgments

This project builds upon the excellent work of the original author and contributors. All modifications respect the original GPL-3.0 license terms.
