

# Question 1:
Some of the APIs used to allocate memory in Linux are:

|  API       | Description                                                                               |
|:----------:|-------------------------------------------------------------------------------------------|
| `vmalloc`  | Useful for allocating virtually contiguous memory                                         |
| `kzmalloc` | This will allocate specified memory size and set it all memory allocated to zero          |
| `mmap`     | API used to mapped files or devices into memory                                           |
| `realloc`  | Reallocates called on memory previously allocated by malloc and not yet freed with `free` |

# Question 2:
The reason a deferenced p may cause a SIGSEGV is due to when the address is passed we get the current task struct for our process
and from there are able to get the head of the linked list of VMAs that our process encompasses. If the virtual address we are 
looking for is within these VMAs then the TLB will be able to return to us the frame number and we can get specific info about this address
from the vma. The issue is that the address that we are looking for is not contained within the processes page table and therefore 
not within the VMAs of the process and because of this a SIGSEGV will occur.


