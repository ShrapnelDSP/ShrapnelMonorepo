#pragma once

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Update the parameters of the filter. 
 *
 *  \todo warp the parameter, to make the response more linear
 */
void fmv_update_params(float l, float m, float t);

/** Process a buffer. The buffer is processed in-place.  */
void fmv_process(float *buf, size_t buf_len);

/** Initialise the fmv processor. The bass middle and treble settings are all
 * initialised to 0.5.
 */
esp_err_t fmv_init(void);

#ifdef __cplusplus
}
#endif
