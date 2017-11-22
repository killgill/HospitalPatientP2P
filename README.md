# ee450-pa2

Name: Karan Singh Gill
ID: 3453909285

I have fulfilled the assignment to the requirements laid out for me. I did not use fork() because it was not required and instead made patient1, patient2, patient3, patient4 and doctor1 and doctor2.

healthcenter.cpp is the main server, it takes information from the patients and records it in directory.txt. It then verifies the doctors' usernames and passwords and then sends them the filename of the directory. It then doesn't do anything else.

The patient#.cpp create a TCP and UDP socket and then sends the information about its TCP port to the healthcenter over its UDP port. It is then contacted by the doctor over TCP. If there are other patients in the same doctor group, it picks a time from the schedule and then sends the list of remaining patients and the remaining schedule to the first patient on the list. This continues until all patients have been assigned to their doctor. There is a delay at the beginning of each patient program, this ensures that it is easy to see the messages arriving in the console. Every TCP message is followed by an ACK.

Doctor#.cpp create a dynamic TCP port and a UDP port. They use the UDP port to authenticate themselves to the healthcenter and receive the filename with the patients. They read the file, detect which patients are theirs and contact the first patient with the list of other patients and the schedule over TCP. Every TCP message is followed by an ACK.

To run the program, you first do:
./healthcenter & ./patient1 & ./patient2 & ./patient3 & ./patient4 &
Once the console tells you to run the doctors, you run
./doctor1 & ./doctor2 & 

Whenever a message is received by any program, it is printed to the console like so "program_name: message". The only difference in format from that specified in the document is for transmitting the patients information. I first send the number of patients that are left. This avoids an issue where the list of patients would be an empty string and cause the program to stop working because it would keep waiting for an Ack.

Project doesn't fail under any conditions tested - there are compiler warnings, but these do not stop the code from working properly.

Reused code:
Beej's guide and https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html were invaluable resources, as was code from my first assignment for this class.

Additional Notes:
A Makefile is included, please just run make instead of manually compiling.
Detailed inline comments are only given for healthcenter.cpp, patient1.cpp, and doctor1.cpp as the other doctors and patients are too similar.