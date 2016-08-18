#ifndef INCL_PIXELDEFS
#define INCL_PIXELDEFS

#define PIXEL_BGRA_ORDER  // MS Windows ordering
#undef PIXEL_RGBA_ORDER // GNU/Linux (at least)

// We have this pixel byte ordering
//#define @PIXEL_BYTE_ORDER@

#ifdef PIXEL_RGBA_ORDER
 // To access in RGBA order
 #define RGBA_RED    0
 #define RGBA_GREEN  1
 #define RGBA_BLUE   2
 #define RGBA_ALPHA  3
#elif defined(PIXEL_BGRA_ORDER)
 #define RGBA_BLUE   0
 #define RGBA_GREEN  1
 #define RGBA_RED    2
 #define RGBA_ALPHA  3
#else
  #error Pixel RGB bytes order not selected
#endif

// Note that the preceeding #defines aren't static const ints because
// this breaks preprocessor tests in several places.

#endif

