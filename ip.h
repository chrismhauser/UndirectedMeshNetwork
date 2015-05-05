#ifndef IP_H
#define IP_H

#include <string>
#include <cstdlib>
#include <cctype>
#include <iostream>

class Ip
{
public:
    Ip();
    Ip(std::string ip);
    Ip(int num1, int num2, int num3, int num4);

    std::string getIpString();
    int getIpNum(int numIndex) const;

    void setIp(std::string ip);
    void setIp(int num1, int num2, int num3, int num4);

    bool operator==(const Ip& rhs) const {
        for(int i=0; i<4; i++) {
            if(this->ipNums[i] != rhs.getIpNum(i)) return false;
        }
        return true;
    }

    bool operator!=(const Ip& rhs) const {
        return !(*this == rhs);
    }

private:
    int* ipNums = new int[4];
};

#endif // IP_H
