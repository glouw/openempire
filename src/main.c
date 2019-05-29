#include "Video.h"
#include "Input.h"
#include "Units.h"
#include "Args.h"

int32_t main(int32_t argc, char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    const Video video = Video_Setup(1024, 600, args.demo ? "Render Demo" : "Open Empires");
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(30, 30, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(video.xres, video.yres, grid);
    Units units = Units_New(8, map.rows, map.cols);

    if(args.demo)
        Video_RenderDataDemo(video, data, args.color);
    else for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        overview = Overview_Update(overview, input);
        Units_Caretake(units, data.graphics, overview, grid, input, map);
        Video_Draw(video, data, map, units, overview, input);
        Video_PresentCanvas(video);
        const int32_t t1 = SDL_GetTicks();
        const int32_t ms = 1000 / 60 - (t1 - t0);
        SDL_Delay(ms < 0 ? 0 : ms);
    }
    Units_Free(units);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
}
