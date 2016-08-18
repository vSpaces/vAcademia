
#pragma once

class Data;

unsigned CreateTexture3D(bool mipmap, const wchar_t* fileName);
unsigned CreateTexture3D(bool mipmap, Data* data);

