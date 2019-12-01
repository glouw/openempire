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
    const Video video = Video_Setup(640, 480, "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(color, video.xres, video.yres);
    Units units = Units_New(grid, video.cpu_count, CONFIG_UNITS_MAX);
    units = Units_GenerateTestZone(units, map, grid, data.graphics);
    Units floats = Units_New(grid, video.cpu_count, 16);
    if(DEMO == 1)
    {
        // The demo renderer shows a preview of all unit and interface graphics.
        // A constant is used to compile in the demo renderer. Choosing
        // to set DEMO to 0 will remove the demo renderer from the final binary.
        Video_RenderDataDemo(video, data, args.color);
    }
    else
    {
        const Sock sock = Sock_Connect(args.host, args.port);
        int32_t cycles = 0;
        for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
        {
            const int32_t t0 = SDL_GetTicks();

            // The client is responsible for only updating the server with its own overview.
            const uint64_t parity = Units_Xor(units);
            overview = Overview_Update(overview, input, parity, cycles);
            Sock_Send(sock, overview);

            // The server returns all client overviews.
            const Packet packet = Packet_Get(sock);

            // Client units are updated according to all client overviews.
            const Field field = Units_Field(units, map);
            units = Units_PacketService(units, data.graphics, packet, grid, map, field);
            units = Units_Caretake(units, data.graphics, grid, map, field);
            floats = Units_Float(floats, data.graphics, overview, grid, map, units.motive);
            cycles++;

            // XXX. Just for fun right now.
            Map_Edit(map, overview, grid);

            // The server will send a speed up control character if the client falls behind.
            // The quickest way for the client to catch up with the other clients
            // is to skip renderering the current scene.
            if(packet.control == PACKET_CONTROL_SPEED_UP)
                continue;

            Video_Render(video, data, map, units, floats, overview, grid);
            const int32_t t1 = SDL_GetTicks();

            Video_CopyCanvas(video);
            Log_Dump();
            Video_PrintPerformanceMonitor(video, units, t1 - t0, cycles);
            Video_PrintResources(video, units);
            Video_PrintHotkeys(video);
            Video_Present(video);
            Field_Free(field);

            // The client will attempt to roughly maintain a 60 FPS game loop,
            // regardless of screen vertical sync rates.
            const int32_t t2 = SDL_GetTicks();
            const int32_t ms = 15 - (t2 - t0);
            if(ms > 0)
                SDL_Delay(ms);
        }
        Sock_Disconnect(sock);
    }
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
    printf("%ld\n", sizeof(Packet));
    SDLNet_Init();
    const Args args = Args_Parse(argc, argv);
    args.is_server
        ? RunServer(args)
        : RunClient(args);
    SDLNet_Quit();
}
