## Building using CMake

You'll need [CMake](https://cmake.org). You can then run CMake, the normal way:

```console
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

**NOTE**: You must create a separate directory for the build artifacts (e.g. `build` above).
Running `cmake` from the repo root directory will not work since the build produces an
executable called `wasm2c` which conflicts with the `wasm2c` directory.