#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Sockets.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

static void RunClient(const Args args)
{
#define DEMO (1)
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    IPaddress ip;
    const char* const host = "localhost";
    const int32_t port = 1234;
    SDLNet_ResolveHost(&ip, host, port);
    TCPsocket sock = SDLNet_TCP_Open(&ip);
    if(sock == NULL)
        Util_Bomb("Could not connect to %s:%d... Is the openempires server running?\n", host, port);
    const Color color = args.color;
    const Video video = Video_Setup(600, 480, "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(color, video.xres, video.yres);
    Units units = Units_New(grid, video.cpu_count, CONFIG_UNITS_MAX);
    units = Units_GenerateTestZone(units, map, grid, data.graphics);
    Units floats = Units_New(grid, video.cpu_count, 16);
#if DEMO == 0
    Video_RenderDataDemo(video, data, args.color);
#else
    int32_t cycles = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        const Field field = Units_Field(units, map);
        overview = Overview_Update(overview, input);
        SDLNet_TCP_Send(sock, &overview, sizeof(overview)); // XXX. MAYBE make a macro for this?
        Map_Edit(map, overview, grid);
        units = Units_Service(units, data.graphics, overview, grid, map, field);
        units = Units_Caretake(units, grid, map, field);
        floats = Units_Float(floats, data.graphics, overview, grid, map, units.motive);
        Video_Render(video, data, map, units, floats, overview, grid);
        const int32_t t1 = SDL_GetTicks();
        Video_CopyCanvas(video);
        Log_Dump();
        Video_PrintPerformanceMonitor(video, units, t1 - t0, cycles);
        Video_PrintResources(video, units);
        Video_PrintHotkeys(video);
        Video_Present(video);
        Field_Free(field);
        cycles++;
        if(args.should_measure && cycles > 10)
            break; // Use valgrind --tool=cachegrind and measure instruction counts when optimizing.
        const int32_t t2 = SDL_GetTicks();
        const int32_t ms = 15 - (t2 - t0);
        if(ms > 0)
            SDL_Delay(ms);
    }
#endif
    Units_Free(units);
    Units_Free(floats);
    Map_Free(map);
    Data_Free(data);
    SDLNet_TCP_Close(sock);
    SDLNet_Quit();
    Video_Free(video);
#undef DEMO
}

static void RunServer(void)
{
    SDLNet_Init();
    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, 1234);
    TCPsocket server = SDLNet_TCP_Open(&ip);
    Sockets sockets = Sockets_Init();
    for(int32_t cycles = 0; true; cycles++)
    {
        sockets = Sockets_Accept(sockets, server);
        sockets = Sockets_Service(sockets, 1);
        sockets = Sockets_Relay(sockets, cycles, 250);
    }
    Sockets_Free(sockets);
    SDLNet_Quit();
    SDL_Quit();
}

int main(const int argc, const char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    args.is_server
        ? RunServer()
        : RunClient(args);
    return 0;
}
