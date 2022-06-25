#include "api.h"
#include "fixed_point.h"

// Use dodo_main instead of main so that we don't need extra code to return a value.
// void main is illegal in c++
extern "C" void dodo_main() {
	LED_ON();
	DELAY_MS(Real(255).asInt());
	LED_OFF();
}
