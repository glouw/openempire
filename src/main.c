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
    const Color color = args.color;
    const Video video = Video_Setup(1280, 700, "Open Empires");
    Log_Init(video);
    const Data data = Data_Load(args.path);
    const Map map = Map_Make(60, data.terrain);
    const Grid grid = Grid_Make(map.cols, map.rows, map.tile_width, map.tile_height);
    Overview overview = Overview_Init(color, video.xres, video.yres, grid, video.cpu_count);
    Units units = Units_New(map, overview, data.graphics, video.cpu_count);
#if 1
    int32_t cycles = 0;
    for(Input input = Input_Ready(); !input.done; input = Input_Pump(input))
    {
        const int32_t t0 = SDL_GetTicks();
        overview = Overview_Update(overview, input); // XXX. TO BE SENT VIA P2P (ALONG WITH INPUT).
        Map_Edit(map, overview, input);
        const Field field = Units_Field(units, map);
        const Window window = Window_Make(overview);
        units = Units_Caretake(units, data.graphics, overview, input, map, field, window); // XXX. TO BE MODIFIED BY ALL P2P CLIENTS.
        Video_Render(video, data, map, units, overview, input, window);
        const int32_t t1 = SDL_GetTicks();
        Video_CopyCanvas(video);
        Log_Dump();
        Video_PrintPerformanceMonitor(video, units, t1 - t0, cycles);
        Video_PrintResources(video, units);
        Video_PrintHotkeys(video);
        Video_Present(video);
        Field_Free(field);
        Window_Free(window);
        cycles++;
        if(args.measure && cycles > 10)
            break;
        const int32_t t2 = SDL_GetTicks();
        const int32_t ms = 15 - (t2 - t0);
        if(ms > 0)
            SDL_Delay(ms);
    }
#else
    Video_RenderDataDemo(video, data, args.color);
#endif
    Units_Free(units);
    Map_Free(map);
    Data_Free(data);
    Video_Free(video);
}
