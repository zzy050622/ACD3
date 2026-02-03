#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

// 结构体：适配「时间、名字、序列号」格式，保留原始行号
// Struct: Adapt to "time, name, serial number" format, retain original line number
struct Record {
    string time;          // 第1字段：时间（如 06.10.2016）| Field 1: Time (e.g., 06.10.2016)
    string name;          // 第2字段：名字（如 Lopez, Anthony，含空格）| Field 2: Name (e.g., Lopez, Anthony, with space)
    int serialNum;        // 第3字段：序列号（搜索关键字，数值类型）| Field 3: Serial number 
    int originalLine;     // 原始行号（从1开始，对应文件中的行序）| Original line number (starts from 1, corresponds to line order in file)
};

// 线性搜索：遍历所有元素，匹配关键字并记录行号，返回搜索步骤数
// Linear Search (logic unchanged): Traverse all elements, match key and record line numbers, return number of search steps
int linearSearch(const vector<Record>& data, int key, vector<int>& matchLines) {
    int steps = 0;
    matchLines.clear();  // 清空结果容器 | Clear result container

    for (size_t i = 0; i < data.size(); ++i) {
        steps++;  // 每遍历1个元素，计1步 | Count 1 step for each element traversed
        if (data[i].serialNum == key) {
            matchLines.push_back(data[i].originalLine);  // 记录匹配的原始行号 | Record original line number of match
        }
    }

    return steps;  // 返回总步骤数 | Return total number of steps
}

// 均匀二分搜索：依赖有序数据，通过二分缩小范围找到匹配项，再扩展收集所有重复匹配
// Uniform Binary Search (logic unchanged): Relies on sorted data, narrows range via binary division to find matches, then expands to collect all duplicate matches
int uniformBinarySearch(const vector<Record>& sortedData, int key, vector<int>& matchLines) {
    int steps = 0;
    matchLines.clear();  // 清空结果容器 | Clear result container
    int left = 0;        // 搜索范围左边界 | Left boundary of search range
    int right = sortedData.size() - 1;  // 搜索范围右边界 | Right boundary of search range
    int foundPos = -1;   // 找到的匹配项位置（初始为-1表示未找到）| Position of found match (initial -1 means not found)

    // 二分查找核心逻辑：不断缩小搜索范围，直到找到匹配项或范围无效
    // Core binary search logic: Continuously narrow search range until match is found or range is invalid
    while (left <= right) {
        steps++;  // 每一次二分判断，计1步 | Count 1 step for each binary judgment
        int mid = left + (right - left) / 2;  // 计算中间位置（避免整数溢出）| Calculate middle position (avoid integer overflow)

        if (sortedData[mid].serialNum == key) {
            foundPos = mid;  // 记录匹配项位置 | Record position of match
            break;           // 退出二分循环，后续收集所有匹配项 | Exit binary loop, collect all matches later
        }
        else if (sortedData[mid].serialNum < key) {
            left = mid + 1;  // 目标在右半部分，更新左边界 | Target is in right half, update left boundary
        }
        else {
            right = mid - 1; // 目标在左半部分，更新右边界 | Target is in left half, update right boundary
        }
    }

    // 收集所有匹配项（向左+向右扩展）：有序数据中相同关键字连续排列
    // Collect all matches (expand left + right): Same keys are arranged consecutively in sorted data
    if (foundPos != -1) {
        matchLines.push_back(sortedData[foundPos].originalLine);  // 先记录当前找到的位置 | First record current found position

        // 向左扩展：查找左侧所有相同序列号的元素 | Expand left: Find all elements with same serial number on the left
        int i = foundPos - 1;
        while (i >= 0 && sortedData[i].serialNum == key) {
            steps++;
            matchLines.push_back(sortedData[i].originalLine);
            i--;
        }

        // 向右扩展：查找右侧所有相同序列号的元素 | Expand right: Find all elements with same serial number on the right
        i = foundPos + 1;
        while (i < sortedData.size() && sortedData[i].serialNum == key) {
            steps++;
            matchLines.push_back(sortedData[i].originalLine);
            i++;
        }
    }

    return steps;  // 返回总步骤数 | Return total number of steps
}

// 功能：按「制表符分隔」读取 data.txt，处理名字含空格的情况
// Function: Read data.txt separated by tabs, handle names with spaces
bool readFileData(const string& filename, int n, vector<Record>& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Failed to open file " << filename << endl;  // 错误：无法打开文件 | Error: Failed to open file
        return false;
    }

    data.clear();
    string line;          // 存储每行完整数据 | Store complete data of each line
    int lineCount = 0;    // 记录已读取的行数 | Record number of lines read

    // 逐行读取文件（直到读取 n 行或文件结束）| Read file line by line (until n lines are read or file ends)
    while (getline(file, line) && lineCount < n) {
        lineCount++;
        stringstream ss(line);  // 用 stringstream 拆分当前行 | Use stringstream to split current line
        string timeStr, nameStr, serialStr;

        // 按制表符（\t）拆分字段：第1个制表符前是时间，第2个制表符前是名字，之后是序列号
        // Split fields by tab (\t): Time before first tab, name before second tab, serial number after second tab
        if (!getline(ss, timeStr, '\t')) {
            cerr << "Error: Line " << lineCount << " format error (missing time field)" << endl;  // 错误：缺少时间字段 | Error: Missing time field
            file.close();
            return false;
        }
        if (!getline(ss, nameStr, '\t')) {
            cerr << "Error: Line " << lineCount << " format error (missing name field)" << endl;  // 错误：缺少名字字段 | Error: Missing name field
            file.close();
            return false;
        }
        if (!getline(ss, serialStr)) {
            cerr << "Error: Line " << lineCount << " format error (missing serial number field)" << endl;  // 错误：缺少序列号字段 | Error: Missing serial number field
            file.close();
            return false;
        }

        // 将序列号字符串转换为整数（处理格式错误）| Convert serial number string to integer (handle format errors)
        int serialNum;
        try {
            serialNum = stoi(serialStr);  // 字符串转整数 | Convert string to integer
        }
        catch (const invalid_argument& e) {
            cerr << "Error: Line " << lineCount << " serial number " << serialStr << " is not a valid integer" << endl;  // 错误：序列号不是有效整数 | Error: Serial number is not a valid integer
            file.close();
            return false;
        }
        catch (const out_of_range& e) {
            cerr << "Error: Line " << lineCount << " serial number " << serialStr << " is out of integer range" << endl;  // 错误：序列号超出整数范围 | Error: Serial number is out of integer range
            file.close();
            return false;
        }

        // 存入结构体，记录原始行号 | Store in struct and record original line number
        Record record;
        record.time = timeStr;
        record.name = nameStr;
        record.serialNum = serialNum;
        record.originalLine = lineCount;  // 原始行号 = 已读取行数 | Original line number = number of lines read
        data.push_back(record);
    }

    // 检查是否读取到足够的行数 | Check if enough lines are read
    if (lineCount < n) {
        cerr << "Error: File contains only " << lineCount << " lines, less than " << n << " required lines" << endl;  // 错误：文件行数不足 | Error: Insufficient lines in file
        file.close();
        return false;
    }

    file.close();
    return true;
}


int main() {
    // 1. 输入参数：从控制台读取 | Input parameters: Read from console
    string filename = "data.txt";  // 数据文件路径 | Data file path (can be modified to console input if needed)
    int n;                         // 处理行数（10 ≤ n ≤ 1000000）| Number of lines to process (10 ≤ n ≤ 1000000)
    int searchKey;                 // 搜索关键字（序列号）| Search key (serial number)

    // 读取并校验处理行数n | Read and validate number of lines to process (n)
    cout << "Please enter the number of lines to process (10 ≤ n ≤ 1000000): ";
    while (!(cin >> n) || n < 10 || n > 1000000) {
        cin.clear();                // 清除输入错误状态 | Clear input error state
        cin.ignore(1024, '\n');     // 忽略缓冲区中的无效输入 | Ignore invalid input in buffer
        cerr << "Invalid input! n must be an integer between 10 and 1000000. Please try again: ";
    }

    // 读取搜索关键字searchKey | Read search key (serial number)
    cout << "Please enter the search key (serial number, integer): ";
    while (!(cin >> searchKey)) {
        cin.clear();
        cin.ignore(1024, '\n');
        cerr << "Invalid input! Search key must be an integer. Please try again: ";
    }

    // 2. 读取文件数据 | Read file data
    vector<Record> data;
    if (!readFileData(filename, n, data)) {
        return 1;  // 读取失败，退出程序 | Exit program if reading fails
    }

    // 3. 线性搜索 | Linear search
    vector<int> linearMatchLines;
    int linearSteps = linearSearch(data, searchKey, linearMatchLines);

    // 4. 均匀二分搜索（先按序列号排序，排序不计入搜索步骤）| Uniform binary search (sort by serial number first, sorting not counted in search steps)
    vector<Record> sortedData = data;
    sort(sortedData.begin(), sortedData.end(), [](const Record& a, const Record& b) {
        return a.serialNum < b.serialNum;  // 按序列号升序排序 | Sort by serial number in ascending order
        });
    vector<int> binaryMatchLines;
    int binarySteps = uniformBinarySearch(sortedData, searchKey, binaryMatchLines);

    // 5. 输出结果 | Output results
    cout << "Search Key (Serial Number): " << searchKey << endl;
    cout << "Number of Lines Processed: " << n << endl;

    cout << "\n--- Linear Search Results ---" << endl;
    cout << "Number of Search Steps: " << linearSteps << endl;
    if (linearMatchLines.empty()) {
        cout << "No lines with matching serial number found" << endl;
    }
    else {
        cout << "Matching Original Line Numbers (Total " << linearMatchLines.size() << " lines): ";
        for (int line : linearMatchLines) {
            cout << line << " ";
        }
        cout << endl;
    }

    cout << "\n--- Uniform Binary Search Results ---" << endl;
    cout << "Number of Search Steps: " << binarySteps << endl;
    if (binaryMatchLines.empty()) {
        cout << "No lines with matching serial number found" << endl;
    }
    else {
        cout << "Matching Original Line Numbers (Total " << binaryMatchLines.size() << " lines): ";
        for (int line : binaryMatchLines) {
            cout << line << " ";
        }
        cout << endl;
    }

    return 0;
}