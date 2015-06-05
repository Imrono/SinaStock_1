#include "analyzePosition.h"

char *TradingPoint::stockTradeStr[] = {	"BUY_UP",
									"SELL_DOWN",
									// 做空
									"SELL_SHORT",
									"BUY_COVER",
									// 止损
									"STOP_LOSS_SELL_DOWN",
									"STOP_LOSS_BUY_COVER"};