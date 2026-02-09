This experiment checks the `fs/` subdirectory of the Linux kernel source tree.
I derived `notable.json` by going through `fs.h` and trying to list all
functions which weren't simple getters.

Something I noticed while going through the sources was the presence of a
`__must_check` macro. It might be more productive to, rather than scan all
functions, or even just "notable" ones, to only track when the return value of
`__must_check` functions are propagated to others and ignored.
