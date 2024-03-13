// audio parameter basic test
// factory reset
// all parameters loaded correctly
// update a parameter
// force save to NVS
// reboot
// check parameter reloaded

// preset test
// start up
// create preset
// change a parameter
// create another preset
// reboot
// last saved preset is reloaded
// load first preset
// parameter is restored to value stored in preset

// preset reload test
// start up
// create preset
// change a parameter
// load preset
// parameter is restored to value stored in preset

// MIDI mapping test
// for each mapping type
// start up
// create MIDI mapping
// trigger midi mapping
// verify correct reaction

// MIDI mapping preset
// Initial conditions:
//   preset exists
//   current parameters are different to preset
// start up
// create MIDI mapping
// trigger midi mapping
// verify correct reaction

// stress test
// factory reset
// do lots of stuff
// dump task list, heap stats

// Audio
// bypass all audio processing
// check level, gain, distortion etc
