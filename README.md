# TinyButton
Based on OneButton https://github.com/mathertel/OneButton Library. Simplified and optimized to fit int MicroCore and tiny13 AVR uC.
* used byte instead of int where possible (saved about 200bytes of code)
* removed callback functions with parameter
* no doubleclick callback (but multiclick still works)
* simplified and optimized FSM function

