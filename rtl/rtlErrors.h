#pragma once

#define RtlError unsigned int

#define	RTLERROR_NOERROR				0
#define	RTLERROR_CONVERTERROR			1			/*ошибка при конвертации*/

#define	RTLERROR_UNRECOGNIZEDERROR		0xFFFF		/*какая-то ошибка*/

#define RTLERROR_FILE_ACCESS_DENIDED     2           /*ошибка доступа к файлу при удалении*/

#define RTLERROR_FOLDER_ACCESS_DENIDED   3			 /*ошибка доступа к папке при удалении*/

#define RTLERROR_NULL_PATH				 4			/*пустой путь к файлу*/