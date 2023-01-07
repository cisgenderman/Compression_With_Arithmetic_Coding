#include <iostream>

int main()
{
    std::cout << "Hello World!\n";

	char* String = new char[1000];
	float encoding_message_number;

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
	fprintf(stream, "number encoding message: %d\n", encoding_message_number);
	fclose(stream);
	delete[] String;
	return 0;
}