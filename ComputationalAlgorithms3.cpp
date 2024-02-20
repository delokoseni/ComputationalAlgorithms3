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
void OutputLagrangePolynomial(std::vector<std::vector<double>> Table, std::ostream& Stream);
std::vector<std::vector<double>> GetTableByFunction(std::vector<double> Arguments, std::string expression_str);
std::vector<exprtk::expression<double>> MakeExpressionTable(std::vector<exprtk::symbol_table<double>> SymbolTable,
                                                            std::string ExpressionString, int Size);

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //Получение количества значений
    std::string AmountOfValuesFileName = "AmountOfValues.txt";
    int AmountOfValues = GetAmountOfValues(AmountOfValuesFileName);
    //Получение таблицы значений (для режима 1)
    std::string FunctionValueTableFileName = "FunctionValueTable.txt";
    std::ifstream FunctionValueTableFile;
    FunctionValueTableFile.open(FunctionValueTableFileName);
    //Получение массива аргументов (для режима 2)
    std::string ArgumentValuesArrayFileName = "ArgumentValuesArray.txt";
    std::ifstream ArgumentValuesArrayFile;
    ArgumentValuesArrayFile.open(ArgumentValuesArrayFileName);

    std::cout << "Выберите режим работы программы: " << std::endl;
    std::cout << "1) По заданной таблице значений функции определять приближенное значение функции в некоторой точке." << std::endl;
    std::cout << "2) По заданной аналитически функции и массиву значений аргумента вычислить таблицу значений функции." << std::endl;
    char SelectedMode;
    std::cout << "Режим: ";
    std::cin >> SelectedMode;

    std::string Function; //Строка для ввода функции (для режима 2)
    std::vector<std::vector<double>> Table(2, std::vector<double>(AmountOfValues)); //Таблица значений, используюется в двух режимах
    std::vector<double> ArgumentValuesArray(AmountOfValues); //Массив значений аргументов (для режима 2)
    switch (SelectedMode)
    {
    case '1':
        Table = GetFunctionValueTable(FunctionValueTableFile, AmountOfValues); //Заполнение таблицы из файла
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, std::cout);
        std::cout << "Введите точку для определения приближенного значения функции: ";
        double PointX;
        std::cin >> PointX;
        std::cout << "Результат: " << LagrangeInterpolation(Table, PointX) << std::endl;
        OutputLagrangePolynomial(Table, std::cout);
        break;
    case '2':
        std::cout << "Пример функции: sqrt(abs(x))" << std::endl;
        std::cout << "Введите функцию в аналитическом виде: y=";
        std::cin >> Function;
        ArgumentValuesArray = GetArgumentValuesArray(ArgumentValuesArrayFile, AmountOfValues); //Получение массива аргументов
        Table = GetTableByFunction(ArgumentValuesArray, Function);
        std::cout << "Таблица значений функции: " << std::endl;
        TableOutput(Table, std::cout);
        OutputLagrangePolynomial(Table, std::cout);
        break;
    default:
        std::cout << "Режим не был выбран." << std::endl;
    }
    FunctionValueTableFile.close();
    ArgumentValuesArrayFile.close();
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
//Ввод массива значений аргумента
std::vector<double> GetArgumentValuesArray(std::ifstream& File, int AmountOfValues)
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
//Вывод таблицы значений функции и аргумента
void TableOutput(std::vector<std::vector<double>> Table, std::ostream& Stream)
{
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            Stream << "x\t\t";
        else
            Stream << "y\t\t";
        for (int j = 0; j < Table[i].size(); j++)
        {
            Stream << Table[i][j] << "\t\t";
        }
        Stream << std::endl << std::endl;
    }
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
                X *= (PointX - Table[0][j]) / (Table[0][i] - Table[0][j]); //(x-x[j])/(x[i]-x[j])
        Polynomial += Table[1][i] * X;
        X = 1.0;
    }
    return Polynomial;
}
//Вывод интерполяционного полинома Лагранжа (можно добавить вывод в файл)
void OutputLagrangePolynomial(std::vector<std::vector<double>> Table, std::ostream& Stream)
{
    Stream << "y=";
    for (int i = 0; i < Table[0].size(); i++)
    {
        Stream << Table[1][i];
        for (int j = 0; j < Table[0].size(); j++) {
            if (i != j)
                if(Table[0][j] < 0)
                    Stream << "*(x-(" << Table[0][j] << "))/(" << Table[0][i] << "-(" << Table[0][j] << "))";
                else
                    Stream << "*(x-" << Table[0][j] << ")/(" << Table[0][i] << "-" << Table[0][j] << ")";
            if ((j + 1) % Table[0].size() == 0 && (i + 1) != Table[0].size())
                Stream << "+";
        }
    }
}
//Получает таблицу по введенной в аналитическом виде функции
std::vector<std::vector<double>> GetTableByFunction(std::vector<double> Arguments, std::string ExpressionString)
{
    std::vector<exprtk::symbol_table<double>> SymbolTable(Arguments.size());
    for (int i = 0; i < Arguments.size(); i++) { //Заполнение таблицы символов
        SymbolTable[i].add_variable("x", Arguments[i]);
        SymbolTable[i].add_function("abs", abs);
        SymbolTable[i].add_function("sqrt", sqrt);
        SymbolTable[i].add_function("exp", exp);
    }
    //Создаются выражения, вычисляются значения функции
    std::vector<exprtk::expression<double>> Expression = MakeExpressionTable(SymbolTable, ExpressionString, Arguments.size());
    std::vector<std::vector<double>> Table(2, std::vector<double>(Arguments.size()));
    // Заполняет таблицу аргументами и значениями функции
    for (int i = 0; i < Arguments.size(); i++) 
    {
        Table[0][i] = Arguments[i];
        Table[1][i] = Expression[i].value();
    }
    return Table;
}
//Создает выражения, вычисляются значения функции
std::vector<exprtk::expression<double>> MakeExpressionTable(std::vector<exprtk::symbol_table<double>> SymbolTable,
                                                            std::string ExpressionString, int Size)
{
    //Создается вектор выражений заданного размера
    std::vector<exprtk::expression<double>> Expression(Size);
    for (int i = 0; i < Size; i++)
        Expression[i].register_symbol_table(SymbolTable[i]);
    //Компилируется каждое выражение с помощью парсера
    exprtk::parser<double> Parser;
    for (int i = 0; i < Size; i++)
        if (!Parser.compile(ExpressionString, Expression[i]))
            std::cout << "Ошибка в выражении!\n";
    return Expression; //Возвращается вектор скомпилированных выражений
}