#include <iostream>
#include <string>
#include <cmath>
using namespace std;

//尝试数
#define max_num 65536//2^16
//查表
string inlist[max_num];
string outlist[max_num];
string BinToHex(string str);//二进制转换为十六进制
string HexToBin(string str);//十六进制转换为二进制
int BinToDec(string str);//二进制转换为十进制
string DecToBin(int str);//十进制转换为二进制
int HexToDec(string str);//十六进制转换为十进制
string DecToHex(int str);//十进制转换为十六进制
string padding(string str);//对数据进行填充的函数实现
string RollLeft(string str, int len);//实现循环左移len位功能
string XOR(string str1, string str2);//实现异或操作
string AND(string str1, string str2);//实现与操作
string OR(string str1, string str2);//实现或操作
string NOT(string str);//实现非操作
char Xor_bit(char str1, char str2);//实现单比特的异或操作
char And_bit(char str1, char str2);//实现单比特的与操作
string ModAdd(string str1, string str2);//mod 2^32运算的函数实现
string P1(string str);//实现置换功能P1（X）
string P0(string str);//实现置换功能P0（X）
string T(int j);//返回Tj常量值的函数实现
string FF(string str1, string str2, string str3, int j);//实现布尔函数FF功能
string GG(string str1, string str2, string str3, int j);//实现布尔函数GG
string extension(string str);//消息扩展函数
string compress(string str1, string str2);//消息压缩函数
string iteration(string str);//迭代压缩函数实现


string BinToHex(string str) {
	string hex = "";
	int temp = 0;
	while (str.size() % 4 != 0) {
		str = "0" + str;
	}
	for (int i = 0; i < str.size(); i += 4) {
		temp = (str[i] - '0') * 8 + (str[i + 1] - '0') * 4 + (str[i + 2] - '0') * 2 + (str[i + 3] - '0') * 1;
		if (temp < 10) {
			hex += to_string(temp);
		}
		else {
			hex += 'A' + (temp - 10);
		}
	}
	return hex;
}

string HexToBin(string str) {
	string bin = "";
	string table[16] = { "0000","0001","0010","0011","0100","0101","0110","0111","1000","1001","1010","1011","1100","1101","1110","1111" };
	for (int i = 0; i < str.size(); i++) {
		if (str[i] >= 'A' && str[i] <= 'F') {
			bin += table[str[i] - 'A' + 10];
		}
		else {
			bin += table[str[i] - '0'];
		}
	}
	return bin;
}

int BinToDec(string str) {
	int dec = 0;
	for (int i = 0; i < str.size(); i++) {
		dec += (str[i] - '0') * pow(2, str.size() - i - 1);
	}
	return dec;
}

string DecToBin(int str) {
	string bin = "";
	while (str >= 1) {
		bin = to_string(str % 2) + bin;
		str = str / 2;
	}
	return bin;
}

int HexToDec(string str) {
	int dec = 0;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] >= 'A' && str[i] <= 'F') {
			dec += (str[i] - 'A' + 10) * pow(16, str.size() - i - 1);
		}
		else {
			dec += (str[i] - '0') * pow(16, str.size() - i - 1);
		}
	}
	return dec;
}

string DecToHex(int str) {
	string hex = "";
	int temp = 0;
	while (str >= 1) {
		temp = str % 16;
		if (temp < 10 && temp >= 0) {
			hex = to_string(temp) + hex;
		}
		else {
			hex += ('A' + (temp - 10));
		}
		str = str / 16;
	}
	return hex;
}

string padding(string str) {
	string res = "";
	for (int i = 0; i < str.size(); i++) {
		res += DecToHex((int)str[i]);
	}

	int res_length = res.size() * 4;
	//根据sm3填充规则,在获得的数据后面添1，在16进制下相当于是添加8
	res += "8";
	while (res.size() % 128 != 112) {
		res += "0";//“0”数据填充
	}
	string res_len = DecToHex(res_length);//用于记录数据长度的字符串
	while (res_len.size() != 16) {
		res_len = "0" + res_len;
	}
	res += res_len;
	return res;
}

string RollLeft(string str, int len) {
	string res = HexToBin(str);
	res = res.substr(len) + res.substr(0, len);
	return BinToHex(res);
}

string XOR(string str1, string str2) {
	string res1 = HexToBin(str1);
	string res2 = HexToBin(str2);
	string res = "";
	for (int i = 0; i < res1.size(); i++) {
		if (res1[i] == res2[i]) {
			res += "0";
		}
		else {
			res += "1";
		}
	}
	return BinToHex(res);
}

string AND(string str1, string str2) {
	string res1 = HexToBin(str1);
	string res2 = HexToBin(str2);
	string res = "";
	for (int i = 0; i < res1.size(); i++) {
		if (res1[i] == '1' && res2[i] == '1') {
			res += "1";
		}
		else {
			res += "0";
		}
	}
	return BinToHex(res);
}

string OR(string str1, string str2) {
	string res1 = HexToBin(str1);
	string res2 = HexToBin(str2);
	string res = "";
	for (int i = 0; i < res1.size(); i++) {
		if (res1[i] == '0' && res2[i] == '0') {
			res += "0";
		}
		else {
			res += "1";
		}
	}
	return BinToHex(res);
}

string NOT(string str) {
	string res1 = HexToBin(str);
	string res = "";
	for (int i = 0; i < res1.size(); i++) {
		if (res1[i] == '0') {
			res += "1";
		}
		else {
			res += "0";
		}
	}
	return BinToHex(res);
}

char Xor_bit(char str1, char str2) {
	return str1 == str2 ? '0' : '1';
}

char And_bit(char str1, char str2) {
	return (str1 == '1' && str2 == '1') ? '1' : '0';
}

string ModAdd(string str1, string str2) {
	string res1 = HexToBin(str1);
	string res2 = HexToBin(str2);
	char temp = '0';
	string res = "";
	for (int i = res1.size() - 1; i >= 0; i--) {
		res = Xor_bit(Xor_bit(res1[i], res2[i]), temp) + res;
		if (And_bit(res1[i], res2[i]) == '1') {
			temp = '1';
		}
		else {
			if (Xor_bit(res1[i], res2[i]) == '1') {
				temp = And_bit('1', temp);
			}
			else {
				temp = '0';
			}
		}
	}
	return BinToHex(res);
}

string P1(string str) {
	return XOR(XOR(str, RollLeft(str, 15)), RollLeft(str, 23));
}

string P0(string str) {
	return XOR(XOR(str, RollLeft(str, 9)), RollLeft(str, 17));
}

string T(int j) {
	//常量
	if (0 <= j && j <= 15) {
		return "79CC4519";
	}
	else {
		return "7A879D8A";
	}
}

string FF(string str1, string str2, string str3, int j) {
	if (0 <= j && j <= 15) {
		return XOR(XOR(str1, str2), str3);
	}
	else {
		return OR(OR(AND(str1, str2), AND(str1, str3)), AND(str2, str3));
	}
}

string GG(string str1, string str2, string str3, int j) {
	if (0 <= j && j <= 15) {
		return XOR(XOR(str1, str2), str3);
	}
	else {
		return OR(AND(str1, str2), AND(NOT(str1), str3));
	}
}
string extension(string str) {
	string res = str;//字符串类型存储前68位存储扩展字W值
	for (int i = 16; i < 68; i++) {//根据公式生成第17位到第68位的W值
		res += XOR(XOR(P1(XOR(XOR(res.substr((i - 16) * 8, 8), res.substr((i - 9) * 8, 8)), RollLeft(res.substr((i - 3) * 8, 8), 15))), RollLeft(res.substr((i - 13) * 8, 8), 7)), res.substr((i - 6) * 8, 8));
	}

	//根据公式生成64位W'
	for (int j = 0; j < 64; j++)
	{
		res += XOR(res.substr((j * 8), 8), res.substr((j + 4) * 8, 8));
	}
	return res;
}

string compress(string str1, string str2) {
	string IV = str2;
	string A = IV.substr(0, 8), B = IV.substr(8, 8), C = IV.substr(16, 8), D = IV.substr(24, 8), E = IV.substr(32, 8), F = IV.substr(40, 8), G = IV.substr(48, 8), H = IV.substr(56, 8);
	string SS1 = "", SS2 = "", TT1 = "", TT2 = "";

	for (int j = 0; j < 64; j++) {
		SS1 = RollLeft(ModAdd(ModAdd(RollLeft(A, 12), E), RollLeft(T(j), (j % 32))), 7);
		SS2 = XOR(SS1, RollLeft(A, 12));
		TT1 = ModAdd(ModAdd(ModAdd(FF(A, B, C, j), D), SS2), str1.substr((j + 68) * 8, 8));
		TT2 = ModAdd(ModAdd(ModAdd(GG(E, F, G, j), H), SS1), str1.substr(j * 8, 8));
		D = C;
		C = RollLeft(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = RollLeft(F, 19);
		F = E;
		E = P0(TT2);
	}
	string res = (A + B + C + D + E + F + G + H);
	//cout << endl;
	return res;
}

string iteration(string str) {
	int num = str.size() / 128;
	string V = "7380166F4914B2B9172442D7DA8A0600A96F30BC163138AAE38DEE4DB0FB0E4E";
	string B = "", extensionB = "", compressB = "";
	for (int i = 0; i < num; i++) {
		//cout << "the " << to_string(i + 1) << " group:" << endl;
		//cout << endl;
		B = str.substr(i * 128, 128);
		extensionB = extension(B);
		compressB = compress(extensionB, V);
		V = XOR(V, compressB);
	}
	return V;
}

string SM3(string message)
{
	//string Mmessage = dec_bin(message);
	string paddingmessage = padding(message);
	paddingmessage = BinToHex(paddingmessage);
	string result = iteration(paddingmessage);
	return result;
}

void birthday_attack() {
	string str;
	string result;
	string paddingValue;
	for (int i = 0; i < max_num; outlist[i++] = result) {
		cout << "目前进度：" << i << endl;
		str = to_string(i);
		inlist[i] = str;
		paddingValue = padding(str);
		result = iteration(paddingValue);
		//查表寻找弱碰撞
		for (int j = 0; j < i; j++) {
			if (outlist[j].substr(0, 4) == result.substr(0, 4)) {
				cout << endl;
				cout << "碰撞字符串1 :" + str << endl << endl;
				cout << "杂凑值1:" << endl;
				cout << result.substr(0, 8) << "  ";
				cout << result.substr(8, 8) << "  ";
				cout << result.substr(16, 8) << "  ";
				cout << result.substr(24, 8) << "  ";
				cout << result.substr(32, 8) << "  ";
				cout << result.substr(40, 8) << "  ";
				cout << result.substr(48, 8) << "  ";
				cout << result.substr(56, 8) << "  ";
				cout << endl;
				cout << "碰撞字符串 2 :" + inlist[j] << endl << endl;
				cout << "杂凑值 2:" << endl;
				cout << outlist[j].substr(0, 8) << "  ";
				cout << outlist[j].substr(8, 8) << "  ";
				cout << outlist[j].substr(16, 8) << "  ";
				cout << outlist[j].substr(24, 8) << "  ";
				cout << outlist[j].substr(32, 8) << "  ";
				cout << outlist[j].substr(40, 8) << "  ";
				cout << outlist[j].substr(48, 8) << "  ";
				cout << outlist[j].substr(56, 8) << "  ";
				cout << endl << "查表数:  " << i;
				return;
			}
		}
	}
	cout << "birthday attack failed!";
}

int main() {
	clock_t start, end;
	start = clock();
	birthday_attack();
	end = clock();
	cout << "消耗时间：" << (float)(end - start) * 1000 / CLOCKS_PER_SEC << "ms";
	return 0;
}
