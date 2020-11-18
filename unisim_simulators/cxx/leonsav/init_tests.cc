bool keep_test() { static unsigned counter = 0; return ((counter++ & 0x3f) == 26); }
