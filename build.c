#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "build.h"

Cmd cmd = {0};

int main()
{
    auto_rebuild(__FILE__);

    push(&cmd, "cc");
    push(&cmd, "-O3");
    push(&cmd, "-Wall");
    push(&cmd, "-Wextra");
    push(&cmd, "-I./raylib-5.5_macos/include");
    push(&cmd, "-L./raylib-5.5_macos/lib");
    push(&cmd, "-Wl,-rpath,@executable_path/raylib-5.5_macos/lib");
    push(&cmd, "-o", "plasma", "plasma.c");
    push(&cmd, "-lraylib");
    push(&cmd, "-lm");
    push(&cmd, "-framework", "OpenGL");
    push(&cmd, "-framework", "Cocoa");
    push(&cmd, "-framework", "IOKit");
    push(&cmd, "-framework", "CoreVideo");

    if (!run_always(&cmd)) return 1;

    return 0;
}
