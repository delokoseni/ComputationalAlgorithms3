#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "exprtk.hpp"

int GetAmountOfValues(std::string FileName);
std::vector<std::vector<double>> GetFunctionValueTable(std::ifstream& File, int AmountOfValues);
double LagrangeInterpolation(std::vector<std::vector<double>> Table, int AmountOfValues, double PointX);
void TableOutput(std::vector<std::vector<double>> Table, int AmountOfValues, std::ostream& Stream);
std::vector<std::vector<double>> InputFunction(std::vector<double> Arguments, int AmountOfValues, const std::string& expression_str);
std::vector<double> GetArgumentValuesArray(std::ifstream& File, int AmountOfValues);
void OutputLagrangeInterpolation(std::vector<std::vector<double>> Table, int AmountOfValues);

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::cout << "Выберите режим работы программы: " << std::endl;
    std::cout << "1) По заданной таблице значений функции определять приближенное значение функции в некоторой точке." << std::endl;
    std::cout << "2) По заданной аналитически функции y = f (x) и массиву значений аргумента вычислить таблицу значений функции." << std::endl;
    char SelectedMode;
    bool SelectFlag = false;
    std::string AmountOfValuesFileName = "AmountOfValues.txt";
    int AmountOfValues = GetAmountOfValues(AmountOfValuesFileName);;
    std::string FunctionValueTableFileName = "FunctionValueTable.txt";
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues));
    std::string Function; //Строка для ввода функции (Б)
    std::string ArgumentValuesArrayFileName = "ArgumentValuesArray.txt";
    std::vector<double> ArgumentValuesArray(AmountOfValues);
    std::ifstream FunctionValueTableFile;
    FunctionValueTableFile.open(FunctionValueTableFileName);
    std::ifstream ArgumentValuesArrayFile;
    ArgumentValuesArrayFile.open(ArgumentValuesArrayFileName);
    std::cout << "Режим: ";
    std::cin >> SelectedMode;
    switch (SelectedMode)
    {
    case '1':
        Table = GetFunctionValueTable(FunctionValueTableFile, AmountOfValues);
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, AmountOfValues, std::cout);
        std::cout << "Введите точку для определения приближенного значения функции: ";
        double PointX;
        std::cin >> PointX;
        std::cout << LagrangeInterpolation(Table, AmountOfValues, PointX);
        break;
    case '2':
        std::cout << "Пример функции: sqrt(abs(x))" << std::endl;
        std::cout << "Введите функцию в аналитическом виде: y=";
        std::cin >> Function;
        ArgumentValuesArray = GetArgumentValuesArray(ArgumentValuesArrayFile, AmountOfValues);
        Table = InputFunction(ArgumentValuesArray, AmountOfValues, Function);
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, AmountOfValues, std::cout);
        OutputLagrangeInterpolation(Table, AmountOfValues);
        break;
    default:
        std::cout << "Режим не был выбран." << std::endl;
    }
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
//Ввод таблицы значений аргументов и функции
std::vector<std::vector<double>> GetFunctionValueTable(std::ifstream &File, int AmountOfValues)
{
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues));
    for (int i = 0; i < 2; i++)
        Table[i] = GetArgumentValuesArray(File, AmountOfValues);
    return Table;
}
//Вычисление значения функции когда аргумент равен PointX при помощи полинома
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
//Вывод таблицы значений функции и аргумента
void TableOutput(std::vector<std::vector<double>> Table, int AmountOfValues, std::ostream& Stream)
{
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            std::cout << "x\t\t";
        else
            std::cout << "y\t\t";
        for (int j = 0; j < AmountOfValues; j++)
        {
            Stream << Table[i][j] << "\t\t";
        }
        Stream << std::endl;
    }
}

std::vector<std::vector<double>> InputFunction(std::vector<double> Arguments, int AmountOfValues, const std::string& ExpressionString)
{
    std::vector < exprtk::symbol_table<double>> SymbolTable(AmountOfValues);
    for (int i = 0; i < AmountOfValues; i++) {
        SymbolTable[i].add_variable("x", Arguments[i]);
        SymbolTable[i].add_function("abs", abs);
        SymbolTable[i].add_function("sqrt", sqrt);
        SymbolTable[i].add_function("exp", exp);
    }
    std::vector<exprtk::expression<double>> Expression(AmountOfValues);
    for (int i = 0; i < AmountOfValues; i++)
        Expression[i].register_symbol_table(SymbolTable[i]);

    exprtk::parser<double> Parser;
    for (int i = 0; i < AmountOfValues; i++)
        if (!Parser.compile(ExpressionString, Expression[i]))
            std::cout << "Ошибка в выражении!\n";

    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues));
    for (int i = 0; i < AmountOfValues; i++)
    {
        Table[0][i] = Arguments[i];
        Table[1][i] = Expression[i].value();
    }

    return Table;
}
//Ввод массива значений аргумента
std::vector<double> GetArgumentValuesArray(std::ifstream &File, int AmountOfValues)
{
    std::vector<double> Arguments(AmountOfValues);
    try
    {
        for (int i = 0; i < AmountOfValues; i++)
            File >> Arguments[i];
    }
    catch (const std::exception& Exception)
    {
        std::cout << Exception.what() << std::endl;
    }
    return Arguments;
}
//Вывод интерполяционного полинома Лагранжа (можно добавить вывод в файл)
void OutputLagrangeInterpolation(std::vector<std::vector<double>> Table, int AmountOfValues)
{
    std::cout << "y=";
    for (int i = 0; i < AmountOfValues; i++)
    {
        std::cout << Table[1][i];
        for (int j = 0; j < AmountOfValues; j++) {
            if (i != j)
                std::cout << "*(x-" << Table[0][j] << ")/(" << Table[0][i] << "-" << Table[0][j] << ")";
            if ((j + 1) % AmountOfValues == 0 && (i + 1) != AmountOfValues)
                std::cout << "+";
        }
    }
}