# Dynamic_Instruction_Scheduling

This project simulates an out-of-order superscalar processor based on Tomasulo's algorithm. The simulator fetches, dispatches, and issues N instructions per cycle. <br/>

The simulator reads a trace file with the following format:<br/>
**\<PC\> \<op type\> \<dest reg #\> \<src1 reg #\> \<src2 reg #\>**

The simulator accepts the command-line arguments as follows:<br/>
**sim \<S\>\<N\>\<tracefile\>** <br/>
in which \<S\> is the Scheduling Queue size, \<N\> is the peak fetch and dispatch rate,  issue rate will be N+1, and \<tracefile\> is the input trace file. 
