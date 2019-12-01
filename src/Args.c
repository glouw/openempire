#include "Args.h"

#include "Util.h"

#include <assert.h>

static bool Check(const char* const arg, const char* const full, const char* const hyphen)
{
    return Util_StringEqual(arg, full)
        || Util_StringEqual(arg, hyphen);
}

Args Args_Parse(const int32_t argc, const char* argv[])
{
    static Args zero;
    Args args = zero;
    args.path = "/home/gl/.steam/steam/steamapps/common/Age2HD/Directory/data/";
    args.color = COLOR_BLU;
    args.host = "localhost";
    args.port = 1234;
    for(int32_t i = 0; i < argc; i++)
    {
        const char* const arg = argv[i];
        if(Check(arg, "--color", "-c"))
        {
            if(i != argc - 1)
            {
                const Color color = (Color) atoi(argv[++i]);
                if(color < COLOR_COUNT)
                    args.color = color;
            }
        }
        else
        if(Check(arg, "--path", "-p"))
            args.path = argv[i + 1];
        else
        if(Check(arg, "--server", "-s"))
            args.is_server = true;

    }
    assert(args.path);
    return args;
}
