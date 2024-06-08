#include <iostream>
#include <fstream>
#include <cstring>

class FileHandler {
public:
    void fileSave(const char** linesArray, int currLine) {
        char fileName[128];
        std::cout << "Enter the file name for saving: ";
        std::cin.getline(fileName, 128);

        std::ofstream file(fileName);
        if (!file) {
            std::cout << "Error opening file for writing." << std::endl;
            return;
        }

        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL) {
                file << linesArray[i] << std::endl;
            }
        }

        file.close();
        std::cout << "Text has been saved successfully." << std::endl;
    }

    void fileLoad(char** linesArray, size_t* lineSizes, int& currLine, int& maxLines) {
        char fileName[128];
        std::cout << "Enter the file name for loading: ";
        std::cin.getline(fileName, 128);

        std::ifstream file(fileName);
        if (!file) {
            std::cout << "Error opening file for reading." << std::endl;
            return;
        }

        char line[128];
        int lineIndex = currLine + 1;
        while (file.getline(line, 128)) {
            size_t len = strlen(line);
            if (lineIndex >= maxLines) {
                maxLines *= 2;
                linesArray = (char**)realloc(linesArray, maxLines * sizeof(char*));
                lineSizes = (size_t*)realloc(lineSizes, maxLines * sizeof(size_t));
                if (linesArray == NULL || lineSizes == NULL) {
                    std::cout << "Memory reallocation failed." << std::endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }
                for (int i = lineIndex; i < maxLines; i++) {
                    linesArray[i] = NULL;
                    lineSizes[i] = 0;
                }
            }

            linesArray[lineIndex] = (char*)malloc((len + 1) * sizeof(char));
            if (linesArray[lineIndex] == NULL) {
                std::cout << "Memory allocation failed." << std::endl;
                file.close();
                return;
            }
            strcpy_s(linesArray[lineIndex], len + 1, line);
            lineSizes[lineIndex] = len + 1;
            lineIndex++;
        }

        file.close();
        currLine = lineIndex - 1;
        std::cout << "Text has been loaded successfully." << std::endl;
    }
};

class TextEditor {
private:
    char** linesArray;
    size_t* lineSizes;
    int currLine;
    int maxLines;

public:
    TextEditor() {
        maxLines = 10;
        currLine = 0;
        linesArray = (char**)malloc(maxLines * sizeof(char*));
        lineSizes = (size_t*)malloc(maxLines * sizeof(size_t));
        if (linesArray == NULL || lineSizes == NULL) {
            std::cout << "Memory allocation failed." << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < maxLines; i++) {
            linesArray[i] = NULL;
            lineSizes[i] = 0;
        }
    }

    void addText() {
        char text[128];
        std::cout << "Enter text to append: ";
        std::cin.getline(text, 128);

        if (strlen(text) > 0) {
            if (linesArray[currLine] == NULL) {
                linesArray[currLine] = (char*)malloc((strlen(text) + 1) * sizeof(char));
                if (linesArray[currLine] == NULL) {
                    std::cout << "Memory allocation failed." << std::endl;
                    return;
                }
                strcpy_s(linesArray[currLine], strlen(text) + 1, text);
                lineSizes[currLine] = strlen(text) + 1;
            }
            else {
                size_t lineArraylen = strlen(linesArray[currLine]);
                lineSizes[currLine] += (strlen(text) + 1);
                linesArray[currLine] = (char*)realloc(linesArray[currLine], lineSizes[currLine] * sizeof(char));
                if (linesArray[currLine] == NULL) {
                    std::cout << "Memory reallocation failed." << std::endl;
                    return;
                }
                strcat_s(linesArray[currLine], lineSizes[currLine], " ");
                strcat_s(linesArray[currLine], lineSizes[currLine], text);
            }

            std::cout << "Current line content: " << linesArray[currLine] << std::endl;
        }
        else {
            std::cout << "Error reading input." << std::endl;
        }
    }

    void newLine() {
        currLine++;
        if (currLine >= maxLines) {
            maxLines *= 2;
            linesArray = (char**)realloc(linesArray, maxLines * sizeof(char*));
            lineSizes = (size_t*)realloc(lineSizes, maxLines * sizeof(size_t));
            if (linesArray == NULL || lineSizes == NULL) {
                std::cout << "Memory reallocation failed." << std::endl;
                exit(EXIT_FAILURE);
            }
            for (int i = currLine; i < maxLines; i++) {
                linesArray[i] = NULL;
                lineSizes[i] = 0;
            }
        }
        std::cout << "New line is started" << std::endl;
    }

    void outputAllText() {
        bool hasContent = false;
        std::cout << "Here is all your lines which were written down: " << std::endl;
        for (int i = 0; i <= currLine; i++) {
            if (linesArray[i] != NULL && strlen(linesArray[i]) > 0) {
                std::cout << linesArray[i] << std::endl;
                hasContent = true;
            }
        }

        if (!hasContent) {
            std::cout << "No non-empty lines found." << std::endl;
        }
    }

    void addTextCoordinates() {
        int lineIndex, charIndex;
        char insertText[128];

        std::cout << "Choose line and index: ";
        std::cin >> lineIndex >> charIndex;
        std::cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            std::cout << "Invalid line index or line is empty." << std::endl;
            return;
        }

        std::cout << "Enter text to insert: ";
        std::cin.getline(insertText, 128);

        if (strlen(insertText) > 0) {
            size_t lenInsertText = strlen(insertText);

            size_t lineLen = strlen(linesArray[lineIndex]);

            if (charIndex < 0 || (size_t)charIndex > lineLen) {
                std::cout << "Invalid character index." << std::endl;
                return;
            }

            lineSizes[lineIndex] += lenInsertText;
            linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
            if (linesArray[lineIndex] == NULL) {
                std::cout << "Memory reallocation failed." << std::endl;
                return;
            }

            memmove(&linesArray[lineIndex][charIndex + lenInsertText], &linesArray[lineIndex][charIndex], lineLen - charIndex + 1);
            memcpy(&linesArray[lineIndex][charIndex], insertText, lenInsertText);

            std::cout << "Text has been inserted successfully." << std::endl;
            std::cout << "Updated line content: " << linesArray[lineIndex] << std::endl;
        }
        else {
            std::cout << "Error reading input." << std::endl;
        }
    }

    void searchWord() {
        char searchTerm[128];
        std::cout << "Enter text to search: ";
        std::cin.getline(searchTerm, 128);

        if (strlen(searchTerm) > 0) {
            bool found = false;
            for (int i = 0; i <= currLine; i++) {
                if (linesArray[i] != NULL) {
                    char* pos = strstr(linesArray[i], searchTerm);
                    while (pos != NULL) {
                        int index = (int)(pos - linesArray[i]);
                        if (found) {
                            std::cout << ", ";
                        }
                        std::cout << i << " " << index;
                        found = true;
                        pos = strstr(pos + 1, searchTerm);
                    }
                }
            }

            if (!found) {
                std::cout << "Has not found your phrase." << std::endl;
            }
            else {
                std::cout << std::endl;
            }
        }
        else {
            std::cout << "Error reading search." << std::endl;
        }
    }

    void deleteChars() {
        int lineIndex, charIndex, numChars;

        std::cout << "Choose line, index and number of symbols: ";
        std::cin >> lineIndex >> charIndex >> numChars;
        std::cin.ignore();

        if (lineIndex < 0 || lineIndex >= maxLines || linesArray[lineIndex] == NULL) {
            std::cout << "Invalid line index or line is empty." << std::endl;
            return;
        }

        size_t lineLen = strlen(linesArray[lineIndex]);

        if (charIndex < 0 || charIndex >= lineLen || numChars <= 0 || (size_t)(charIndex + numChars) > lineLen) {
            std::cout << "Invalid character index or number of symbols." << std::endl;
            return;
        }

        memmove(&linesArray[lineIndex][charIndex], &linesArray[lineIndex][charIndex + numChars], lineLen - charIndex - numChars + 1);
        lineSizes[lineIndex] -= numChars;
        linesArray[lineIndex] = (char*)realloc(linesArray[lineIndex], lineSizes[lineIndex] * sizeof(char));
        if (linesArray[lineIndex] == NULL) {
            std::cout << "Memory reallocation failed." << std::endl;
            return;
        }

        std::cout << "Characters deleted successfully." << std::endl;
        }

    void run() {
        int userChoice = -1;
        while (userChoice != 0) {
            std::cout << "Choose the command:" << std::endl;
            std::cin >> userChoice;
            std::cin.ignore();

            switch (userChoice) {
            case 0:
                std::cout << "Exiting program." << std::endl;
                break;
            case 1:
                addText();
                break;
            case 2:
                newLine();
                break;
            case 3:
                FileHandler().fileSave((const char**)linesArray, currLine);
                break;
            case 4:
                FileHandler().fileLoad(linesArray, lineSizes, currLine, maxLines);
                break;
            case 5:
                outputAllText();
                break;
            case 6:
                addTextCoordinates();
                break;
            case 7:
                searchWord();
                break;
            case 8:
                deleteChars();
                break;
            default:
                std::cout << "Invalid choice, please try again." << std::endl;
            }
        }
    }

    ~TextEditor() {
        for (int i = 0; i < maxLines; i++) {
            free(linesArray[i]);
        }
        free(linesArray);
        free(lineSizes);
    }
};
int main() {
    TextEditor editor;
    editor.run();

    return 0;
}


