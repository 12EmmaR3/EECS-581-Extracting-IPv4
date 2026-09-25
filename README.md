# EECS-581-Extracting-IPv4 Prompts Used 
# AI model used: Claude - Sonnet 5 
# Dates the model was used: 9/23, 9/25
# The entirety of this code was AI-generated, however I went through and thororughly understand
# every line of code written, and have tested/debugged the code to ensure it works at intended.

# Turn #1
I need a C function called extractIPv4 that finds an IPv4 address (and port if there is one) somewhere in a line of text and returns it. Can't use atoi/strtol/sscanf/inet_pton/regex or anything like that, has to be done by hand. Also write a main that loops asking for input until the user types END, and prints the result each time.

# Turn #2
This is close but doesn't fully match everything my assignment asks for, fix the following:
- the function signatures have to be exactly int extractIPv4(const char* str, unsigned long* outAddress, int* outPort) for C, and on success outAddress/outPort get filled in (port is -1 if none), on failure outAddress is 0 and outPort is -1
- octets need to be 1-3 digits, 0-255, no leading zeros unless it's literally "0" and have the same rule for the port but 1-5 digits, 0-65535
- if there's a colon but the port part is invalid, reject the WHOLE match, not just drop the port
- if a run of digits/dots/colons doesn't match the full pattern exactly, fail it entirely instead of finding a smaller valid piece inside it (e.g. "1.2.3.4.5" should NOT match as "1.2.3.4")
- only digits/dots/colons count as part of a candidate, everything else is garbage that gets skipped
- if there are multiple valid candidates in the line just take the first one
-print success as exactly Extracted IPv4 address: A.B.C.D (decimal value: N, port: P) where P is the number or "none"
- END has to be an exact case-sensitive match, then print "Program terminated." and exit

# Turn #3
Now write me a file that contains a comprehensive list of test cases that I can use to validate whether this code works or not, I want every possible edge case to be listed within this file 
