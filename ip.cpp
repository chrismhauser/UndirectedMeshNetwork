#include "ip.h"

Ip::Ip()
{
}

Ip::Ip(std::string ip)
{
    setIp(ip);
}

Ip::Ip(int num1, int num2, int num3, int num4)
{
    setIp(num1, num2, num3, num4);
}

int Ip::getIpNum(int numIndex) const
{
    return ipNums[numIndex];
}

std::string Ip::getIpString()
{
    char* num1 = new char[80];
    char* num2 = new char[80];
    char* num3 = new char[80];
    char* num4 = new char[80];

    itoa(ipNums[0], num1, 10);
    itoa(ipNums[1], num2, 10);
    itoa(ipNums[2], num3, 10);
    itoa(ipNums[3], num4, 10);

    return std::string(num1) + '.' + std::string(num2) + '.' + std::string(num3) + '.' + std::string(num4);
}

void Ip::setIp(int num1, int num2, int num3, int num4)
{
    ipNums[0] = num1;
    ipNums[1] = num2;
    ipNums[2] = num3;
    ipNums[3] = num4;

    for(int i=0; i<4; i++) {
        if(ipNums[i] < 0 || ipNums[i] >= 256) {
            std::cout << "Error: Invalid IP Adress (IP Num " << i+1 << "(" << ipNums[i] <<") Out of Bounds)" << std::endl;
        }
    }
}


// UNUSED
void Ip::setIp(std::string ip)
{
    int temp = 0;
    int index = 0;
    for(int i=0; i<ip.length(); i++) {
        if(isdigit(ip[i])) {
            temp = temp*10 + ip[i]-'0'; // check here
        } else if(ip[i] == '.') {
            if(temp >= 256) {
                std::cout << "Error: Out of bounds  number found in IP Adress (" + ip[i] + ')' << std::endl;
            }

            ipNums[index] = temp;
            temp = 0;
            index++;
        } else {
            std::cout << "Error: Unknown character found in IP Adress (" + ip[i] + ')' << std::endl;
        }
    }
}
