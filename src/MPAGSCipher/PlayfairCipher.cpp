#include "PlayfairCipher.hpp"

#include <algorithm>
#include <string>

/**
 * \file PlayfairCipher.cpp
 * \brief Contains the implementation of the PlayfairCipher class
 */

PlayfairCipher::PlayfairCipher(const std::string& key)
{
    this->setKey(key);
}

void PlayfairCipher::setKey(const std::string& key)
{
    // Store the original key
    key_ = key;

    // Append the alphabet to the key
    key_ += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Make sure the key is upper case
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   ::toupper);

    // Remove non-alphabet characters
    key_.erase(std::remove_if(std::begin(key_), std::end(key_),
                              [](char c) { return !std::isalpha(c); }),
               std::end(key_));

    // Change J -> I
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   [](char c) { return (c == 'J') ? 'I' : c; });

    // Remove duplicated letters
    std::string lettersFound{""};
    auto detectDuplicates = [&](char c) {
        if (lettersFound.find(c) == std::string::npos) {
            lettersFound += c;
            return false;
        } else {
            return true;
        }
    };
    key_.erase(
        std::remove_if(std::begin(key_), std::end(key_), detectDuplicates),
        std::end(key_));

    // Store the coordinates of each letter
    // (at this point the key length must be equal to the square of the grid dimension)
    for (std::size_t i{0}; i < keyLength_; ++i) {
        std::size_t row{i / gridSize_};
        std::size_t column{i % gridSize_};

        auto coords = std::make_pair(row, column);

        charLookup_[key_[i]] = coords;
        coordLookup_[coords] = key_[i];
    }
}

std::string PlayfairCipher::applyCipher(const std::string& inputText,
                                        const CipherMode /*cipherMode*/) const
{
    // Create the output string, initially a copy of the input text
    std::string outputText{inputText};

    // Change J -> I
    std::transform(std::begin(outputText), std::end(outputText), std::begin(outputText),
                   [](char c) { return (c == 'J') ? 'I' : c; });


    std::string newString{""};

    // Find repeated characters and add an X (or a Q for repeated X's)
    // If the size of the input is odd, add a trailing Z

    for(std::size_t i{0}; i<outputText.size(); i++) {
        char a = outputText[i];

        if (i + 1 >= outputText.size()) {
            newString.push_back(a);
            newString.push_back('Z');  
            break;
        }

        char b = outputText[i + 1];

        if (a == b) {
            newString.push_back(a);
            newString.push_back(a == 'X' ? 'Q' : 'X');
            i += 1;  
        } else {
            newString.push_back(a);
            newString.push_back(b);
            i += 2;   
        }
    }

    outputText.swap(newString);


    // Loop over the input bigrams
    for(std::size_t i{0}; i<outputText.size(); i+=2) {

        // - Find the coordinates in the grid for each bigram
        std::size_t index1 = key_.find(outputText[i]);
        std::size_t index2 = key_.find(outputText[i+1]);

        int row1 = index1 / 5;
        int col1 = index1 % 5;

        int row2 = index2 / 5;
        int col2 = index2 % 5;

        // - Apply the rules to these coords to get new coords
        if(row1==row2){
            if(col1==gridSize_-1){
                col1=0;
            };
            if(col2==gridSize_-1){
                col2=0;
            };
            if(col1!=gridSize_-1) {
                col1+=1;
            };
            if(col2!=gridSize_-1) {
                col2+=1;
            }
        } else if(col1==col2){
            if(row1==gridSize_-1){
                row1=0;
            };
            if(row2==gridSize_-1){
                row2=0;
            };
            if(row1!=gridSize_-1) {
                row1+=1;
            };
            if(row2!=gridSize_-1) {
                row2+=1;
            }
        } else{
            int shift = col2-col1;
            col1+=shift;
            col2-=shift;
        };

        std::size_t newIndex1 = row1 * gridSize_ + col1;
        std::size_t newIndex2 = row2 * gridSize_ + col2;

        // - Find the letters associated with the new coords
        // - Make the replacements

        outputText[i]   = key_.at(newIndex1);
        outputText[i+1] = key_.at(newIndex2);

    };

    

    
    

    

    

    // Return the output text
  

    return outputText;
}
