#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Packets.h"
#include "Ping.h"
#include "Sockets.h"
#include "Overview.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

static Overview WaitInLobby(const Video video, const Sock sock, int32_t* users)
{
    int32_t loops = 0;
    Overview overview = Overview_Init(video.xres, video.yres);
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        Sock_Send(sock, overview);
        const Packet packet = Packet_Get(sock);
        if(packet.turn > 0)
        {
            overview.share.color = (Color) packet.client_id;
            Video_PrintLobby(video, packet.users_connected, packet.users, overview.share.color, loops++);
            if(packet.game_running)
            {
                *users = packet.users;
                break;
            }
        }
        SDL_Delay(CONFIG_MAIN_LOOP_SPEED_MS);
    }
    return overview;
}

static void Play(const Video video, const Data data, const Map map, const Grid grid, const Args args)
{
    int32_t users = 0;
    const Sock sock = Sock_Connect(args.host, args.port);
    Overview overview = WaitInLobby(video, sock, &users);
    Units units = Units_New(grid, video.cpu_count, CONFIG_UNITS_MAX, overview.share.color, args.civ);
    Units floats = Units_New(grid, video.cpu_count, CONFIG_UNITS_FLOAT_BUFFER, overview.share.color, args.civ);
    units = Units_GenerateTestZone(units, map, grid, data.graphics, users);
    overview.pan = Units_GetFirstTownCenterPan(units, grid, overview.share.color);
    Packets packets = Packets_Init();
    int32_t cycles = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        const Field field = Units_Field(units, map);
        const int32_t size = Packets_Size(packets);
        const uint64_t parity = Units_Xor(units);
        const int32_t ping = Ping_Get();
        overview = Overview_Update(overview, input, parity, cycles, size, units.share, ping);
        Sock_Send(sock, overview);
        const Packet packet = Packet_Get(sock);
        if(Packet_IsStable(packet))
            packets = Packets_Queue(packets, packet);
        packets = Packets_ClearWaste(packets, cycles);
        if(Packets_Active(packets))
            while(Packets_MustExecute(packets, cycles))
            {
                Packet dequeued;
                packets = Packets_Dequeue(packets, &dequeued);
                units = Units_PacketService(units, data.graphics, dequeued, grid, map, field);
            }
        units = Units_Caretake(units, data.graphics, grid, map, field);
        cycles++;
        if(packet.control == PACKET_CONTROL_SPEED_UP)
            continue;
        floats = Units_Float(floats, units, data.graphics, overview, grid, map, units.share.motive);
        Video_Draw(video, data, map, units, floats, overview, grid);
        const int32_t t1 = SDL_GetTicks();
        const int32_t dt = t1 - t0;
        Video_Render(video, units, dt, cycles);
        Field_Free(field);
        const int32_t t2 = SDL_GetTicks();
        const int32_t ms = CONFIG_MAIN_LOOP_SPEED_MS - (t2 - t0);
        if(ms > 0)
            SDL_Delay(ms);
    }
    Units_Free(floats);
    Units_Free(units);
    Packets_Free(packets);
    Sock_Disconnect(sock);
}

static void RunClient(const Args args)
{
    Ping_Init();
    SDL_CreateThread(Ping_Ping, "N/A", (void*) &args);
    SDL_Init(SDL_INIT_VIDEO);
    const Video video = Video_Setup(args.xres, args.yres, CONFIG_MAIN_GAME_NAME);
    Video_PrintLobby(video, 0, 0, COLOR_GAIA, 0);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(40, data.terrain);
    const Grid grid = Grid_Make(map.size, map.tile_width, map.tile_height);
    args.demo
        ? Video_RenderDataDemo(video, data, args.color)
        : Play(video, data, map, grid, args);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
    SDL_Quit();
    Ping_Shutdown();
}

static void RunServer(const Args args)
{
    Sockets sockets = Sockets_Init(args.port, args.users);
    Sockets pingers = Sockets_Init(args.port_ping, args.users);
    for(int32_t cycles = 0; true; cycles++)
    {
        sockets = Sockets_Accept(sockets);
        sockets = Sockets_Service(sockets, CONFIG_SOCKETS_SERVER_TIMEOUT_MS);
        sockets = Sockets_Relay(sockets, cycles, CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES, args.quiet);
        pingers = Sockets_Accept(pingers);
        Sockets_Ping(pingers, CONFIG_SOCKETS_SERVER_TIMEOUT_MS);
    }
    Sockets_Free(pingers);
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
