mod shrapnel {
    mod parameters {
        trait AudioParameters {
            /// Update a parameter
            /// 
            /// # Arguments
            /// 
            /// * `id` - Name of the parameter to update
            /// * `value` - New normalised value of the parameter
            /// 
            /// returns 0 on success
            fn update(id: str, value: f32) -> i32;
        }
    }
}
