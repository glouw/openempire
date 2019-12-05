#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Packets.h"
#include "Sockets.h"
#include "Stream.h"
#include "Overview.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

#define DEMO (false)

static void RunClient(const Args args)
{
    SDL_Init(SDL_INIT_VIDEO);
    const Color color = args.color;
    const Video video = Video_Setup(args.xres, args.yres, "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(color, video.xres, video.yres);
    Units units = Units_New(grid, video.cpu_count, CONFIG_UNITS_MAX);
    units = Units_GenerateTestZone(units, map, grid, data.graphics);
    Units floats = Units_New(grid, video.cpu_count, CONFIG_UNITS_FLOAT_BUFFER);
    Stream stream = Stream_Init();
    Packets packets = Packets_Init();
    if(DEMO == true)
        Video_RenderDataDemo(video, data, args.color);
    else
    {
        const Sock sock = Sock_Connect(args.host, args.port);
        int32_t cycles = 0;
        for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
        {
            const int32_t t0 = SDL_GetTicks();
            const uint64_t parity = Units_Xor(units);
            Map_Edit(map, overview, grid); // XXX. FOR FUN. REMOVE IN FUTURE.
            overview = Overview_Update(overview, input, parity, cycles, Packets_Size(packets));
            Sock_Send(sock, overview);
            stream = Stream_Flow(stream, sock);
            if(stream.packet.turn > 0)
                packets = Packets_Queue(packets, stream.packet);
            const Field field = Units_Field(units, map);
            if(Packets_Active(packets))
            {
                if(cycles > Packets_Peek(packets).exec_cycle)
                    Util_Bomb("CLIENT_ID %d :: OUT OF SYNC - CLIENT MISSED PACKET EXECUTION\n");
                while(true)
                {
                    if(cycles != Packets_Peek(packets).exec_cycle)
                        break;
                    Packet dequeued;
                    packets = Packets_Dequeue(packets, &dequeued);
                    units = Units_PacketService(units, data.graphics, dequeued, grid, map, field);
                }
            }
            units = Units_Caretake(units, data.graphics, grid, map, field);
            cycles += 1;
            if(stream.packet.control == PACKET_CONTROL_SPEED_UP)
                continue;
            floats = Units_Float(floats, data.graphics, overview, grid, map, units.motive);
            const int32_t t1 = SDL_GetTicks();
            Video_Draw(video, data, map, units, floats, overview, grid);
            const int32_t t2 = SDL_GetTicks();
            const int32_t dt = t2 - t0;
            Video_Render(video, units, dt, cycles);
            Field_Free(field);
            const int32_t t3 = SDL_GetTicks();
            const int32_t ms = CONFIG_MAIN_LOOP_SPEED- (t3 - t0);
            if(ms > 0)
                SDL_Delay(ms);
            if(stream.packet.control == PACKET_CONTROL_SLOW_DOWN)
                SDL_Delay(t3 - t1);
        }
        Sock_Disconnect(sock);
    }
    Units_Free(units);
    Units_Free(floats);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
    Packets_Free(packets);
    SDL_Quit();
}

static void RunServer(const Args args)
{
    Sockets sockets = Sockets_Init(args.port);
    for(int32_t cycles = 0; true; cycles++)
    {
        sockets = Sockets_Accept(sockets);
        sockets = Sockets_Service(sockets, CONFIG_SOCKETS_SERVER_TIMEOUT);
        sockets = Sockets_Relay(sockets, cycles, CONFIG_SOCKETS_SERVER_UPDATE_SPEED);
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
