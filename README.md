# OS

This repository contains a collection of operating system (OS) example codes.
Each example code demonstrates a specific concept in OS design, such as interrupt systems, semaphores, and more.

## Signals
`signals.c` simulates the operation of an ideal interrupt system with four priority levels. Interrupts can be triggered either by keyboard input or by a signal from
another program. The simulator displays the state of the system every time there is a change in the system, including whether the main program or an interrupt handler
is currently executing, along with the values of all relevant data structures.

To compile run `make signals`.

## Shell
`shell.c` is a basic shell program that can:
    - run a program in the foreground (first plan)
    - run multiple programs in the background
    - display a list of running background processes
    - gracefully terminate a specific background process using the SIGTERM signal, or forcefully terminating it using the SIGKILL signal

To compile run `make shell`.

## Monitor
`monitor.c` is an example solution to the concurrent search-insert-delete problem.
A single shared data structure is being utilized simultaneously by three distinct categories of independent entities: searchers, inserters, and deleters.
A *searcher* can inspect the data structure without making any changes, allowing for the safe concurrent access of the structure by any number of searchers.
So, any number of concurrent searchers can be accessing the structure safely.
An *inserter* has the capability to add new elements to the end of the data structure. Only one inserter can access the structure at any given time,
and this can occur concurrently with any number of searchers.
A *deleter* can remove items from any position in the structure. As such, any deleter demands exclusive access to the structure.
While a deleter is accessing the data structure, no other deleter, inserter, or searcher is allowed to access it.

To compile run `make monitor`.

## Semaphore
`semaphore.c` simulates a control system that consists of NIB input, NWB worker, and NOB output branches.
The system has NWB input circular buffers UMS[ ], and NOB output circular buffers IMS[ ].

Every input thread reads its input (sensor, camera, etc.) every X seconds (for example, 5-10) with `U = get_input(I)`,
processes it with `T = process_input(I, U)` and saves it in a circular input buffer UMS[T] (with a specific index; in fact, the same
sensor can give multiple information, temperature, pressure, humidity; the camera can detect various events;
each reading can give different information). If the input buffer UMS[T] is full, the thread overwrites the oldest data:
in addition to writing the new data over the oldest data with `UMS[INSERT] = U; INSERT = (INSERT + 1) Mod N;` it is also necessary to move
the output pointer `OUT = (OUT + 1) Mod N`.

The worker thread J is waiting for data in its intermediate storage UMS[J]. If it exists, it takes it (`P=UMS[OUTPUT]; OUTPUT = (OUTPUT + 1) MOD N`),
processes it with `process(J,P,&r,&t)` and puts the result r in the output intermediate storage IMS[t].
Similar to the input thread, if the output intermediate storage is full, the oldest data in it will be overwritten.
The processing time is simulated (2-3 seconds, randomly) before placing it in IMS[t].

The output thread K sends a value to its output every Y seconds.
It will take the value from the output buffer IMS[K] if there are new data in it.
Otherwise, it sends the data that it sent in the previous iteration. It will initially send zero while there are no data in IMS[K].

To compile run `make semaphore`

## Paging
`paging.c` simulates paging on a computer with 16-bit logical addresses and a page size of 64 octets.
After each memory access request, the content of all used data structures is printed.

To compile run `make paging`.
