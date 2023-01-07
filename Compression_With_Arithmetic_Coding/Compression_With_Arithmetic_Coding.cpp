#include <iostream>
#include <errno.h>

using namespace std;

struct sym
{
	unsigned char ch;
	float freq;
	float range;
	char code[255];
	sym* left;
	sym* right;
};

int chh;					//переменная для подсчета информация из строки
int k = 0;					//счётчик количества различных букв, уникальных символов
int kk = 0;					//счетчик количества всех знаков в файле
int kolvo[256] = { 0 };		//инициализируем массив количества уникальных символов
sym simbols[256] = { 0 };	//инициализируем массив записей
sym* psym[256];				//инициализируем массив указателей на записи
float summ_of_all_freq = 0;	//сумма частот встречаемости

//compression
/*
int l_0 = 0;
int h_0 = 65535;
int i = 0;
float delitel = b[c_last];
float first_qtr = (h_0 + 1) / 4;
float half = first_qtr * 2;
float third_qtr = first_qtr * 3;
int bits_to_follow = 0;
while (not DataFile.EOF())
{
	char c = DataFile.ReadSymbol();
	int j = IndexForSymbol(c);
	i++;
	l[i] = l[i - 1] + b[j - 1] * (h[i - 1] - l[i - 1] + 1) / delitel;
	h[i] = l[i - 1] + b[j] * (h[i - 1] - l[i - 1] + 1) / delitel - 1;
	for (;;)
	{
		if (h[i] < half)
			BitsPlusFollow(0);
		else if (l[i] >= half)
		{
			BitsPlusFollow(1);
			l[i] -= half;
			h[i] -= half;
		}
		else if ((l[i] >= first_qtr) && (h[i] < third_qtr))
		{
			bits_to_follow++;
			l[i] -= first_qtr;
			h[i] -= first_qtr;
		}
		else break;
		l[i] += l[i];
		h[i] += h[i] + 1;
	}
}

void BitsPlusFollow(int bit)
{
	CompressedFile.WriteBit(bit);
	for (; bits_to_follow > 0; bits_to_follow--)
	{
		CompressedFile.WriteBit(!bit);
	}
}
*/
//decompression
/*
int l[0] = 0;
int h[0] = 65535;
float delitel = b[c_last];
float first_qtr = (h[0] + 1) / 4;
float half = first_qtr * 2;
float third_qtr = first_qtr * 3;
float value = CompressedFile.Read16bit();
for (i = 1; i < compressedFile.DataLength(); i++)
{
	freq = ((value - l[i - 1] + 1) * delitel - 1) / (h[i - 1] - l[i - 1] + 1);
	for (j = 1; b[j] <= freq; j++);
	l[i] = l[i - 1] + b[j - 1] * (h[i - 1] - l[i - 1] + 1) / delitel;
	h[i] = l[i - 1] + b[j] * (h[i - 1] - l[i - 1] + 1) / delitel - 1;
	for (;;)
	{
		if (h[i] < half)
			;
		else if (l[i] >= half)
		{
			l[i] -= half;
			h[i] -= half;
			value -= half;
		}
		else if ((l[i] >= first_qtr) && (h[i] < third_qtr))
		{
			l[i] -= first_qtr;
			h[i] -= first_qtr;
			value -= first_qtr;
		}
		else break;
		l[i] += l[i];
		h[i] += h[i] + 1;
		value += value + CompressedFile.ReadBit();
	}
	DataFile.WriteSymbol(c);
};
*/

void Statistics(char* String)
{
	//посимвольно считываем строку и составляем таблицу встречаемости
	for (int i = 0; i < strlen(String); i++)
	{
		//цикл для подсчета информация из строки
		chh = String[i];
		for (int j = 0; j < 256; j++)
		{
			//если символ нашли в массиве записей символов, то в массиве количества уникальных символов увеличиваем количество
			//и увеличиваем общее количество символов
			if (chh == simbols[j].ch)
			{
				kolvo[j]++;
				kk++;
				break;
			}
			//если не нашли в массиве записей символов, то знаносим этот символ 
			//в массиве количества уникальных символов ставим единицу
			//и увеличиваем общее количество символов и уникальных символов
			if (simbols[j].ch == 0)
			{
				simbols[j].ch = (unsigned char)chh;
				kolvo[j] = 1;
				k++;
				kk++;
				break;
			}
		}
	}
	// расчет частоты встречаемости 
	
	for (int i = 0; i < k; i++)
	{
		simbols[i].freq = (float)kolvo[i] / kk;
		
	}
	// в массив указателей заносим адреса записей
	for (int i = 0; i < k; i++)
	{
		psym[i] = &simbols[i];
	}
	//сортировка по убыванию
	sym tempp;
	for (int i = 1; i < k; i++)
	{
		for (int j = 0; j < k - 1; j++)
		{
			if (simbols[j].freq < simbols[j + 1].freq)
			{
				tempp = simbols[j];
				simbols[j] = simbols[j + 1];
				simbols[j + 1] = tempp;
			}
		}
	}
	//печатаем статистику и диапозона
	//по итогу сумма частот должна дать 1
	simbols[-1].range = 0;
	for (int i = 0; i < k; i++)
	{
		summ_of_all_freq += simbols[i].freq;
		simbols[i].range = simbols[i-1].range + simbols[i].freq;
		printf("Character = %d\tFrequancy = %f\tRange = [%f;%f)\tSymbol = %c\t\n", simbols[i].ch, simbols[i].freq, simbols[i - 1].range, simbols[i].range, psym[i]->ch);
	}
	printf("\nKolovo simvolov : %d\nSumm of all Frequancy : %f\n", kk, summ_of_all_freq);
}

float Compression(char* String, sym* simbols)
{
	float* l = new float[100];
	float* h = new float[100];
	//интервалы i кодироуемого сивола потока
	l[-1] = 0;	
	h[-1] = 1;
	for (int i = 0; i < strlen(String); i++)
	{
		chh = String[i];
		for (int j = 0; j < k; j++)
			if (chh == simbols[j].ch)
			{
				l[i] = l[i - 1] + simbols[j - 1].range * (h[i - 1] - l[i - 1]);
				h[i] = l[i - 1] + simbols[j].range * (h[i - 1] - l[i - 1]);
			}
	}
	cout <<"range govna: [" << l[strlen(String) - 1]<<";" << h[strlen(String) - 1]<<")\n";
	return l[strlen(String) - 1];
}

void Decompression(float encoding_message_number, sym* simbols, char* ReducedString,int length_file)
{
	float* l = new float[100];
	float* h = new float[100];
	//интервалы i кодироуемого сивола потока
	l[-1] = 0;
	h[-1] = 1;
	int i = 0;
	for (; i < length_file; i++)
	{
		int j = 0;
		for (; j < k; j++)
		{
			l[i] = l[i - 1] + simbols[j - 1].range * (h[i - 1] - l[i - 1]);
			h[i] = l[i - 1] + simbols[j].range * (h[i - 1] - l[i - 1]);
			if ((l[i] <= encoding_message_number) && (encoding_message_number < h[i]))
				break;
		}
		ReducedString[i] = simbols[j].ch;
	}
	ReducedString[i] = '\0';
}

int main()
{
    std::cout << "Hello World!\n";

	char* String = new char[1000];
	char* ReducedString = new char[1000];
	float encoding_message_number = 0;

	//считывание файла
	FILE* stream;
	errno_t Input = fopen_s(&stream, "Input.txt", "r");
	char x;
	int i = 0;
	while ((feof(stream) == 0))
	{
		fscanf_s(stream, "%c", &x);
		String[i] = x;
		i++;
	}
	String[i - 1] = '\0';
	fclose(stream);
	////////////////////////////////////////
	Statistics(String);						//вызов функции определения частоты символов в строке
	encoding_message_number=Compression(String, simbols);

	//вывод в файл
	errno_t Output = fopen_s(&stream, "Output.txt", "w");
	fprintf(stream, "number encoding message: %f\n", encoding_message_number);
	//fclose(stream);

	Decompression(encoding_message_number, simbols, ReducedString, strlen(String));
	fprintf(stream, "Decompression Code:\n%s\n", ReducedString);
	fclose(stream);

	delete[] String;
	delete[] ReducedString;
	return 0;
}