# Huffman-Encoding
Program that does a representation of huffman encoding word by word. It can also decode data that is being input in the same way that it outputs data.
Coded in C++

It can:

1 - Encode a sentence, given the sentence, symbols to encode with and distribution

2 - Encode a set of symbols, given the symbols, probabilities, symbols to encode with and distribution

3 - Decode a sentence, given the sentence, the symbols and their encoded value

Results output to a "data.txt" the program creates in the same folder as it is run into

Flaws:

1 - Mixed data types, the program is written extensively using std::vector and std::string, for their great storage capacity, however not all components use vectors or strings. Example: The number of steps required for the program to run through in the encoding process is saved as a pointer to int, loops use int, and probably the worst offender is the encoding process assigns chars for the encoding process, which will loops around if there's too many.

End result of this flaw: Program is limited in the amount of text it can process, despite the fact that outside of these flaws, it doesn't have a realistical upper limit to the amount of text it can encode.

Possible fix: First and foremost, replace the assignment of unique chars to symbols for the encoding process (to prevent the following situation: "A", "B", ... "<last char>", "A", "B", ... ). Secondly to replace all ints to a data type that has a capacity similar to vector/string.
  
  2 - Limited input and output, specifically only input being windows console and only output being output to a .txt file. These problems lead to: Limited input of text because any newline will end the input and the maximum input size, despite the input text being saved to an std::string, it is limited to the maximum size of input text to windows console. Output to txt leads to symbols outside of the english language not being correctly displayed, thus limiting the encoded text to english only. 
  
  Possible fix: Allow input from a text document with customizable ending ( number of phrases/sentences/paragraphs ) and ability to read until end of file, as opposed to current iteration which is only until new line. Find a better format to output to ( customizable? ) that would prevent data loss due to its own limits.
