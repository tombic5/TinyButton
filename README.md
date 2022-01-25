# TinyButton
OneButton https://github.com/mathertel/OneButton Library simplified and optimized to fit in tiny13 AVR uC with MicroCore package.
* used byte instead of int where possible (saved about 200bytes of code)
* removed callback functions with parameter
* no doubleclick callback (but multiclick still works)
* no dynamic parameters, only compile time parameters
* simplified and optimized FSM function
* 19 Bytes of RAM per Button (2 buttons possible in tiny 13)

Total size of compiled library is about 600-800 Bytes, so it fits into tiny13 with some useful callback functions.

