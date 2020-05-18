#include "Args.h"

#include "Util.h"

Args Args_Parse(const int32_t argc, const char* argv[])
{
    static Args zero;
    Args args = zero;
    args.color = COLOR_BLU;
    args.host = "localhost";
    args.port = 1234;
    args.xres = 800;
    args.yres = 600;
    args.users = 1;
    args.map_size = 64;
    for(int32_t i = 0; i < argc; i++)
    {
        const char* const arg = argv[i];
        const char* const next = argv[i + 1];
        if(Util_StringEqual(arg, "--color"        )) args.color = (Color) atoi(next);
        if(Util_StringEqual(arg, "--path"         )) args.path = next;
        if(Util_StringEqual(arg, "--xres"         )) args.xres = atoi(next);
        if(Util_StringEqual(arg, "--yres"         )) args.yres = atoi(next);
        if(Util_StringEqual(arg, "--users"        )) args.users = atoi(next);
        if(Util_StringEqual(arg, "--map_size"     )) args.map_size = atoi(next);
        if(Util_StringEqual(arg, "--host"         )) args.host = next;
        if(Util_StringEqual(arg, "--port"         )) args.port = atoi(next);
        if(Util_StringEqual(arg, "--measure"      )) args.must_measure = true;
        if(Util_StringEqual(arg, "--quiet"        )) args.is_quiet = true;
        if(Util_StringEqual(arg, "--server"       )) args.is_server = true;
        if(Util_StringEqual(arg, "--demo"         )) args.is_demo = true;
        if(Util_StringEqual(arg, "--randomize"    )) args.must_randomize_mouse = true;
        if(Util_StringEqual(arg, "--slow_download")) args.must_simulate_slow_download = true;
    }
    args.port_ping  = args.port + 1;
    args.port_reset = args.port + 2;
    return args;
}
