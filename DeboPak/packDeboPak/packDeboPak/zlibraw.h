#pragma once
#include "stdafx.h"

int uncompr_raw(Bytef *dest, uLongf destLen, 
						  const Bytef *source, uLong sourceLen);
int compr_raw(Bytef *dest, uLongf* destLen, 
						  const Bytef *source, uLong sourceLen);
