#include <iostream>

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

int main()
{
    std::cout << "Hello World!\n";

	char* String = new char[1000];
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

	///////////////////////////////////
	
	//вывод в файл
	errno_t Output = fopen_s(&stream, "Output.txt", "w");
	fprintf(stream, "number encoding message: %f\n", encoding_message_number);
	fclose(stream);
	delete[] String;
	return 0;
}