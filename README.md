# udpClientServer transfers binary files on the client computer to files on the server.
It is 'generic' code that demonstrates building c language projects on a Linux pc. 
Since UDP protocol doesn't guarantee arrival or order of packets, the client assures that
the server echo the packet number before sending the next packet.
Packet size is limited to 8972 bytes (jumbo packets). File size is intended to be unlimited.

For development, Visual Studio Code is used as the IDE. 
Two projects are involved:
   1. Client prompts the operator for a file name or file containing
      a list of files, then transmits them to server
   2. Server receives udp data and writes it to file(s) named with
      date_time.bin
Invoking the bin/server or bin/server with no argument lists the available command line options.
Each VS project is built using options in tasks.json, and launched
using options in launch.json (where command line options are placed to run with the gdb debugger).

