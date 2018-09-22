#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <cmath>

void removeSpaces(std::string &word){
    for(unsigned int i=0; i < word.length(); i++){
        if(word[i] == ' '){
            word.erase(i,1);
        }
    }
}

void strToLower(std::string &word){
    std::locale loc;
    for(unsigned int i=0; i<word.length(); i++){
        word[i] = tolower(word[i],loc);
    }
}

// Taken from disp.c program provided to us
int convertASCIIHexToInt(char c) {
    if(c >=65 && c <= 70) { // c is a uppercase letter
        return c-55;
    } else if(c >=97 && c <= 102) { // c is a lowercase letter
        return c-87;
    } else if(c >=48 && c <= 57) { // c is a number
        return c-48;
    } else { // Not a hex digit
        return -1;
    }
}

char singleDigitIntToHex(unsigned int i){
    if(i<=9){ //no bottom value since its an unsigned int
        return '0' + i;
    }
    else if(i==10){
        return 'a';
    }
    else if(i==11){
        return 'b';
    }
    else if(i==12){
        return 'c';
    }
    else if(i==13){
        return 'd';
    }
    else if(i==14){
        return 'e';
    }
    else if(i==15){
        return 'f';
    }
    else{
        return '!'; // Error
    }
}

bool isKeyPermutationValid(std::string key){
    for(unsigned int i=0; i<key.length(); i++){
        for(unsigned int j=i+1; j<key.length(); j++){
            if(key[i] == key[j]){
                return false;
            }
        }
    }
    return true;
}



int main(int argc, char **argv)
{
    // By default assume e and b are true
    bool e = true;   //encrypt
    bool d = false;  //decrypt
    bool x = false;  //if the key file is in ASCII-coded hex
    bool b = true;   //if the key file is in binary


    std::string filename;

    for (int i = 1; i < argc; i++) {
        // First thing in this array is the program name, so start at the first flag.
        // Only need to check for d and x since e and b are the defaults
        if(std::strcmp(argv[i], "-d") == 0) {
            d = true;
            e = false;
        } else if(strcmp(argv[i], "-x") == 0) {
            x = true;
            b = false;
        } else if(strcmp(argv[i], "-k") == 0){
            if (argv[i+1] == NULL){
                // Write out to stderr
                std::cerr << "Error: No key file given." << std::endl;
                return 6;
            }
            else{
                filename = argv[i+1];   //assume space after flag and take next argument
            }
        }
    }


    //Program must read from stdin and write to stdout

    std::string keyStr;
    std::ifstream keyFile(filename, std::ifstream::in);

    if(!keyFile.is_open()){
        std::cerr << "Error: Key file not found." << std::endl;
        return 6;
    }

    if(b){ //IF READING KEY FILE IN "BINARY" (arbitrary values 0-255 decimal):
        std::string temp;
        getline(keyFile, temp); // save binary key into temp


        unsigned int bInt; // "binary int"
        //convert temp (binary) into ascii coded hex in keyStr
        for(int i=0; i<8; i++){

            bInt = (unsigned int) temp.at(i); //bInt is 0-255

            // Convert bInt to ASCII coded hex
            unsigned int lowDig = bInt % 16;
            unsigned int result = bInt/16;   //truncated
            unsigned int highDig = result % 16;

            char lowChar, highChar;
            lowChar = singleDigitIntToHex(lowDig);
            highChar = singleDigitIntToHex(highDig);

            // Append to keyStr
            keyStr.push_back(highChar);
            keyStr.push_back(lowChar);
        }

    }
    else if(x){ //IF READING HEX CODED ASCII FILE (0-9, a-f):
        getline(keyFile, keyStr); // take entire line and save into keyStr

        // Convert to lowercase (original key file could have a-f or A-F)
        strToLower(keyStr);

        // Remove spaces
        removeSpaces(keyStr);
    }


    // Check if valid permutation of nybbles in key file
    if(!isKeyPermutationValid(keyStr)){
        std::cerr << "Error: Invalid permutation of nybbles in key file." << std::endl;
        return 6;
    }



    if(e){  // Encrypt mode
        // Set stdin to binary mode
        freopen(NULL, "rb", stdin);

        unsigned char buf[102400]; // Unsigned char buffer to prevent problems with printf
        size_t bytes_read = fread(buf, 1, sizeof(buf), stdin); // Read the file from stdin


        std::stringstream hexPlainTextStream;

        // Convert plaintext (binary) into ascii coded hex in hexPlainText
        for(unsigned int i=0; i < bytes_read; i++){
            hexPlainTextStream << std::setfill('0') << std::setw(2) << std::hex << (int) buf[i];
        }

        std::string hexPlainText(hexPlainTextStream.str());

        unsigned int i=0;
        while(i < hexPlainText.length()){
            // Look at HEX value "pVal" from plaintext (disp function)
            // Replace that value with it's corresponding ciphertext value from the keyString
            // Ex:  cVal = keyString[pVal]      Note: pVal must be converted from ASCII coded hex to an int

            double decimal = 0;
            char currentChar;

            for(int j=16; j>0; j-=15){
                currentChar = keyStr[convertASCIIHexToInt(hexPlainText[i])];

                if(currentChar <= '9' && currentChar >= '0'){
                    decimal += ((double) currentChar - 48)*j;   //multiply by 16 for 1st digit and 1 for the 2nd digit
                }
                else{
                    decimal += ((double) currentChar - 87)*j;   //ditto
                }

                i++;
            }

            std::cout << (char) decimal; //Cipher text digit by digit in decimal "binary"
        }

    }
    else if(d){ // Decrypt mode
        // Set stdin to binary mode
        freopen(NULL, "rb", stdin);

        unsigned char buf[102400]; // Unsigned char buffer to prevent problems with printf
        size_t bytes_read = fread(buf, 1, sizeof(buf), stdin); // Read the file from stdin


        std::stringstream hexCipherTextStream;

        // Convert ciphertext (binary) into ascii coded hex in hexCipherText
        for(unsigned int i=0; i < bytes_read; i++){
            hexCipherTextStream << std::setfill('0') << std::setw(2) << std::hex << (int) buf[i];
        }

        std::string hexCipherText(hexCipherTextStream.str());

        int charCount = 0;
        double decimal = 0;
        int factor = 0;

        for(unsigned int j=0; j<hexCipherText.length(); j++){
            for(unsigned int i=0; i<16; i++){
                char currentChar;

                // Read two digits at a time

                if(keyStr[i] == hexCipherText[j]){
                    charCount++;

                    if(charCount == 1){
                        factor = 16;
                    }
                    else if(charCount == 2){
                        factor = 1;
                    }

                    currentChar = singleDigitIntToHex(i);

                    if(currentChar <= '9' && currentChar >= '0'){
                        decimal += ((double) currentChar - 48)*factor;   //multiply by 16 for 1st digit and 1 for the 2nd digit
                    }
                    else{
                        decimal += ((double) currentChar - 87)*factor;   //ditto
                    }
                }

                if(charCount == 2){
                    charCount = 0;
                    std::cout << (char) decimal; // Plain text digit by digit in decimal "binary"
                    decimal = 0;
                }

            }
        }
    }

    return 0;
}
