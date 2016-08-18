#pragma once

// начать запись звука с микрофона
int Start_Recording();
// остановить запись звука с микрофона
int Stop_Recording( LPWSTR *fileName, unsigned int aSize);
// возвращает уровень громкости записываемого звука с микрофона
float Get_CurrentMicInputLevelNormalized();
