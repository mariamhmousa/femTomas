# femTomas
Assumptions: 
* ret instruction is working properly since its implemented similar to jalr
* Memory is implemented as an int array of size 64.
* Instruction are stored in a vector and is separate from data memory 


What is working: All instructions are working properly. However, beq has some issues as shown in the results section of the report.


Issues:  
Beq delays the issuing of its following instructions (before the target instruction if the branch is taken) which causes the output to look as if the instructions were not issued in order even if our algorithm forces them to do so.
