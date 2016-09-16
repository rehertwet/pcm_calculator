/// 2016.Sep.16 xxf
/// trying to do some signal PCM coding...
/// good luck!!¯¯
/// -------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>

#define SIZE_T	size_t
#define	UINT	unsigned int
#define	SHORT	short
using namespace std;

typedef struct
{
	UINT div;	// the length of one timestamp = 1/2^div
	vector<SHORT> y;
} SIG;

int sig_encoding_ami(SIG &os, const SIG &is);
int sig_decoding_ami(SIG &os, const SIG &is);
int sig_encoding_hdb3(SIG &os, const SIG &is);
int sig_decoding_hdb3(SIG &os, const SIG &is);
int sig_encoding_cmi(SIG &os, const SIG &is);
int sig_decoding_cmi(SIG &os, const SIG &is);
void sig_print(const SIG &s, UINT scale);

int main(int argc, char *argv[])
{
	SIG is, os;
	string comm;
	while (1)
	{
		cin >> comm;// get command
		if (comm.empty())	{continue;}
		if (!comm.compare("set"))
		{
			UINT d;
			SHORT a;
			cin >> d >> comm;
			is.div = d;	is.y.clear();
			for (SIZE_T i = 0; i < comm.size(); i++)
			{
				switch (comm[i])
				{
				case '0':
					a = 0;
					break;
				case '1':
					a = 1;
					break;
				default:
					continue;
				}
				is.y.push_back(a);
			}
			continue;
		}
		if (!comm.compare("show"))	{sig_print(is, 1); continue;}
		if (!comm.compare("rev"))	{is = os; continue;}
		if (!comm.compare("ami"))
		{
			sig_encoding_ami(os, is);
			sig_print(os, 1);
			continue;
		}
		if (!comm.compare("dami"))
		{
			sig_decoding_ami(os, is);
			sig_print(os, 1);
			continue;
		}
		if (!comm.compare("hdb3"))
		{
			sig_encoding_hdb3(os, is);
			sig_print(os, 1);
			continue;
		}
		if (!comm.compare("dhdb3"))
		{
			sig_decoding_hdb3(os, is);
			sig_print(os, 1);
			continue;
		}
		if (!comm.compare("cmi"))
		{
			sig_encoding_cmi(os, is);
			sig_print(os, 1);
			continue;
		}
		if (!comm.compare("dcmi"))
		{
			sig_decoding_cmi(os, is);
			sig_print(os, 1);
			continue;
		}
		if (!comm.compare("exit"))	{break;}
	}
	return 0;
}

int sig_encoding_ami(SIG &os, const SIG &is)
{
	os.div = is.div << 1;
	os.y.clear();
	SHORT state = 1;
	for (SIZE_T i = 0; i < is.y.size(); i++)
	{
		if (is.y[i])
		{
			os.y.push_back(state);
			os.y.push_back(0);
			state = -state;
		}
		else
		{
			os.y.push_back(0);
			os.y.push_back(0);
		}
	}
	return 0;
}

int sig_decoding_ami(SIG &os, const SIG &is)
{
	os.div = is.div >> 1;
	os.y.clear();
	for (SIZE_T i = 0; i < is.y.size(); i += 2)
	{
		if (is.y[i])	// 1/-1
		{
			if (is.y[i+1])	return -1;	// wrong
			else	os.y.push_back(1);
		}
		else
		{
			if (is.y[i+1])	return -1;	// wrong
			else	os.y.push_back(0);
		}
	}
	return 0;
}

int sig_encoding_hdb3(SIG &os, const SIG &is)
{
	os.div = is.div << 1;
	os.y.clear();
	SHORT state = 1, vstate = 1, zero_count = 0;
	for (SIZE_T i = 0; i < is.y.size(); i++)
	{
		if (is.y[i])
		{
			os.y.push_back(state);
			os.y.push_back(0);
			state = -state;
			zero_count = 0;
		}
		else if (zero_count < 3)
		{
			os.y.push_back(0);
			os.y.push_back(0);
			zero_count++;
		}
		else	// +V/-V
		{
			os.y.push_back(vstate);
			os.y.push_back(0);
			vstate = -vstate;
			zero_count = 0;
			if (vstate != state)	// +B/-B
			{
				os.y[(i-3)*2] = state;
				state = -state;
			}	// no BB
		}
	}
	return 0;
}

int sig_decoding_hdb3(SIG &os, const SIG &is)
{
	os.div = is.div >> 1;
	os.y.clear();
	SHORT state = 1;
	for (SIZE_T i = 0; i < is.y.size(); i += 2)
	{
		if (is.y[i])	// 1/-1
		{
			if (is.y[i+1])	return -1;	// wrong
			else if (is.y[i] == state)	
			{
				os.y.push_back(1);
				state = -state;
			}
			else
			{
				os.y.push_back(0);
				if (i < 6)	return -2;
				os.y[i/2-3] = 0;
			}
		}
		else
		{
			if (is.y[i+1])	return -1;	// wrong
			else	os.y.push_back(0);
		}
	}
	return 0;
}

int sig_encoding_cmi(SIG &os, const SIG &is)
{
	os.div = is.div << 1;
	os.y.clear();
	SHORT state = 1;
	for (SIZE_T i = 0; i < is.y.size(); i++)
	{
		if (is.y[i])
		{
			os.y.push_back(state);
			os.y.push_back(state);
			state = -state;
		}
		else
		{
			os.y.push_back(-1);
			os.y.push_back(1);
		}
	}
	return 0;
}

int sig_decoding_cmi(SIG &os, const SIG &is)
{
	os.div = is.div >> 1;
	os.y.clear();
	for (SIZE_T i = 0; i < is.y.size(); i += 2)
	{
		if (is.y[i] == is.y[i+1])	os.y.push_back(1);
		else	os.y.push_back(0);
	}
	return 0;
}

void sig_print(const SIG &s, UINT scale)
{
	UINT k = s.div << scale, c = 2;
	for (SIZE_T i = 0; i < s.y.size(); i++)
	{
		cout << s.y[i];
		if (c < k)	c += 2;
		else
		{
			cout << ",\t";
			c = 2;
		}
	}
	cout << endl;
}




















