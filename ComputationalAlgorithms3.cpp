#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>

int GetAmountOfValues(std::string FileName);
std::vector<std::vector<double>> GetFunctionValueTable(std::string FileName, int AmountOfValues);
double LagrangeInterpolation(std::vector<std::vector<double>> Table, int AmountOfValues, double PointX);
void TableOutput(std::vector<std::vector<double>> Table, int AmountOfValues, std::ostream& Stream);

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::cout << "Выберите режим работы программы: " << std::endl;
    std::cout << "А) По заданной таблице значений функции определять приближенное значение функции в некоторой точке." << std::endl;
    std::cout << "Б) По заданной аналитически функции y = f (x) и массиву значений аргумента вычислить таблицу значений функции." << std::endl;
    char SelectedMode;
    bool SelectFlag = false;
    std::string AmountOfValuesFileName = "AmountOfValues.txt";
    int AmountOfValues = GetAmountOfValues(AmountOfValuesFileName);;
    std::string FunctionValueTableFileName = "FunctionValueTable.txt";
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues));
    do 
    {
        std::cout << "Режим: ";
        std::cin >> SelectedMode;
        switch (SelectedMode)
        {
        case 'А':
            Table = GetFunctionValueTable(FunctionValueTableFileName, AmountOfValues);
            std::cout << "Таблица значений функции: " << std::endl;
            TableOutput(Table, AmountOfValues, std::cout);
            std::cout << "Введите точку для определения приближенного значения функции: ";
            double PointX;
            std::cin >> PointX;
            std::cout << LagrangeInterpolation(Table, AmountOfValues, PointX);
            SelectFlag = true;
            break;
        case 'Б':
            SelectFlag = true;
            break;
        default:
            std::cout << "Режим не был выбран." << std::endl;
        }

    } while (!SelectFlag);
    return 0;
}

int GetAmountOfValues(std::string FileName)
{
    std::ifstream AmountOfValuesFile;
    int AmountOfValues = 0;
    try
    {
        AmountOfValuesFile.open(FileName);
        AmountOfValuesFile >> AmountOfValues;
    }
    catch (const std::exception& Exception)
    {
        std::cout << Exception.what() << std::endl;
    }
    return AmountOfValues;
}

std::vector<std::vector<double>> GetFunctionValueTable(std::string FileName, int AmountOfValues)
{
    std::ifstream FunctionValueTableFile;
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues));
    try
    {
        FunctionValueTableFile.open(FileName);
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < AmountOfValues; j++)
                FunctionValueTableFile >> Table[i][j];
    }
    catch (const std::exception& Exception)
    {
        std::cout << Exception.what() << std::endl;
    }
    TableOutput(Table, AmountOfValues, std::cout);
    return Table;
}

double LagrangeInterpolation(std::vector<std::vector<double>> Table, int AmountOfValues, double PointX)
{
    double Polynomial = 0.0;
    double X = 1.0;
    for (int i = 0; i < AmountOfValues; i++)
    {
        for (int j = 0; j < AmountOfValues; j++)
            if (i != j)
                X *= (PointX - Table[0][j]) / (Table[0][i] - Table[0][j]);
        Polynomial += Table[1][i] * X;
        X = 1.0;
    }
    return Polynomial;
}

void TableOutput(std::vector<std::vector<double>> Table, int AmountOfValues, std::ostream& Stream)
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < AmountOfValues; j++)
        {
            Stream << Table[i][j] << "\t\t";
        }
        Stream << std::endl;
    }
}