# Evaluating Haswell Hardware Transactional Memory

### Background

As physical limitations have curtailed the growth of processor clock speeds, CPU designers have turned to increasing the number of processor cores on a single CPU to improve performance. Consequently, application developers must exploit concurrency in their programs to fully utilize the underlying hardware. Unfortunately, traditional lock-based approaches to coordinating access to shared data lead to an unappealing tradeoff between performance and complexity: coarse-grained locking results in increased synchronization overhead, while fine-grained locking imposes a significant mental burden on the programmer, resulting in classic concurrency mishaps: race conditions, deadlock and starvation.
 
  To equip application developers with more powerful abstractions for developing correct concurrent programs, systems researchers have proposed transactional memory as an mechanism for safely modifying shared data. By grouping related updates into transactional regions that are executed atomically by the underlying hardware, application developers are able to indicate where synchronization is needed without concerning themselves with how it is implemented. 

Intel's [Transactional Syncronization Extensions](http://software.intel.com/en-us/blogs/2012/02/07/transactional-synchronization-in-haswell) (TSX) are now enabled on commercially available high end desktop and server grade Haswell CPU's. While  TM-like functionality has been previously provided in a limited manner by the load-linked and store-conditional (LL/SC) instructions present in the Alpha AXP, IBM PowerPC and other architectures [1], as well as in more complete implementations present on some high-end IBM CPUs [citation needed], with TSX, Intel aims to bring a powerful and complete HTM implementation to commodity server processors.

### TSX Overview

Intel provides access to Haswell's transactional capabilities with two separate interfaces: Hardware Lock Elision (HLE) and Restricted Transactional Memory (RTM).

#### Hardware Lock Elision
HLE provides a backwards compatible interface accessible through the `xacquire` and `xrelease` instruction prefixes hints. Programmers can place the hints prior to the corresponding instructions for acquiring and releasing the lock. During execution, the processor will enter the critical section without acquiring the lock and then attempt to execute the critical section transactionally. If the transaction fails, the critical section will be re-executed in a non-transactional fashion, with lock acquisitions. Since the HLE prefixes are ignored on CPU's without TSX, non-TSX processors simply ignore the prefixes and acquire the lock.

#### Restricted Transactional Memory
 								
### References

[1] - Herlihy, Maurice, and Nir Shavit. The art of multiprocessor programming. Amsterdam London: Elsevier/Morgan Kaufmann, 2008. Print.

[2] - Intel x86 architectures software developer's manual (http://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-manual-325462.pdf)


