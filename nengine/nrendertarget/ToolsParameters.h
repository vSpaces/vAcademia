
#pragma once

#define PEN_COLOR				1
#define PEN_WIDTH				2
#define PEN_OPACITY				3
#define CIRCLE_COLOR			4
#define CIRCLE_BACK_COLOR		5
#define CIRCLE_STROKE_WIDTH		6
#define CIRCLE_OPACITY			7
#define CIRCLE_MODE				8
#define RECTANGLE_COLOR			9
#define RECTANGLE_BACK_COLOR	10
#define RECTANGLE_STROKE_WIDTH	11
#define RECTANGLE_OPACITY		12
#define RECTANGLE_MODE			13
#define LINE_COLOR				14
#define LINE_WIDTH				15
#define LINE_OPACITY			16
#define LINE_ARROW1				17
#define LINE_ARROW2				18
#define LINE_ARROW_LENGTH		19
#define ERASER_WIDTH			20
#define ERASER_OPACITY			21
#define TEXT_FONT_NAME			22
#define TEXT_COLOR				23
#define TEXT_SIZE				24
#define TEXT_BOLD				25
#define TEXT_ITALIC				26
#define TEXT_UNDERLINE			27
#define TEXT_ANTIALIASING		28
#define TEXT_OPACITY			29
#define CLEAR_COLOR				30
#define SELECTION_COLOR			31
#define TEXT_NEW_CURSOR_POSITION	32

int GetToolParameterSize(int paramID);

#define TOOL_OPTIMIZATION		1