#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "exprtk.hpp"

int GetAmountOfValues(std::string FileName);
std::vector<double> GetArgumentValuesArray(std::ifstream& File, int AmountOfValues);
std::vector<std::vector<double>> GetFunctionValueTable(std::ifstream& File, int AmountOfValues);
void TableOutput(std::vector<std::vector<double>> Table, std::ostream& Stream);
double LagrangeInterpolation(std::vector<std::vector<double>> Table, double PointX);
void OutputLagrangeInterpolation(std::vector<std::vector<double>> Table);
std::vector<std::vector<double>> InputFunction(std::vector<double> Arguments, std::string expression_str);
std::vector<exprtk::symbol_table<double>> CreateSymbolTable(std::vector<double> Arguments);
std::vector<exprtk::expression<double>> MakeExpressionTable(std::vector<exprtk::symbol_table<double>> SymbolTable,
                                                            std::string ExpressionString, int Size);

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
        TableOutput(Table, std::cout);
        std::cout << "Введите точку для определения приближенного значения функции: ";
        double PointX;
        std::cin >> PointX;
        std::cout << LagrangeInterpolation(Table, PointX);
        break;
    case '2':
        std::cout << "Пример функции: sqrt(abs(x))" << std::endl;
        std::cout << "Введите функцию в аналитическом виде: y=";
        std::cin >> Function;
        ArgumentValuesArray = GetArgumentValuesArray(ArgumentValuesArrayFile, AmountOfValues);
        Table = InputFunction(ArgumentValuesArray, Function);
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, std::cout);
        OutputLagrangeInterpolation(Table);
        break;
    default:
        std::cout << "Режим не был выбран." << std::endl;
    }
    return 0;
}
//Получить количество аргументов из файла
int GetAmountOfValues(std::string FileName)
{
    std::ifstream AmountOfValuesFile;
    int AmountOfValues = 0;
    try
    {
        AmountOfValuesFile.open(FileName);
        AmountOfValuesFile >> AmountOfValues;
        AmountOfValuesFile.close();
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
double LagrangeInterpolation(std::vector<std::vector<double>> Table, double PointX)
{
    double Polynomial = 0.0;
    double X = 1.0;
    for (int i = 0; i < Table[0].size(); i++)
    {
        for (int j = 0; j < Table[0].size(); j++)
            if (i != j)
                X *= (PointX - Table[0][j]) / (Table[0][i] - Table[0][j]);
        Polynomial += Table[1][i] * X;
        X = 1.0;
    }
    return Polynomial;
}
//Вывод таблицы значений функции и аргумента
void TableOutput(std::vector<std::vector<double>> Table, std::ostream& Stream)
{
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            std::cout << "x\t\t";
        else
            std::cout << "y\t\t";
        for (int j = 0; j < Table[i].size(); j++)
        {
            Stream << Table[i][j] << "\t\t";
        }
        Stream << std::endl;
    }
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
void OutputLagrangeInterpolation(std::vector<std::vector<double>> Table)
{
    std::cout << "y=";
    for (int i = 0; i < Table[0].size(); i++)
    {
        std::cout << Table[1][i];
        for (int j = 0; j < Table[0].size(); j++) {
            if (i != j)
                std::cout << "*(x-" << Table[0][j] << ")/(" << Table[0][i] << "-" << Table[0][j] << ")";
            if ((j + 1) % Table[0].size() == 0 && (i + 1) != Table[0].size())
                std::cout << "+";
        }
    }
}

std::vector<std::vector<double>> InputFunction(std::vector<double> Arguments, std::string ExpressionString)
{
    std::vector<exprtk::symbol_table<double>> SymbolTable(Arguments.size());
    for (int i = 0; i < Arguments.size(); i++) {
        SymbolTable[i].add_variable("x", Arguments[i]);
        SymbolTable[i].add_function("abs", abs);
        SymbolTable[i].add_function("sqrt", sqrt);
        SymbolTable[i].add_function("exp", exp);
    }
    //Создает выражения, вычисляются значения функции
    std::vector<exprtk::expression<double>> Expression = MakeExpressionTable(SymbolTable, ExpressionString, Arguments.size());
    std::vector<std::vector<double>> Table(2, std::vector<double>(Arguments.size()));
    for (int i = 0; i < Arguments.size(); i++) //Заполняет таблицу аргументами и значениями функции
    {
        Table[0][i] = Arguments[i];
        Table[1][i] = Expression[i].value();
    }
    return Table;
}
//Создает таблциу переменных, добавляет функции
std::vector<exprtk::symbol_table<double>> CreateSymbolTable(std::vector<double> Arguments)
{
    std::vector<exprtk::symbol_table<double>> SymbolTable(Arguments.size());
    for (int i = 0; i < Arguments.size(); i++) {
        SymbolTable[i].add_variable("x", Arguments[i]);
        SymbolTable[i].add_function("abs", abs);
        SymbolTable[i].add_function("sqrt", sqrt);
        SymbolTable[i].add_function("exp", exp);
    }
    return SymbolTable;
}
//Создает выражения, вычисляются значения функции
std::vector<exprtk::expression<double>> MakeExpressionTable(std::vector<exprtk::symbol_table<double>> SymbolTable,
                                                            std::string ExpressionString, int Size)
{
    std::vector<exprtk::expression<double>> Expression(Size);
    for (int i = 0; i < Size; i++)
        Expression[i].register_symbol_table(SymbolTable[i]);

    exprtk::parser<double> Parser;
    for (int i = 0; i < Size; i++)
        if (!Parser.compile(ExpressionString, Expression[i]))
            std::cout << "Ошибка в выражении!\n";
    return Expression;
}