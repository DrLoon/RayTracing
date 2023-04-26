#pragma once
#define PI 3.14159265358979323846

auto convert_to_rad = [](double angle) { return angle * PI / 180.0; };

bool sign(const double n) {
	if (n > 0)
		return true;
	else
		return false;
}

int get_int(std::string& line, const std::string& diff_symbol) {
    std::string num_s;
    for (int i = 0; i < line.size(); ++i) {
        if (std::isdigit(line[i])) {
            num_s.push_back(line[i]);
        }
        else {
            if (diff_symbol == line.substr(i, diff_symbol.size())) {
                int a = std::atof(num_s.c_str());
                num_s.clear();
                line = line.substr(i + diff_symbol.size());
                return a;
            }
            if (num_s.size() == 0) {
                throw "strange";
            }
            throw "strange";
        }
    }
    if (num_s.size() == 0) {
        throw "strange";
    }
    else {
        int a = std::atof(num_s.c_str());
        num_s.clear();
        line.clear();
        return a;
    }
}
