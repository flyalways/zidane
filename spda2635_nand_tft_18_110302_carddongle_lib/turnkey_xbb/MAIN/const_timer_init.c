#include "SPDA2K.h"

//! To initialize the constant timer.
/*!	After initialization, the constant timer is activated with interrupt disabled.
 *	The counter value is reset to 0.
 *	Do this only at boot-up to avoid timing incorrectness.
 */
void init_const_timer(void)
{
	GLOBAL_REG[0x80] = 0; // disable constant timer, interrupt also disabled.

	GLOBAL_REG[0x81] = 1; // clear const timer counter
	while ((GLOBAL_REG[0x81] & 0x10) == 0);

	GLOBAL_REG[0x80] |= 1; // enable constant timer
	while ((GLOBAL_REG[0x80] & 0x10) == 0); // wait enable ready.
}
