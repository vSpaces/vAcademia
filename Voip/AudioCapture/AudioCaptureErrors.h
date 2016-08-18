#pragma once

typedef unsigned int VOIPAC_ERROR;

#define	ACERROR_NOERROR						0	/*все ок*/
#define	ACERROR_NOAUDIOCABLEINSTALLED		1	/*не найден инсталенный VAC*/
#define	ACERROR_PRIMARYCHANGEFAILED			2	/*не удалось сменить текущее устройство*/
#define	ACERROR_REPEATERSTARTFAILED			3	/*не удалось запустить AudioRepeater*/
#define	ACERROR_DEFAULTSRESTOREFAILED		4	/*не удалось восстановить устройства по умолчанию*/
#define	ACERROR_INDEVICENOTFOUND			5	/*не удалось найти указанное устройство входа*/
#define	ACERROR_OUTDEVICENOTFOUND			6	/*не удалось найти указанное устройство выхода*/
#define	ACERROR_INCORRECTAUDIOPARAMES		7	/*неверные параметры звука*/
#define	ACERROR_DIRECTSOUNDERROR			8	/*ошибка DirectSound*/
#define	ACERROR_NOTINITIALIZED				9	/*не было инициализации*/
#define	ACERROR_INDEVICEPARAMSFAILED		10	/*параметры не подходят для устройства воспроизведения*/
#define	ACERROR_OUTDEVICEPARAMSFAILED		11	/*параметры не подходят для устройства записи*/
#define	ACERROR_ISBISY						12	/*устройство не может сейчас изменить параметры звука*/
#define	ACERROR_DATATRUNCATED				13	/*буфер меньше чем данные, будут записаны не все*/
#define	ACERROR_BUFFERLOST					14	/*буфер потерян*/
#define	ACERROR_INDEVICEFAILED				15	/*ошибка чтения звуковых данных*/
#define	ACERROR_OUTDEVICEFAILED				16	/*ошибка записи звуковых данных*/
#define	ACERROR_VACNOTINSTALLED				17	/*не найден драйвер VAC*/
#define	ACERROR_DEVICECHANGEFAILED			18	/*ошибка смены устройства*/
#define	ACERROR_DEFAULTALLREADY				19	/*устройство уже по дефолту*/
#define	ACERROR_BUFFERTOOSMALL				20	/*не хватает буфера*/
#define	ACERROR_ACCESSDENIED				21	/*не хватает прав*/
#define	ACERROR_COMFAILED					22	/*ошибка создания COM компонента*/

#define	ACERROR_NOTIMPLEMENTED			0x8FFFFFFF	/*метод еще не реализован*/
#define	ACERROR_UNKNOWNERROR			0xFFFFFFFF	/*неизвестная ошибка*/