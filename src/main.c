#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Packets.h"
#include "Cache.h"
#include "Backup.h"
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
        Sock_Send(sock, overview);
        const Packet packet = Packet_Get(sock);
        if(packet.turn > 0)
        {
            overview.share.color = (Color) packet.client_id;
            Video_PrintLobby(video, packet.users_connected, packet.users, overview.share.color, loops++);
            if(packet.is_game_running)
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
    const Sock panic = Sock_Connect(args.host, args.port_panic, "PANIC");
    Overview overview = WaitInLobby(video, sock);
    Util_Srand(overview.seed);
    const Map map = Map_Make(overview.map_power, data.terrain);
    const Grid grid = Grid_Make(map.size, map.tile_width, map.tile_height);
    Units units = Units_New(grid.size, video.cpu_count, CONFIG_UNITS_MAX, overview.share.color, args.civ);
    Units floats = Units_New(grid.size, video.cpu_count, CONFIG_UNITS_FLOAT_BUFFER, overview.share.color, args.civ);
    units = Units_Generate(units, map, grid, data.graphics, overview.users);
    overview.pan = Units_GetFirstTownCenterPan(units, grid, overview.share.color);
    Packets packets = Packets_Init();
    Backup backup = Backup_Init();
    int32_t cycles = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        const Field field = Units_Field(units, map);
        const int32_t size = Packets_Size(packets);
        const uint64_t xorred = Units_Xor(units);
        const int32_t ping = Ping_Get();
        overview = Overview_Update(overview, input, xorred, cycles, size, units.share, ping);
        Sock_Send(sock, overview);
        const Packet packet = Packet_Get(sock);
        if(Packet_IsStable(packet))
        {
            if(packet.is_out_of_sync)
            {
                puts("PANICKING...");
                puts("SENDING...");
                SDLNet_TCP_Send(panic.server, backup.parity, sizeof(backup.parity));
                puts("DONE SENDING...");
                int32_t index;
                puts("RECIEVING...");
                SDLNet_TCP_Recv(panic.server, &index, sizeof(index));
                printf("GOT WHAT I NEEDED... INDEX %d\n", index);
                if(index < 0 || index >= BACKUP_MAX)
                    Util_Bomb("NO GOOD INDEX\n");
                Units_Free(units);
                units = Units_Copy(backup.units[index]);
                packets = Packets_Flush(packets);
                backup = Backup_Free(backup);
                cycles = backup.parity[index].cycles;
                printf("XORRED 0x%016lX\n", Units_Xor(units));
                continue;
            }
            packets = Packets_Queue(packets, packet);
        }
        packets = Packets_ClearWaste(packets, cycles);
        while(Packets_MustExecute(packets, cycles))
        {
            Packet dequeued;
            packets = Packets_Dequeue(packets, &dequeued);
            backup = Backup_Push(backup, units, cycles);
            units = Units_PacketService(units, data.graphics, dequeued, grid, map, field);
        }
        units = Units_Caretake(units, data.graphics, grid, map, field);
        cycles++;
        if(packet.control == PACKET_CONTROL_SPEED_UP)
            continue;
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
    Units_Free(floats);
    Units_Free(units);
    Packets_Free(packets);
    Sock_Disconnect(panic);
    Sock_Disconnect(sock);
    Ping_Shutdown();
    Map_Free(map);
    Backup_Free(backup);
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
    Sockets pingers = Sockets_Init(args.port_ping);
    Sockets panicks = Sockets_Init(args.port_panic);
    Cache cache = Cache_Init(args.users, args.map_power);
    for(int32_t cycles = 0; true; cycles++)
    {
        sockets = Sockets_Accept(sockets);
        pingers = Sockets_Accept(pingers);
        panicks = Sockets_Accept(panicks);
        Sockets_Ping(pingers);
        Sockets_CountConnectedPlayers(sockets, &cache);
        sockets = Sockets_Recieve(sockets, &cache);
        Cache_CalcOutOfSync(&cache);
        Sockets_Send(sockets, cycles, &cache);
        Sockets_Panic(panicks, &cache);
        SDL_Delay(1);
    }
    Sockets_Free(pingers);
    Sockets_Free(sockets);
    Sockets_Free(panicks);
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
