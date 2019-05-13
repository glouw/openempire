#include "Animation.h"

#include "Util.h"

Animation Animation_Get(const Slp slp, const Palette palette, const Color color)
{
    static Animation zero;
    Animation animation = zero;
    animation.count = slp.num_frames;
    animation.surface = UTIL_ALLOC(SDL_Surface*, slp.num_frames);
    animation.frame = UTIL_ALLOC(Frame, slp.num_frames);
    animation.image = UTIL_ALLOC(Image, slp.num_frames);
    UTIL_CHECK(animation.surface);
    UTIL_CHECK(animation.frame);
    UTIL_CHECK(animation.image);
    for(int32_t i = 0; i < animation.count; i++)
    {
        const Image image = Image_Copy(slp.image[i]);
        const Frame frame = slp.frame[i];
        animation.surface[i] = Image_Parse(image, frame, palette, color);
        animation.image[i] = image;
        animation.frame[i] = frame;
    }
    return animation;
}

void Animation_Free(const Animation animation)
{
    for(int32_t i = 0; i < animation.count; i++)
    {
        SDL_FreeSurface(animation.surface[i]);
        Image_Free(animation.image[i]);
    }
    free(animation.surface);
    free(animation.frame);
    free(animation.image);
}
