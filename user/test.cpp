#include <iostream>
#include <string>

int replaceEvenSpacesWithNewline(std::string& inputString,int isEvenSpace) {
    int spaces=0;
    for (size_t i = 0; i < inputString.length(); ++i) {
        if (inputString[i] == ' ') {
            spaces+=1;
            if (isEvenSpace) {
                inputString[i] = '\n';
            }
            isEvenSpace = !isEvenSpace;
        }
    }
    return spaces%2;
}

int main() {
    std::string inputString = "This is a test string with even spaces";

    std::cout << "Original String: " << inputString << std::endl;

    replaceEvenSpacesWithNewline(inputString,1);

    std::cout << "Modified String: " << inputString << std::endl;

    return 0;
}
