Part A : Crawler

In this project, we wrote a crawler function that crawls pages.

The program works by maintaining two queues : 
a) Links queue - The parser threads parse the downloaded pages and put the links identified in this queue. The downloader threads download the links by fetching from this queue.
b) Page queue - The downloader threads download the pages and put the contents in the page queue. The parsers then fetch these pages for parsing.

In addition to this, we maintain a linked list of links that have already been visited. Every time a new link is found on a page, it is chekced if it is already present in the queue; if so, it is added to this list.

We have used conditional variables and mutex to signal and wait for locks and to maintain atomicity in executing the critical sections.

Part B: xv6 Threads

In this project, we have added a thread library to the xv6 OS. This is done using locks and also using two system calls clone() and join(). The locks are implemented using the xchg instruction which makes sure the lock acquisition and release happens atomically. The clone() system call is similar to the fork() and join() is similar to wait(). The clone system call creates a new process with same address space as that of its  parent and  with a new user stack. When clone returns the execution of new process will begin from thread routine which is set as the next instruction pointer while creating the new process. The join system call makes the calling process wait for the thread to complete execution. 
