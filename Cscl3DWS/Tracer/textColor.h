#include <vector>

struct STextColor
{
	int r;
	int g;
	int b;
	char* start_color_pos;
	char* end_color_pos;
};

std::vector<STextColor> gcColors;