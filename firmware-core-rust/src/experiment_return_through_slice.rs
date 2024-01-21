struct SliceWriter {}

impl SliceWriter {
    fn write<'a>(buffer: &'a mut [u8], len: usize, fill: u8) -> &'a mut [u8] {
        for i in 0..len {
            buffer[i] = fill;
        }

        &mut buffer[0..len]
    }
}

#[cfg(test)]
mod test {
    use crate::experiment_return_through_slice::SliceWriter;
    use std::println;

    #[test]
    fn test() -> () {
        let mut buffer = [0u8; 8];

        let slice = SliceWriter::write(buffer.as_mut_slice(), 2, 42);
        println!("{:?}", slice);
        println!("{:?}", buffer);
    }
}
