# Integer Java-Virtual Machine
*An IJVM emulator by Khanh Vu  
OS: Ubuntu 18.04 LTS  
https://github.com/khanhvu207/Integer-Java-Virtual-Machine*

## Run the code

1. Compile all necessary files using Make
  ```
  $ make
  ```
or
  ```
  $ make ijvm
  ```
2. Run basic and advanced tests
  ```
  $ make testall
  ```
3. Check for memory leaks (Valgrind)  
    1. Install Valgrind
    ```
    $ sudo apt-get install valgrind
    ```  
    2. Run tests
    ```
    $ make testleaks
    ```
4. Run the IJVM on a specific .ijvm binary file
```
$ ./ijvm <binary_file_directory>
```
## Implementations
### Stack
The stack was implemented accordingly to the *Structured computer organization* by Andrew S. Tanenbaum. Each frame is bounded by the **Stack pointer** and **Local variables pointer**. For the ease of implementating the stack, I created an array storing local variables for each frame. 

![stack_img](https://github.com/khanhvu207/Integer-Java-Virtual-Machine/blob/master/img/stack.png)

### Heap
I maintain a list of pointers, each of which points to the start of an array.

![heap_img](https://github.com/khanhvu207/Integer-Java-Virtual-Machine/blob/master/img/heap.png)

## Bonus features
### Garbage collection
For each frame, I keep track of arrays created by the NEWARRAY instruction. Those arrays (**arrayref** values) are pushed to the list, which later on being cleared during the *SWEEP* phase. 

![gc_img](https://github.com/khanhvu207/Integer-Java-Virtual-Machine/blob/master/img/gc.png)

### GUI
I implemented this GUI using GTK+ 2.0.
* Display information: program counter, top of stack.  
* Allow users to select IJVM binary.  
* Allow users to select input and output files.  
* Users can click the STEP button to go to the next state of the machine.

1. Dependencies installation
   ```
   $ sudo apt-get install gtk2.0
   $ sudo apt-get install gimp
   ```
2. Run
   ```
   $ make ijvm-gui
   $ ./ijvm-gui
   
![gui_img](https://github.com/khanhvu207/Integer-Java-Virtual-Machine/blob/master/img/gui.png)

### Network communication (Socket programming)
Standard socket programming, supports BIND (server side) and CONNECT (client side).
