#pragma once

namespace rmml{

// коды изменений бинарного состояния
enum BSSCCode{
	BSSCC_REPLACE_ALL = 1,	// заменить полностью все состояние
	BSSCC_APPEND,			// дополнить состояние 
	BSSCC_REPLACE,			// заменить часть бинарного состояния
	BSSCC_CLEAR				// полностью очистить состояние
};

}
