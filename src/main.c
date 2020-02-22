#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Packets.h"
#include "Restore.h"
#include "Ping.h"
#include "Sockets.h"
#include "Overview.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

#include <time.h>

static Overview WaitInLobby(const Video video, const Sock sock)
{
    int32_t loops = 0;
    Overview overview = Overview_Init(video.xres, video.yres);
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        SDLNet_TCP_Send(sock.server, &overview, sizeof(overview));
        const Packet packet = Packet_Get(sock);
        if(packet.turn > 0)
        {
            overview.share.color = (Color) packet.client_id;
            Video_PrintLobby(video, packet.users_connected, packet.users, overview.share.color, loops++);
            if(packet.game_running)
            {
                overview.users = packet.users;
                overview.map_power = packet.map_power;
                overview.seed = packet.seed;
                break;
            }
        }
        SDL_Delay(CONFIG_MAIN_LOOP_SPEED_MS);
    }
    return overview;
}

static void Play(const Video video, const Data data, const Args args)
{
    Ping_Init(args);
    const Sock sock = Sock_Connect(args.host, args.port, "MAIN");
    const Sock reset = Sock_Connect(args.host, args.port_reset, "RESET");
    Overview overview = WaitInLobby(video, sock);
    Util_Srand(overview.seed);
    const Map map = Map_Make(overview.map_power, data.terrain);
    const Grid grid = Grid_Make(map.size, map.tile_width, map.tile_height);
    Units units = Units_New(grid.size, video.cpu_count, CONFIG_UNITS_MAX, overview.share.color, args.civ);
    Units floats = Units_New(grid.size, video.cpu_count, CONFIG_UNITS_FLOAT_BUFFER, overview.share.color, args.civ);
    units = Units_Generate(units, map, grid, data.graphics, overview.users);
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
        SDLNet_TCP_Send(sock.server, &overview, sizeof(overview));
        const Packet packet = Packet_Get(sock);
        if(packet.is_out_of_sync)
        {
            puts("OUT OF SYNC. RESTORING");
            if(packet.client_id == COLOR_BLU)
            {
                const Restore restore = Units_PackRestore(units, cycles); // XXX. THIS CANNOT BE SENT BY PLAYER 0. SERVER MUST TRACK STATE OF ALL PLAYERS AND SEND.
                Restore_Send(restore, reset.server);
            }
            const Restore restore = Restore_Recv(reset.server);
            units = Units_Restore(units, restore, grid);
            cycles = restore.cycles;
            Restore_Free(restore);
            Util_Srand(overview.seed);
            packets = Packets_Clear(packets);
            Packet_Flush(sock);
        }
        else
        {
            if(Packet_IsStable(packet))
                packets = Packets_Queue(packets, packet);
            packets = Packets_ClearStale(packets, cycles);
            while(Packets_MustExecute(packets, cycles))
            {
                Packet dequeued;
                packets = Packets_Dequeue(packets, &dequeued);
                units = Units_PacketService(units, data.graphics, dequeued, grid, map, field);
            }
            units = Units_Caretake(units, data.graphics, grid, map, field);
            cycles++;
            if(packet.control != PACKET_CONTROL_SPEED_UP)
            {
                floats = Units_Float(floats, units, data.graphics, overview, grid, map, units.share.motive);
                Video_Draw(video, data, map, units, floats, overview, grid);
                const int32_t t1 = SDL_GetTicks();
                Video_Render(video, units, overview, map, t1 - t0, cycles, ping);
                Field_Free(field);
                const int32_t t2 = SDL_GetTicks();
                const int32_t ms = CONFIG_MAIN_LOOP_SPEED_MS - (t2 - t0);
                if(ms > 0)
                    SDL_Delay(ms);
            }
        }
    }
    Units_Free(floats);
    Units_Free(units);
    Packets_Free(packets);
    Sock_Disconnect(sock);
    Sock_Disconnect(reset);
    Ping_Shutdown();
    Map_Free(map);
}

static void RunClient(const Args args)
{
    SDL_Init(SDL_INIT_VIDEO);
    const Video video = Video_Setup(args.xres, args.yres, CONFIG_MAIN_GAME_NAME);
    Video_PrintLobby(video, 0, 0, COLOR_GAIA, 0);
    const Data data = Data_Load(args.path);
    args.demo
        ? Video_RenderDataDemo(video, data, args.color)
        : Play(video, data, args);
    Video_Free(video);
    Data_Free(data);
    SDL_Quit();
}

static void RunServer(const Args args)
{
    srand(time(NULL));
    Sockets sockets = Sockets_Init(args.port);
    Sockets pings = Sockets_Init(args.port_ping);
    Sockets resets = Sockets_Init(args.port_reset);
    Cache cache = Cache_Init(args.users, args.map_power);
    for(int32_t cycles = 0; true; cycles++)
    {
        if(cycles > 1500 && cycles % 750 == 0)
            cache.is_out_of_sync = true;
        const int32_t t0 = SDL_GetTicks();
        sockets = Sockets_Accept(sockets);
        pings = Sockets_Accept(pings);
        resets = Sockets_Accept(resets);
        sockets = Sockets_Recv(sockets, &cache);
        Sockets_Send(sockets, &cache, cycles, args.quiet);
        Sockets_Ping(pings);
        Sockets_Reset(resets, &cache);
        const int32_t t1 = SDL_GetTicks();
        const int32_t ms = 10 - (t1 - t0);
        if(ms > 0)
            SDL_Delay(ms);
    }
    Sockets_Free(resets);
    Sockets_Free(pings);
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
