#include <stdio.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define BITS_IN_REGISTER 16
#define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)
#define FIRST_QTR (TOP_VALUE / 4 + 1)
#define HALF      (2 * FIRST_QTR)
#define THIRD_QTR (3 * FIRST_QTR)
#define NO_OF_CHARS 256
#define EOF_SYMBOL    (NO_OF_CHARS + 1)
#define NO_OF_SYMBOLS (NO_OF_CHARS + 1)
#define MAX_FREQUENCY 16383

unsigned char index_to_char[NO_OF_SYMBOLS];
//wchar_t index_to_char[NO_OF_SYMBOLS];
int	char_to_index[NO_OF_CHARS];
int cum_freq[NO_OF_SYMBOLS + 1];
int	freq[NO_OF_SYMBOLS + 1];
long low, high;
long value;

long bits_to_follow;
int	buffer;
int	bits_to_go;
int garbage_bits;
int _stateMenu;

int test_counter = 0;

FILE* in, * out;

void start_model(void)
{
    for (int i = 0; i < NO_OF_CHARS; i++)
    {
        char_to_index[i] = i + 1;
        index_to_char[i + 1] = i;
    }
    for (int i = 0; i <= NO_OF_SYMBOLS; i++)
    {
        freq[i] = 1;
        cum_freq[i] = NO_OF_SYMBOLS - i;
    }
    freq[0] = 0;
}

void update_model(int symbol)
{
    int i;
    int ch_i, ch_symbol;
    int cum;

    if (cum_freq[0] == MAX_FREQUENCY)
    {
        cum = 0;
        for (i = NO_OF_SYMBOLS; i >= 0; i--)
        {
            freq[i] = (freq[i] + 1) / 2;
            cum_freq[i] = cum;
            cum += freq[i];
        }
    }
    for (i = symbol; freq[i] == freq[i - 1]; i--);
    if (i < symbol)
    {
        ch_i = index_to_char[i];
        ch_symbol = index_to_char[symbol];
        index_to_char[i] = ch_symbol;
        index_to_char[symbol] = ch_i;
        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }
    freq[i] += 1;
    while (i > 0)
    {
        i -= 1;
        cum_freq[i] += 1;
    }
}

void start_inputing_bits(void)
{
    bits_to_go = 0;
    garbage_bits = 0;
}

int input_bit(void)
{
    int t;
    if (bits_to_go == 0)
    {
        buffer = getc(in);
        if (buffer == EOF)
        {
            garbage_bits += 1;
            if (garbage_bits > BITS_IN_REGISTER - 2)
            {
                printf("Bad input file\n");
                exit(-1);
            }
        }
        bits_to_go = 8;
    }
    t = buffer & 1;
    buffer >>= 1;
    bits_to_go -= 1;
    return t;
}

void start_outputing_bits(void)
{
    buffer = 0;
    bits_to_go = 8;
}

void output_bit(int bit)
{
    buffer >>= 1;
    if (bit)
        buffer |= 0x80;
    bits_to_go -= 1;
    if (bits_to_go == 0)
    {
        putc(buffer, out);
        bits_to_go = 8;
    }
}

void done_outputing_bits(void)
{
    putc(buffer >> bits_to_go, out);
}

void output_bit_plus_follow(int bit)
{
    output_bit(bit);
    while (bits_to_follow > 0)
    {
        output_bit(!bit);
        bits_to_follow--;
    }
}

void start_encoding(void)
{
    low = 0l;
    high = TOP_VALUE;
    bits_to_follow = 0l;
}

void done_encoding(void)
{
    bits_to_follow++;
    if (low < FIRST_QTR)
        output_bit_plus_follow(0);
    else
        output_bit_plus_follow(1);
}

void start_decoding(void)
{
    value = 0l;
    for (int i = 1; i <= BITS_IN_REGISTER; i++)
        value = 2 * value + input_bit();
    low = 0l;
    high = TOP_VALUE;
}

void encode_symbol(int symbol)
{
    long range;

    range = (long)(high - low) + 1;
    high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
    low = low + (range * cum_freq[symbol]) / cum_freq[0];
    for (;;)
    {
        if (high < HALF)
            output_bit_plus_follow(0);
        else if (low >= HALF)
        {
            output_bit_plus_follow(1);
            low -= HALF;
            high -= HALF;
        }
        else if (low >= FIRST_QTR && high < THIRD_QTR)
        {
            bits_to_follow += 1;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        }
        else
            break;
        low = 2 * low;
        high = 2 * high + 1;
    }
}

int decode_symbol(void)
{
    long range;
    int cum, symbol;

    range = (long)(high - low) + 1;

    cum = (int)((((long)(value - low) + 1) * cum_freq[0] - 1) / range);

    for (symbol = 1; cum_freq[symbol] > cum; symbol++);
    high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
    low = low + (range * cum_freq[symbol]) / cum_freq[0];
    for (;;)
    {
        if (high < HALF)
        {
        }
        else if (low >= HALF)
        {
            value -= HALF;
            low -= HALF;
            high -= HALF;
        }
        else if (low >= FIRST_QTR && high < THIRD_QTR)
        {
            value -= FIRST_QTR;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        }
        else
            break;
        low = 2 * low;
        high = 2 * high + 1;
        /*
        int test= input_bit();
        if (test == -1) return 257;
        */
        value = 2 * value + input_bit();
    }
    return symbol;
}

void encode(const char* infile, const char* outfile)
{
    int ch, symbol;
    in = fopen(infile, "r");
    out = fopen(outfile, "w");
    if (in == NULL || out == NULL)
        return;
    start_model();
    start_outputing_bits();
    start_encoding();
    for (;;)
    {
        ch = getc(in);
        if (ch == EOF)
            break;
        symbol = char_to_index[ch];
        encode_symbol(symbol);
        update_model(symbol);
    }
    encode_symbol(EOF_SYMBOL);
    done_encoding();
    done_outputing_bits();
    fclose(in);
    fclose(out);
}

void decode(const char* infile, const char* outfile)
{

    int ch, symbol;
    in = fopen(infile, "r");
    out = fopen(outfile, "w");
    if (in == NULL || out == NULL)
        return;
    start_model();
    start_inputing_bits();
    start_decoding();
    for (;;)
    {
        symbol = decode_symbol();
        if (symbol == EOF_SYMBOL)
            break;
        ch = index_to_char[symbol];
        putc(ch, out);
        update_model(symbol);
    }
    fclose(in);
    fclose(out);
}

//функция для выбора действия пользователя
void Menu()
{
    cout << "Menu: " << endl
        << "(0) exit" << endl
        << "(1) compression" << endl
        << "(2) decompression" << endl
        << "enter: ";
    cin >> _stateMenu;
}

void main(int argc, char** argv)
{
    const char* in = new char[9];
    in = "input.txt";
    const char* out = new char[10];
    out = "output.txt";
    const char* out1 = new char[11];
    out1 = "output1.txt";

    //очищение outputa дбля проверки на точ то было кодирование
    FILE* stream;
    errno_t Output;
    Output = fopen_s(&stream, "output.txt", "w");
    fclose(stream);
    Menu();
    while (_stateMenu != 0)
    {
        ifstream check("output.txt");
        string encode_string;
        getline(check, encode_string);
        check.close();

        switch (_stateMenu)
        {
        case 1:
            encode(in, out);
            Menu();
            break;
        case 2:
            if (encode_string.length() == 0)
            {
                cout << "\tCOMPRESSION FILE DOSENT EXIST\n";
                exit(-1);
            }
            decode(out, out1);
            Menu();
            break;
        default:
            cout << "\tWRONG CHOISE" << endl;
            Menu();
            break;
        }
    }
    delete[] in;
    delete[] out;
    delete[] out1;
}