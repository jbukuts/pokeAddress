
# pokeAddress
Kernel patch and userspace program that allows for address space to be poked and flags to be returned for given address.

## Compilation
Included makefile has multiple options
- `make` will run the `userspace`  option in the makefile which will simple compile to the program
- `make clean` will clean the working the directory of compiled and object files

## Usage
Once compiled both the case of various variables as well as the entire address space will have it's own option when running the program.

In order to properly execute the various variables case:
`./userspace base`

In order to properly execute the entire address space case:
`./userspace all`

## Design
For the userspace, the design is that the user passes in some agument when running the program (either 'base' or 'all') and from here we can then run that case scenario. In order to invoke the system call from the userspace program though I created a wrapper method that would simple take in a void pointer and return the flags of given pointer as a short. Once the short was return this value would be passed to another method known as 'print_flags' which would take an short as input and print the flags for that given input. The base is simple as we need only declare some variables, get the flags through our wrapper, and then pass them to 'print_flags' for the result. For the case of the entire address space we need iterate through our entire address space by the proper stride length and simple invoke our wrapper and print the returned flags for each iteration.  
