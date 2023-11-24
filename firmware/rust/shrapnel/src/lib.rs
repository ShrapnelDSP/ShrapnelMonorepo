#![no_std]
#[no_mangle]
extern "C" fn rust_main() -> i32 {
    42
}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {
    }
}
