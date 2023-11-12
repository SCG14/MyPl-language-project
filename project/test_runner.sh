#!/bin/bash

# Program 1
./mypl prog1.mypl | tail -n +2 > tests/output1.pl
./mypl --csharp prog1.mypl | tail -n +11 > tests/output1.cs
cmp tests/output1.pl tests/output1.cs

# Program 2
./mypl prog2.mypl | tail -n +2 > tests/output2.pl
./mypl --csharp prog2.mypl | tail -n +11 > tests/output2.cs
cmp tests/output2.pl tests/output2.cs

# Program 3
./mypl prog3.mypl | tail -n +2 > tests/output3.pl
./mypl --csharp prog3.mypl | tail -n +11 > tests/output3.cs
cmp tests/output3.pl tests/output3.cs

# Program 4
./mypl prog4.mypl | tail -n +2 > tests/output4.pl
./mypl --csharp prog4.mypl | tail -n +11 > tests/output4.cs
cmp tests/output4.pl tests/output4.cs

# Program 5
./mypl prog5.mypl | tail -n +2 > tests/output5.pl
./mypl --csharp prog5.mypl | tail -n +11 > tests/output5.cs
cmp tests/output5.pl tests/output5.cs

# Program 6
./mypl prog6.mypl | tail -n +2 > tests/output6.pl
./mypl --csharp prog6.mypl | tail -n +11 > tests/output6.cs
cmp tests/output6.pl tests/output6.cs

# Program 7
./mypl prog7.mypl | tail -n +2 > tests/output7.pl
./mypl --csharp prog7.mypl | tail -n +11 > tests/output7.cs
cmp tests/output7.pl tests/output7.cs
