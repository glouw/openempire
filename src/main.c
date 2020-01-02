#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Packets.h"
#include "Sockets.h"
#include "Overview.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

#define DEMO (0)

static void RunClient(const Args args)
{
    SDL_Init(SDL_INIT_VIDEO);
    const Video video = Video_Setup(args.xres, args.yres, CONFIG_MAIN_GAME_NAME);
    Video_PrintLobby(video, 0, 0, (Color) -1);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(40, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    if(DEMO)
        Video_RenderDataDemo(video, data, args.color);
    else
    {
        // -- LOBBY.
        int32_t users = 0;
        Overview overview = Overview_Init(video.xres, video.yres);
        const Sock sock = Sock_Connect(args.host, args.port);
        for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
        {
            Sock_Send(sock, overview);
            const Packet packet = Packet_Get(sock);
            if(packet.turn > 0)
            {
                overview.color = (Color) packet.client_id;
                Video_PrintLobby(video, packet.users_connected, packet.users, overview.color);
                if(packet.game_running)
                {
                    users = packet.users;
                    break;
                }
            }
            SDL_Delay(CONFIG_MAIN_LOOP_SPEED_MS);
        }
        // -- GAME.
        Units units = Units_New(grid, video.cpu_count, CONFIG_UNITS_MAX);
        Units floats = Units_New(grid, video.cpu_count, CONFIG_UNITS_FLOAT_BUFFER);
        Packets packets = Packets_Init();
        units = Units_GenerateTestZone(units, map, grid, data.graphics, users);
        overview.pan = Units_GetFirstTownCenterPan(units, grid, overview.color);
        int32_t cycles = 0;
        for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
        {
            const int32_t t0 = SDL_GetTicks();
            const uint64_t parity = Units_Xor(units);
            Map_Edit(map, overview, grid); // XXX. FOR FUN. REMOVE IN FUTURE.
            overview = Overview_Update(overview, input, parity, cycles, Packets_Size(packets));
            Sock_Send(sock, overview);
            const Packet packet = Packet_Get(sock);
            if(Packet_IsStable(packet))
                packets = Packets_Queue(packets, packet);
            const Field field = Units_Field(units, map);
            if(Packets_Active(packets))
            {
                const Packet peek = Packets_Peek(packets);
                if(cycles > peek.exec_cycle)
                    Util_Bomb("CLIENT - CLIENT_ID %d :: OUT OF SYNC\n", peek.client_id);
                while(cycles == Packets_Peek(packets).exec_cycle) // FLUSH.
                {
                    Packet dequeued;
                    packets = Packets_Dequeue(packets, &dequeued);
                    units = Units_PacketService(units, data.graphics, dequeued, grid, map, field);
                }
            }
            units = Units_Caretake(units, data.graphics, grid, map, field);
            cycles++;
            if(packet.control == PACKET_CONTROL_SPEED_UP)
                continue;
            floats = Units_Float(floats, units, data.graphics, overview, grid, map, units.motive);
            const int32_t t1 = SDL_GetTicks();
            Video_Draw(video, data, map, units, floats, overview, grid);
            const int32_t t2 = SDL_GetTicks();
            const int32_t dt = t2 - t0;
            Video_Render(video, units, dt, cycles);
            Field_Free(field);
            const int32_t t3 = SDL_GetTicks();
            const int32_t ms = CONFIG_MAIN_LOOP_SPEED_MS - (t3 - t0);
            if(ms > 0)
                SDL_Delay(ms);
            if(packet.control == PACKET_CONTROL_SLOW_DOWN)
                SDL_Delay(t3 - t1);
        }
        Sock_Disconnect(sock);
        Packets_Free(packets);
        Units_Free(floats);
        Units_Free(units);
    }
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
    SDL_Quit();
}

static void RunServer(const Args args)
{
    Sockets sockets = Sockets_Init(args.port, args.users);
    for(int32_t cycles = 0; true; cycles++)
    {
        sockets = Sockets_Accept(sockets);
        sockets = Sockets_Service(sockets, CONFIG_SOCKETS_SERVER_TIMEOUT_MS);
        sockets = Sockets_Relay(sockets, cycles, CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES, args.quiet);
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
