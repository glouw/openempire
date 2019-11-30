#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Sockets.h"
#include "Overview.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

#define DEMO (0)

static void RunClient(const Args args)
{
    SDL_Init(SDL_INIT_VIDEO);
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
    const Sock sock = Sock_Connect(args.host, args.port);
    int32_t cycles = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        const Field field = Units_Field(units, map);
        const Packet packet = Packet_Get(sock);
        overview = Overview_Update(overview, input);
        Sock_Send(sock, overview);
        units = Units_PacketService(units, data.graphics, packet, grid, map, field);
        Map_Edit(map, overview, grid);
        units = Units_Caretake(units, data.graphics, grid, map, field);
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
        if(args.should_measure && cycles > 10)
            break; // Use valgrind --tool=cachegrind and measure instruction counts when optimizing.
        const int32_t t2 = SDL_GetTicks();
        const int32_t ms = 15 - (t2 - t0);
        if(ms > 0)
            SDL_Delay(ms);
        cycles++;
    }
    Sock_Disconnect(sock);
#endif
    Units_Free(units);
    Units_Free(floats);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
    SDL_Quit();
}

static void RunServer(const Args args)
{
    Sockets sockets = Sockets_Init(args.port);
    for(int32_t cycles = 0; true; cycles++)
    {
        sockets = Sockets_Accept(sockets);
        sockets = Sockets_Service(sockets, 1);
        sockets = Sockets_Relay(sockets, cycles, 100);
    }
    Sockets_Free(sockets);
}

int main(const int argc, const char* argv[])
{
    SDLNet_Init();
    const Args args = Args_Parse(argc, argv);
    args.is_server
        ? RunServer(args)
        : RunClient(args);
    SDLNet_Quit();
}
