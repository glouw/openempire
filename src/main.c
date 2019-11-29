#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Sockets.h"
#include "Overview.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

static void RunClient(const Args args)
{
#define DEMO (0)
    SDL_Init(SDL_INIT_EVERYTHING);
    const Color color = args.color;
    const Video video = Video_Setup(800, 600, "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(color, video.xres, video.yres);
    Units units = Units_New(grid, video.cpu_count, CONFIG_UNITS_MAX);
    units = Units_GenerateTestZone(units, map, grid, data.graphics);
    Units floats = Units_New(grid, video.cpu_count, 16);
#if DEMO == 1
    Video_RenderDataDemo(video, data, args.color);
#else
    SDLNet_Init();
    IPaddress ip;
    SDLNet_ResolveHost(&ip, args.host, args.port);
    TCPsocket server = SDLNet_TCP_Open(&ip);
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(set, server);
    if(server == NULL)
        Util_Bomb("Could not connect to %s:%d... Is the openempires server running?\n", args.host, args.port);
    int32_t cycles = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        const Field field = Units_Field(units, map);
        overview = Overview_Update(overview, input);
        SDLNet_TCP_Send(server, &overview, sizeof(overview));
        const Packet packet = Packet_Get(server, set);
        for(int32_t i = 0; i < COLOR_COUNT; i++)
            units = Units_Service(units, data.graphics, packet.overview[i], grid, map, field);
        Map_Edit(map, overview, grid);
        units = Units_Caretake(units, data.graphics, overview, grid, map, field);
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
    SDLNet_FreeSocketSet(set);
    SDLNet_TCP_Close(server);
#endif
    Units_Free(units);
    Units_Free(floats);
    Map_Free(map);
    Data_Free(data);
    SDLNet_Quit();
    Video_Free(video);
#undef DEMO
}

static void RunServer(const Args args)
{
    SDLNet_Init();
    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, args.port);
    TCPsocket server = SDLNet_TCP_Open(&ip);
    Sockets clients = Sockets_Init();
    for(int32_t cycles = 0; true; cycles++)
    {
        clients = Sockets_Accept(clients, server);
        clients = Sockets_Service(clients, 1);
        clients = Sockets_Relay(clients, cycles, 100);
    }
    Sockets_Free(clients);
    SDLNet_Quit();
    SDL_Quit();
}

int main(const int argc, const char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    args.is_server
        ? RunServer(args)
        : RunClient(args);
    return 0;
}
