#include "IO.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;

namespace {

string trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

vector<string> split(const string& line, char delim) {
    vector<string> out;
    stringstream ss(line);
    string cell;
    while (getline(ss, cell, delim)) out.push_back(trim(cell));
    if (!line.empty() && line.back() == delim) out.push_back("");
    return out;
}

bool isInt(const string& s) {
    if (s.empty()) return false;
    char* end = nullptr;
    strtol(s.c_str(), &end, 10);
    return *end == '\0';
}

bool isDouble(const string& s) {
    if (s.empty()) return false;
    char* end = nullptr;
    strtod(s.c_str(), &end);
    return *end == '\0';
}

template <typename T, typename Convert>
unique_ptr<ColumnBase> makeColumn(const string& name, const vector<string>& raw,
                                       Convert convert) {
    auto col = make_unique<Column<T>>(name);
    for (const auto& v : raw) col->addValue(convert(v));
    return col;
}

string escapeJSON(const string& s) {
    string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

}

void FileFormat::checkExtension(const string& path) const {
    const string ext = extension();
    if (path.size() < ext.size() || path.compare(path.size() - ext.size(), ext.size(), ext) != 0)
        throw FileException("'" + path + "' does not have extension " + ext);
}

DataSet CSVHandler::importFrom(const string& path) const {
    checkExtension(path);
    ifstream in(path);
    if (!in) throw FileException("cannot open '" + path + "'");

    string line;
    if (!getline(in, line)) throw FileException("'" + path + "' is empty");
    vector<string> headers = split(line, delimiter);

    vector<vector<string>> raw(headers.size());
    size_t lineNo = 1;
    while (getline(in, line)) {
        ++lineNo;
        if (trim(line).empty()) continue;
        auto cells = split(line, delimiter);
        if (cells.size() != headers.size())
            throw FileException("line " + to_string(lineNo) + " has " + to_string(cells.size()) +
                                " fields, expected " + to_string(headers.size()));
        for (size_t i = 0; i < cells.size(); ++i) raw[i].push_back(cells[i]);
    }

    DataSet ds;
    for (size_t i = 0; i < headers.size(); ++i) {
        const auto& vals = raw[i];
        bool allInt = !vals.empty(), allDouble = !vals.empty();
        for (const auto& v : vals) {
            allInt = allInt && isInt(v);
            allDouble = allDouble && isDouble(v);
        }

        if (allInt)
            ds.addColumn(makeColumn<int>(headers[i], vals, [](const string& s) { return stoi(s); }));
        else if (allDouble)
            ds.addColumn(makeColumn<double>(headers[i], vals, [](const string& s) { return stod(s); }));
        else
            ds.addColumn(makeColumn<string>(headers[i], vals, [](const string& s) { return s; }));
    }
    return ds;
}

void CSVHandler::exportTo(const DataSet& ds, const string& path) const {
    checkExtension(path);
    ofstream out(path);
    if (!out) throw FileException("cannot write '" + path + "'");

    auto names = ds.columnNames();
    for (size_t c = 0; c < names.size(); ++c) out << (c ? string(1, delimiter) : "") << names[c];
    out << "\n";

    for (size_t r = 0; r < ds.rowCount(); ++r) {
        for (size_t c = 0; c < ds.columnCount(); ++c)
            out << (c ? string(1, delimiter) : "") << ds.columnAt(c).getAsString(r);
        out << "\n";
    }
}

void JSONExporter::exportTo(const DataSet& ds, const string& path) const {
    checkExtension(path);
    ofstream out(path);
    if (!out) throw FileException("cannot write '" + path + "'");

    out << "[\n";
    for (size_t r = 0; r < ds.rowCount(); ++r) {
        out << "  {";
        for (size_t c = 0; c < ds.columnCount(); ++c) {
            const ColumnBase& col = ds.columnAt(c);
            out << (c ? ", " : "") << "\"" << escapeJSON(col.getName()) << "\": ";
            if (col.isNumeric())
                out << col.getAsString(r);
            else
                out << "\"" << escapeJSON(col.getAsString(r)) << "\"";
        }
        out << "}" << (r + 1 < ds.rowCount() ? "," : "") << "\n";
    }
    out << "]\n";
}
