#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"

int main(const int argc, const char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    const Video video = Video_Setup(1300, 700, args.demo ? "Render Demo" : "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(video.xres, video.yres, grid);
    Units units = Units_New(8, map, grid, data.graphics);
    int32_t cycles = 0;
    if(args.demo)
        Video_RenderDataDemo(video, data, args.color);
    else for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        Map_Edit(map, overview, input);
        overview = Overview_Update(overview, input);
        const Field field = Units_Field(units, map);
        const Quad quad = Overview_GetRenderBox(overview, CONFIG_VIDEO_TOP_LEFT_BORDER_OFFSET);
        const Points render_points = Quad_GetRenderPoints(quad);
        units = Units_Caretake(units, data.graphics, overview, grid, input, map, field, render_points);
        Video_Render(video, data, map, units, overview, input, render_points);
        const int32_t t1 = SDL_GetTicks();
        Video_CopyCanvas(video);
        Log_Dump();
        Video_PrintPerformanceMonitor(video, units, t1 - t0, cycles);
        Video_Present(video);
        Field_Free(field);
        Points_Free(render_points);
        cycles++;
        if(args.measure)
            if(cycles > 10) // Measure performance with valgrind --tool=cachegrind ./openempires.
                break;
        const int32_t t2 = SDL_GetTicks();
        const int32_t ms = 15 - (t2 - t0);
        if(ms > 0)
            SDL_Delay(ms);
    }
    Units_Free(units);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
}
