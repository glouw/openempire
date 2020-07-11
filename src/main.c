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
    int32_t cycles = 0;
    Overview overview = Overview_Make(video.xres, video.yres);
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        UTIL_TCP_SEND(sock.server, &overview);
        const Packet packet = Packet_Get(sock);
        if(Packet_IsAlive(packet))
        {
            // SERVER ASSIGNS COLOR - SORRY, THERE IS NO ELEGANT WAY TO CHOOSE A COLOR.
            overview.color = (Color) packet.client_id;
            const char* const message = Color_ToString(overview.color);
            Video_PrintLobby(video, packet.users_connected, packet.users, cycles++, message);
            if(packet.game_running)
            {
                overview.users = packet.users;
                overview.seed = packet.seed;
                // THE LAST PLAYER TO CONNECT IS THE SPECTATOR.
                // THE SPECTATOR DICTATES THE TRUE GAME SPEED AND STATE AND DOES NOT RENDER A GRAPHICAL OUTPUT.
                // THE SPECTATOR MUST RUN ON THE FASTEST MOST TRUSTED MACHINE.
                overview.spectator = (Color) (overview.users - 1);
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
    const Sock sock = Sock_Connect(args.host, args.port);
    const Sock reset = Sock_Connect(args.host, args.port_reset);
    Overview overview = WaitInLobby(video, sock);
    const bool is_spectator = Overview_IsSpectator(overview);
    Util_Srand(overview.seed);
    const Map map = Map_Make(data.terrain);
    const Grid grid = Grid_Make(map.size, map.tile_width, map.tile_height);
    Units units  = Units_Make(grid.size, video.cpu_count, CONFIG_UNITS_MAX, overview.color);
    Units floats = Units_Make(grid.size, video.cpu_count, CONFIG_UNITS_FLOAT_BUFFER, overview.color);
    units = Units_Generate(units, map, grid, data.graphics, overview.users, overview.spectator);
    overview.pan = Units_GetFirstTownCenterPan(units, grid);
    Packets packets = Packets_Make();
    Field field = Field_Make(map.size);
    int32_t cycles = 0;
    int32_t control = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        Field_Clear(field);
        Units_Field(units, map, field);
        const int32_t size = Packets_Size(packets);
        const uint64_t parity = Units_Xor(units);
        const int32_t ping = Ping_Get();
        overview = Overview_Update(overview, input, parity, cycles, size, units.share[units.color], ping, args.must_randomize_mouse);
        UTIL_TCP_SEND(sock.server, &overview);
        const Packet packet = Packet_Get(sock);
        if(packet.is_out_of_sync)
        {
            if(is_spectator)
            {
                Units_FreeAllPathsForRecovery(units);
                const Restore restore = Units_PackRestore(units, cycles);
                Restore_Send(restore, reset.server);
            }
            if(args.must_simulate_slow_download)
            {
                const int32_t delay = 3000 + rand() % 2000;
                printf("%d .. %d\n", overview.color, delay);
                SDL_Delay(delay);
            }
            const Restore restore = Restore_Recv(reset.server);
            Sock_GetServerRestoredAck(reset);
            units = Units_ApplyRestore(units, restore, grid, field);
            cycles = restore.cycles;
            Util_Srand(overview.seed);
            Restore_Free(restore);
            packets = Packets_Clear(packets);
            Packet_Flush(sock);
            // THIS DELAY LETS THE SERVER GET AHEAD OF ALL CLIENTS BY A SINGLE CYCLE.
            SDL_Delay(CONFIG_SOCKETS_SERVER_MS_PER_SEND);
        }
        else
        {
            if(Packet_IsReady(packet))
                packets = Packets_Queue(packets, packet);
            packets = Packets_ClearStale(packets, cycles);
            while(Packets_MustExecute(packets, cycles))
            {
                Packet dequeued;
                packets = Packets_Dequeue(packets, &dequeued);
                units = Units_PacketService(units, data.graphics, dequeued, grid, map, field);
            }
            units = Units_Caretake(units, data.graphics, grid, map, field, args.must_randomize_mouse);
            cycles++;
            if(args.must_measure && cycles == 1024)
                break;
            // SERVER SPEEDS UP CLIENTS WITH POSITIVE CONTROL VALUES.
            if(packet.control != 0)
                control = packet.control;
            // POSITIVE CONTROL VALUE: CLIENT SPEEDS UP BY SKIPPING RENDERER.
            if(control > 0)
            {
                control--;
                continue;
            }
            if(is_spectator)
            {
                if(Packet_IsReady(packet))
                    Video_PrintLobby(video, packet.users_connected, packet.users, cycles, "Spectating");
            }
            else
            {
                floats = Units_Float(floats, units, data.graphics, overview, grid, map, units.share[units.color].motive);
                Video_Draw(video, data, map, units, floats, overview, grid);
                const int32_t t1 = SDL_GetTicks();
                const int32_t dt = t1 - t0;
                Video_Render(video, units, overview, map, dt, cycles, ping);
            }
            const int32_t t2 = SDL_GetTicks();
            const int32_t dt = t2 - t0;
            const int32_t ms = CONFIG_MAIN_LOOP_SPEED_MS - dt;
            if(ms > 0)
                SDL_Delay(ms);
            // NEGATIVE CONTRL VALUE: (SPECTATOR CLIENT ONLY) SLOWS DOWN WITH ARBITRARY DELAY.
            if(control < 0)
            {
                SDL_Delay(abs(control));
                control = 0;
            }
        }
    }
    Field_Free(field);
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
    if(args.must_simulate_slow_download)
        srand(time(NULL));
    Sock_Init();
    SDL_Init(SDL_INIT_VIDEO);
    const Video video = Video_Make(args.xres, args.yres, CONFIG_MAIN_GAME_NAME);
    Video_PrintLobby(video, 0, 0, 0, "Loading");
    const Data data = Data_Load(args.path);
    args.is_demo
        ? Video_RenderDataDemo(video, data, args.color)
        : Play(video, data, args);
    Video_Free(video);
    Data_Free(data);
    SDL_Quit();
}

static int32_t RunServerPings(void* const data)
{
    Args* const args = (Args*) data;
    Sockets pings = Sockets_Make(args->port_ping);
    while(true)
    {
        pings = Sockets_Accept(pings);
        Sockets_Ping(pings);
        SDL_Delay(1);
    }
    Sockets_Free(pings);
    return 0;
}

static void RunServer(const Args args)
{
    srand(args.must_measure ? 0 : time(NULL));
    Sockets_Init();
    SDL_CreateThread(RunServerPings, "N/A", (void*) &args);
    Sockets sockets = Sockets_Make(args.port);
    Sockets resets = Sockets_Make(args.port_reset);
    Cache cache = Cache_Make(args.users);
    for(int32_t cycles = 0; true; cycles++)
    {
        const int32_t t0 = SDL_GetTicks();
        sockets = Sockets_Accept(sockets);
        resets = Sockets_Accept(resets);
        sockets = Sockets_Recv(sockets, &cache);
        const int32_t max_ping = Cache_GetPingMax(&cache);
        const int32_t min_ping = Cache_GetPingMin(&cache);
        Sockets_Send(sockets, &cache, cycles, args.is_quiet, max_ping);
        Sockets_Reset(resets, &cache, max_ping, min_ping);
        const int32_t t1 = SDL_GetTicks();
        const int32_t ms = CONFIG_SOCKETS_SERVER_CYCLE_LENGTH_MS - (t1 - t0);
        if(ms > 0)
            SDL_Delay(ms);
    }
    Sockets_Free(resets);
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
