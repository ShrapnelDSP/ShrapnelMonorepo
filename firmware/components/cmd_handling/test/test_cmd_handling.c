/* cJSON source code is used, it is not mocked
 * esp_log is replaced with a fake that prints to stdout
 */

/* Queue returns fail, param_update_parameter is not called */
/* Queue returns an invalid message, param_update_parameter is not called */
/* Queue returns a valid message, param_update_parameter is called with correct parameters */
