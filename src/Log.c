#include "Log.h"

#include "Util.h"

#include <stdarg.h>

#define MAX (32)

#define CAP(index) (index % MAX)

static Log logger;

void Log_Init(const Video video)
{
    logger.video = video;
    char** message = UTIL_ALLOC(char*, MAX);
    UTIL_CHECK(message);
    logger.message = message;
}

void Log_Append(char* const format, ...)
{
    va_list args;
    va_start(args, format);
    const int32_t len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    va_start(args, format);
    char* const message = UTIL_ALLOC(char, len + 1);
    UTIL_CHECK(message);
    vsprintf(message, format, args);
    va_end(args);
    logger.message[logger.b] = message;
    logger.b++;
    logger.b %= MAX;
    if(logger.b == logger.a)
    {
        free(logger.message[logger.a]);
        logger.a++;
        logger.a %= MAX;
    }
}

void Log_Dump(void)
{
    for(int32_t i = logger.a, line = 0; CAP(i) != logger.b ; i++)
        Text_Puts(
                logger.video.text_small,
                logger.video.renderer,
                logger.video.top_left,
                POSITION_TOP_LEFT,
                0xFF,
                line++,
                logger.message[CAP(i)]);
}
