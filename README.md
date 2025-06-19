# QTrace

QTrace is a proof of concept project aiming at using concurrency to render fractal images with an emphasis on logic and graphic interfaces separation, using Qt 6 for the logic interface and Qml for the graphic interface. QTrace can easily be extended to handle any kind of raytracing methods.

## Requirements

- Qt 6.x (Core, Gui, Quick, QuickControls2, QuickTemplates2, Widgets, Concurrent)
- CMake 3.5 or later
- C++17 compatible compiler

![Screenshot](http://jwintz.me/assets/img/qtrace.png)

## Build

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

Or using Ninja (faster builds):

```bash
mkdir -p build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

## Apps

### Newton solver

Computes root of z^3-20 in R and roots of z^3-20 in C.

    $ ./bin/qtrNewtonSolver

### Newton writer single threaded

Computes fractal image z^3-1 in C.

    $ time ./bin/qtrNewtonWriter-st 5 800 800 qtrNewton-st.png # [UNIX]
    $ time ./bin/qtrNewtonWriter-st 5 800 800 qtrNewton-st.png && qlmanage -p qtrNewton-st.png >& /dev/null # [MACX]

### Newton writer multi threaded

Computes fractal image z^3-1 in C.

    $ time ./bin/qtrNewtonWriter-mt 3 800 800 qtrNewton-mt.png # [UNIX]
    $ time ./bin/qtrNewtonWriter-mt 3 800 800 qtrNewton-mt.png && qlmanage -p qtrNewton-st.png >& /dev/null # [MAXC]

### Newton viewer

Renders fractal image in viewport multi threaded.

#### Using wrapper application

    $ ./bin/qtrNewtonViewer 5

#### Using qmlscene from source tree root

```bash
qmlscene -I qml/ app/qtrNewtonViewer/main.qml
```

## Docker Development

For a containerized development environment with all dependencies pre-installed, see [README-DOCKER.md](README-DOCKER.md).
