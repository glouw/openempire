#include "Video.h"
#include "Input.h"
#include "Units.h"
#include "Args.h"

static void PrintPerformanceMonitor(const Video video, const Units units, const int32_t dtb, const int32_t dtd, const int32_t cycles)
{
    static int dtb_hold;
    static int dtd_hold;

    if(cycles % 10 == 0)
    {
        dtb_hold = dtb;
        dtd_hold = dtd;
    }

    Text_Printf(video.text_small, video.renderer, video.top_left, POSITION_TOP_LEFT, 0xFF, 0,
            "units.count   : %4d\n"
            "dt (ms) unit  : %2d\n"
            "dt (ms) video : %2d\n"
            "cycles        : %4d", units.count, dtb_hold, dtd_hold, cycles);
}

int32_t main(int32_t argc, char* argv[])
{
    const Args args = Args_Parse(argc, argv);
    const Video video = Video_Setup(1300, 700, args.demo ? "Render Demo" : "Open Empires");
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
        Map_Edit(map, overview, input);
        overview = Overview_Update(overview, input);
        const int32_t ta = SDL_GetTicks();
        units = Units_Caretake(units, data.graphics, overview, grid, input, map, field);
        const int32_t tb = SDL_GetTicks();
        const int32_t dtd = Video_Render(video, data, map, units, overview, input);
        Field_Free(field);
        const int32_t dtb = tb - ta;
        PrintPerformanceMonitor(video, units, dtb, dtd, cycles);
        Video_Present(video);
        const int32_t t1 = SDL_GetTicks();
        const int32_t ms = 1000 / 60 - (t1 - t0);
        if(ms > 0)
            SDL_Delay(ms);
        cycles++;
        if(args.measure)
            if(cycles > 10) // NOTE: Measure performance with valgrind --tool=cachegrind.
                break;
    }
    Units_Free(units);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
}
