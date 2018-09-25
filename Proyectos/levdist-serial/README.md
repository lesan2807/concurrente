# Levenshtein Distance

Levenshtein distance is a measure of the similarity between two strings: the source string and the target string. The distance is the number of deletions, insertions, or substitutions required to transform the source string into the target string.
For example:
* If source string is "hello world" and target string is "hello world", then their Levenshtein distance is 0. The strings are already identical.
* If source string is "hello world" and target string is "hello waves", then their Levenshtein distance is 4. There are 4 substitutions.
The greater the Levenshtein distance, the more different the strings are.

For more information on how the algorithm is implemented: https://people.cs.pitt.edu/~kirk/cs1501/Pruhs/Spring2006/assignments/editdistance/Levenshtein%20Distance.htm
https://en.wikipedia.org/wiki/Levenshtein_distance

This program compares files stored in directories using the Levenshtein Distance. It lists the files from most similar to less similar.
For example, in a directory called camel_snake there are 4 files. One contains  "whatIsFasterToRead", the second one contains "what_is_fasterToRead", the third one contains "WHAT_IS_FASTER_TO_READ" and the last one contains "what_is_faster_to_read".  
The program will then produce this output:  
4	files/camel.txt	files/camel_snake.txt  
4	files/camel_snake.txt	files/snake.txt  
8	files/camel.txt	files/snake.txt  
18	files/camel.txt	files/const.txt  
18	files/camel_snake.txt	files/const.txt  
18	files/const.txt	files/snake.txt  

The Levenshtein Distance algorithm is used for:
* Spell cheking
* Plagiarism detection
* DNA analysis (https://arxiv.org/pdf/1208.5713.pdf)

## User manual

Usage: levdist [-qr][-w W] DIRS|FILES  
Finds most similar files using Levenshtein distance  
Options:  
      --help       Prints this help  
      --version    Prints information about this command  
  -q, --quiet      Do not print elapsed time (e.g: for testing)  
  -Q, --silent     Do not generate output at all (for testing)  
  -r, --recursive  Analyze files in subdirectories  
  -w W             Use W workers (threads)  

If you write bin/./levdist files --help it will show how to use the command.  
If you write bin/./levdist -q -r files it will not print the elapsed time and it will search recursively in all the subdirectories for the files.
You can also write the files you want to analyze: bin/./levdist -q file1.txt file2.txt file3.txt. This also will not print the elapsed time.  
If you write bin/./levdist files -q -r -w 16, the program is run with 16 workers (threads).
If you write bin/./levdist -Q -r -files, there will no be output generated.  

## Building

Requisites:

1. A POSIX Unix operating system
2. A GCC-compatible compiler

To build the levdist program run the command "make". This will build the program on debug mode.   
In order to remove the build files run the command "make clean".   
To build the levdist program in release mode run the command "make release".   
In order to install the levdist program run the command "make install". This will add the program to the bin folder of the operative system. This allows you to run your program outside of the project folder. If it has not been built, this command will build levdist in release mode.  
In order to uninstall the levdist program run the command "make uninstall".  

## Testing

The testing done for this program is black box testing.  
For more information: http://softwaretestingfundamentals.com/black-box-testing/

In order to test the program with the case files provided, you have to run the command "make test". This will install the program cdiff, if you do not have it installed. It will then compare the expected output with the one produced by the program. And it will also compare the expected error with the produced error.
<Explain how to run the automated tests>


## Author:

Lucía Elizondo   
lesan2807@gmail.com  
This is part of a course's assignment in order to study serial vs. parallel programming.

This program is protected under the MIT license.
Copyright <YEAR> <COPYRIGHT HOLDER>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
