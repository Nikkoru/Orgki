# Orgki
Orgki is a daily, weekly, monthly or yearly plan / activity organizer that works locally on Windows and Linux.
> [!NOTE]
> This is pretty much a WIP, don't expect anything. 

## Compiling
For both OS if compiling the GUI version you'll need to get Qt6 or later in order to compile.

The building process is just like any normal CMake program:
```sh
git clone https://github.com/Nikkoru/Orgki
cd Orgki
mkdir build
cd build
cmake ..
cmake --build .
```

You can also use the presets given:
```sh
git clone https://github.com/Nikkoru/Orgki
cd Orgki
cmake --preset [NAME_PRESET]
cmake --build --preset [NAME_PRESET]
```

By default the program builds as a GUI app, but you can build a CLI or a TUI with the given options in the CMakeList.

### Example building the TUI
```sh
cmake .. -DORGKI_TUI=ON
cmake --build .
```
You can also use an IDE that supports CMake, like Visual Studio or CLion.
