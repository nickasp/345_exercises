# NIKOLAOS ASPRIDIS csd5178
#
# csd5178@csd.uoc.gr
#
# 06/11/24
#
# HY_345 "OPERATING SYSTEMS"
# assignment 2
#
#

I created a thread for each student and a thread for the bus. Except for
main(), the two other important functions used are student_procedure(), that
was executed by each of the student threads, and bus_procedure(), that was
executed by the bus thread. After the initial creation of student threads and
the following creation of bus thread into main(), bus thread becomes the
controller of student threads' actions. Using semaphores, it communicates with
each student thread and insures the effortless execution of the program.
Firstly, all student threads are created, "go" to stop A and signal bus.
Afterwards, bus arrives and decides which students will get in and lets them (one by one)
get into the bus. Then, bus departs for bus stop B. When it arrives there, all
its students (one by one again) hop off at bus stop B and go to University,
where they start studying in parallel. While they are studying, students
that had already been waiting for the bus at bus stop B get into it and bus 
departs for bus stop A. It takes some time for it to reach bus stop A. Meanwhile, 
if a student finishes studying, he leaves University, goes to bus stop B and 
waits for the bus to come again. As soon as bus returns at bus stop A, students 
returned with it go home and their matching threads get destroyed. Bus checks 
if there is at least one student waiting for it either at bus stop A, or at bus 
stop B, and if so, the process described above is repeated. Otherwise, all students
have returned to their home, bus procedure terminates and bus thread gets destroyed. 
It is the end of the program, then. 

