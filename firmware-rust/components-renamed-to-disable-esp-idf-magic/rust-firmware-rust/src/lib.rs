#![no_std]

use core::char::DecodeUtf16Error;
use esp_idf_svc::hal::prelude::Peripherals;
use esp_idf_svc::hal::uart::config::Config as UartConfig;
use esp_idf_svc::hal::uart::UartRxDriver;
use esp_idf_svc::hal::units::Hertz;

use shrapnel_core::midi_protocol;

mod midi;

#[no_mangle]
extern "C" fn rust_main() -> i32 {
    // It is necessary to call this function once. Otherwise some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_svc::sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    esp_idf_svc::log::EspLogger::initialize_default();

    log::info!("Rust task starting");

    let peripherals = Peripherals::take().unwrap();
    let rx = peripherals.pins.gpio26;

    let config = UartConfig::new().baudrate(Hertz(31_250));
    let uart = UartRxDriver::new(
        peripherals.uart1,
        rx,
        Option::<esp_idf_svc::hal::gpio::Gpio0>::None,
        Option::<esp_idf_svc::hal::gpio::Gpio0>::None,
        &config,
    )
    .unwrap();

    let mut on_midi_message_received =
        |x: &_| log::info!("MIDI received message: {:?}", x);
    let mut midi_decoder =
        midi_protocol::MidiDecoder::new(&mut on_midi_message_received);

    loop {
        let mut byte = [0u8; 1];

        match uart.read(byte.as_mut_slice(), 10).unwrap() {
            0 => (),
            _ => {
                let byte = byte[0];
                log::info!("MIDI received byte: {:x}", byte);
                midi_decoder.decode(byte);
            }
        }
    }
}
