#include "Video.h"
#include "Input.h"
#include "Config.h"
#include "Log.h"
#include "Units.h"
#include "Args.h"

int main(const int argc, const char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    const Video video = Video_Setup(1024, 600, args.demo ? "Render Demo" : "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(video.xres, video.yres, grid);
    Units units = Units_New(8, map, grid, data.graphics);
    int32_t cycles = 0;
    if(args.demo)
        Video_RenderDataDemo(video, data, args.color);
    else
        for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
        {
            const int32_t t0 = SDL_GetTicks();
            const Field field = Units_Field(units, map);
            const Quad quad = Overview_GetRenderBox(overview, CONFIG_VIDEO_TOP_LEFT_BORDER_OFFSET);
            const Points render_points = Quad_GetRenderPoints(quad);
            Map_Edit(map, overview, input);
            overview = Overview_Update(overview, input);
            units = Units_Caretake(units, data.graphics, overview, grid, input, map, field, render_points);
            const int32_t dt = Video_Render(video, data, map, units, overview, input, render_points);
            Log_Dump();
            Field_Free(field);
            Points_Free(render_points);
            Video_Present(video);
            const int32_t t1 = SDL_GetTicks();
            const int32_t ms = 1000 / 60 - (t1 - t0);
            if(ms > 0)
                SDL_Delay(ms);
            cycles++;
            if(args.measure)
                if(cycles > 10) // Measure performance with valgrind --tool=cachegrind ./openempires.
                    break;
        }
    Units_Free(units);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
}
