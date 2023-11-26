#![no_std]

mod audio_param;
mod selected_preset_api;

#[no_mangle]
extern "C" fn rust_main() -> i32 {
    42
}

#[cfg(target_os = "none")]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

#[cfg(test)]
mod tests {
    use crate::rust_main;

    #[test]
    fn it_works() {
        let result = rust_main();
        assert_eq!(result, 42);
    }
}
