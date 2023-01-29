#include "../libraries/customTerminalBitMap.hpp"
#include <string>

#define maxArraySize 128

int map(int input, int min1, int max1, int min2, int max2){
    return (int)((input - min1) * (((max2 - min2) * 1.000000) / ((max1 - min1) * 1.000000)) + min2);
}

void sort(int input[maxArraySize], int *outputInt, char *outputChar){
    int charOutput[maxArraySize];
    int intOutput[maxArraySize];

    for(int i = 0;i < maxArraySize;i++){
        charOutput[i] = 0;
        intOutput[i] = 0;
    }

    for(int i = 0;i < maxArraySize;i++){
        if(input[i] > 0){
            for(int j = 0;j < maxArraySize;j++){
                if(intOutput[j] <= input[i]){
                    for(int k = maxArraySize - 1;k > j;k--){
                        if(intOutput[k - 1] <= 0)continue;
                        intOutput[k] = intOutput[k - 1];
                        charOutput[k] = charOutput[k - 1];
                    }

                    intOutput[j] = input[i];
                    charOutput[j] = (char)i;

                    break;
                }
            }
        }
    }

    for(int i = 0;i < maxArraySize;i++){
        if(intOutput[i] == 0 && charOutput[i] == 0)break;

        *(outputInt + i) = intOutput[i];
        *(outputChar + i) = charOutput[i];
    }

    return;
}

void drawRow(int height, char character, double percentage, int timesEncountered){
    std::string timesEncounteredString = std::to_string(timesEncountered);

    terminal::bm::drawPixel(5, height, '|');
    terminal::bm::drawPixel(7, height, '[', YELLOW);
    terminal::bm::drawPixel(terminal::bm::length - timesEncounteredString.length() - 4, height, ']', YELLOW);
    terminal::bm::printText(terminal::bm::length - timesEncounteredString.length() - 2, height, GREEN, BLACK, timesEncounteredString);
    terminal::bm::drawPixel(terminal::bm::length - 1, height, '|');
    terminal::bm::printText(1, height, CYAN, BLACK, "'", character, "'");

    int lineLength = map(percentage, 0, 100, 8, terminal::bm::length - timesEncounteredString.length() - 4);
    if(lineLength <= 8)terminal::bm::drawPixel(8, height, '=', YELLOW);
    else terminal::bm::drawLine(8, height, lineLength, height, '=', YELLOW);

    return;
}

bool manageScroll(int position, char* character, double* percentage, int* timesEncountered){
    if(position < 0 || *(timesEncountered + terminal::bm::height - 6 + position) <= 0)return false;

    for(int i = 0;i <= terminal::bm::height - 6;i++){
        terminal::bm::drawLine(1, i + 4, terminal::bm::length - 2, i + 4, ' ');
        drawRow(i + 4, *(character + i + position), *(percentage + i + position), *(timesEncountered + i + position));
    }
    terminal::bm::updateDisplay();

    return true;
}

int main(void){
    int encounterTimes[maxArraySize];
    int encounterTimesSortedInt[maxArraySize];
    char encounterTimesSortedChar[maxArraySize];
    double percentageSorted[maxArraySize];
    unsigned long totalSum = 0;

    terminal::bm::init();

    terminal::bm::printText(0, terminal::bm::height - 1, WHITE, BLACK, "Input a string : ");
    terminal::bm::updateDisplay();

    terminal::in::get_str();
    char charArrayInput[terminal::in::str_length];
    terminal::in::store_str(charArrayInput);

    std::string input;
    for(int i = 0;i < sizeof(charArrayInput);i++)input[i] = charArrayInput[i];

    for(int i = 0;i < maxArraySize;i++){
        encounterTimes[i] = 0;
        encounterTimesSortedChar[i] = '\0';
        encounterTimesSortedInt[i] = 0;
        percentageSorted[i] = 0.0000;
    }

    for(int i = 0;i < input.length();i++)encounterTimes[(int)input[i]]++;
    for(int i = 0;i < maxArraySize;i++){
        if(encounterTimes[i] <= 0)continue;
        totalSum += encounterTimes[i];
    }

    sort(encounterTimes, encounterTimesSortedInt, encounterTimesSortedChar);
    for(int i = 0;encounterTimesSortedInt[i] > 0;++i){
        percentageSorted[i] = (encounterTimesSortedInt[i] * 1.0000f / totalSum * 1.0000f) * 100.0000f;
    }

    terminal::bm::printText(1, 1, YELLOW, BLACK, "Histogram of the most used letters");
    terminal::bm::printText(1, 2, YELLOW, BLACK, "In the string : '", input, "'");
    terminal::bm::drawRect(0, 0, terminal::bm::length - 1, terminal::bm::height - 1, '+', GREEN);
    terminal::bm::drawLine(0, 3, terminal::bm::length - 1, 3, '=', GREEN);

    for(int i = 0;encounterTimesSortedInt[i] > 0;i++){
        if(i > terminal::bm::height - 6){
            terminal::bm::printText(terminal::bm::length - 33, 1, RED, BLACK, "Use the arrows on your keyboard");
            terminal::bm::printText(terminal::bm::length - 22, 2, RED, BLACK, "To scroll up or down.");
            terminal::bm::updateDisplay();

            int scroll = 0;
            bool breakCycle = false;

            for(;;){
                switch(terminal::in::get_ch()){
                    case KEY_UP: if(!manageScroll(--scroll, encounterTimesSortedChar, percentageSorted, encounterTimesSortedInt))scroll++;;break;
                    case KEY_DOWN: if(!manageScroll(++scroll, encounterTimesSortedChar, percentageSorted, encounterTimesSortedInt))scroll--;break;

                    default : breakCycle = true;break;
                }

                if(breakCycle)break;
            }

            break;
        }
        drawRow(i + 4, encounterTimesSortedChar[i], percentageSorted[i], encounterTimesSortedInt[i]);
    }

    terminal::bm::updateDisplay();
    terminal::in::get_ch();

    terminal::bm::end();

    return 0;
}
