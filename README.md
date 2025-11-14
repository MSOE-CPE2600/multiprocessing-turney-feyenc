# System Programming Lab 11 Multiprocessing

- creates a semaphore that only allows the specified number of child processes
- spawns processes to creates and save each frame
- uses the semaphore to wait until all frames are created
- frees allocated memory

|processes|1|2|5|10|20|50|
|-|-|-|-|-|-|-|
|seconds| 18.9 | 10.1 | 4.7 | 2.8 | 2.6 | 2.6 |

```mermaid
xychart
    title "Time by Processes"
    x-axis [1, 2, 5, 10, 20, 50]
    y-axis "Seconds" 0 --> 20
    bar [18.9, 10.1, 4.7, 2.8, 2.6, 2.6]
```
generally adding more processes makes the program run faster, although no sigificant improvents are made after 10 processes are created this may be because my computer has 12 cores so anything only 12 processes can run at a time any way