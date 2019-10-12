#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"
#include "Util.h"

int main(const int argc, const char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    const Video video = Video_Setup(1200, 700, args.demo ? "Render Demo" : "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Units units = Units_New(map, grid, data.graphics);
    Overview overview = Overview_Init(video.xres, video.yres, grid, COLOR_RED); // TO BE SENT VIA P2P.
    int32_t cycles = 0;
    if(!args.demo)
        for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
        {
            const int32_t t0 = SDL_GetTicks();
            overview = Overview_Update(overview, input); // TO BE SENT VIA P2P.
            Map_Edit(map, overview, input);
            const Field field = Units_Field(units, map);
            const Window window = Window_Make(overview);
            units = Units_Caretake(units, data.graphics, overview, input, map, field, window);
            Video_Render(video, data, map, units, overview, input, window);
            const int32_t t1 = SDL_GetTicks();
            Video_CopyCanvas(video);
            Log_Dump();
            Video_PrintPerformanceMonitor(video, units, t1 - t0, cycles);
            Video_Present(video);
            Field_Free(field);
            Window_Free(window);
            cycles++;
            if(args.measure)
                if(cycles > 10) // Measure performance with valgrind --tool=cachegrind ./openempires.
                    break;
            const int32_t t2 = SDL_GetTicks();
            const int32_t ms = 15 - (t2 - t0);
            if(ms > 0)
                SDL_Delay(ms);
        }
    else
        Video_RenderDataDemo(video, data, args.color);
    Units_Free(units);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
}
