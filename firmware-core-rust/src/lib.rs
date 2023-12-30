#![no_std]

mod midi_protocol;

pub fn add(left: usize, right: usize) -> usize {
    left + right
}
 
#[cfg(test)]
extern crate std;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
