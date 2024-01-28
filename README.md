# Virtual Resonators
VirtualResonators is a VST3 plugin that models sympathetic strings using the Karplus-Strong algorithm.

## Building
After cloning the repository, run these commands:
```
git submodule update --init --recursive
mkdir Build
cmake -B Build
cmake --build Build
```
Plugin will be located in `%ProjectFolder%/Build/VirtualResonators_artefacts/VST3/` (or located in `%ProjectFolder%\Build\VirtualResonators_artefacts\Release\VST3\` on Windows)
