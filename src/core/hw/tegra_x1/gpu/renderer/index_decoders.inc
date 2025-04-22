DEFINE_DECODER_PROTOTYPE(quads_to_triangles) {
    for (u32 i = 0; i < count / 4; i++) {
        const u32 base = i * 4;
        const u32 base_index = i * 6;
        ADD_INDEX(base + 0);
        ADD_INDEX(base + 1);
        ADD_INDEX(base + 2);
        ADD_INDEX(base + 0);
        ADD_INDEX(base + 2);
        ADD_INDEX(base + 3);
    }
}
