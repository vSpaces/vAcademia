#pragma once
#ifdef RM_EXPORTS
#define csclRM_API __declspec(dllexport)
#else
#define csclRM_API __declspec(dllimport)
#endif