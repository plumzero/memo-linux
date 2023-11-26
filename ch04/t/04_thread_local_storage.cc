
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <assert.h>

static __thread char _buf[512];

// 这样定义内存大概率会触发退出
// char _buf[512];

std::vector<std::vector<std::string>> _vvec;

std::string strs[5] = {
	"The thermometer had fallen to zero.",
	"There's only room for one person.",
	"People arrived in twos and threes.",
	"I'll come at three.",
	"We were crawling around on all fours."
};

int main()
{
	const int th_num = 50;
	_vvec.resize(th_num);

    std::vector<std::thread> vec;
    for (int i = 0; i < th_num; i++) {
        std::thread th = std::thread([i](){
			for (int j = 0; j < 100000; j++) {
				int len = snprintf(_buf, 512, "%d", j);
				_vvec[i].emplace_back(strs[i % 5] + std::string(_buf, len));
			}
		});
        vec.emplace_back(std::move(th));
    }
    
    std::for_each(vec.begin(), vec.end(), std::mem_fn(&std::thread::join));

	for (int i = 0; i < _vvec.size(); i++) {
		size_t pos = _vvec[i][0].find_last_of('.');
		std::string half_str = _vvec[i][0].substr(0, pos);
		std::string half_two = _vvec[i][0].substr(pos + 1, _vvec[i][0].size());
		int half_int = std::atoi(half_two.c_str());
		printf("[%d]: %s|%s|%d\n", i, half_str.c_str(), half_two.c_str(), half_int);
		for (int j = 0; j < _vvec[i].size(); j++) {
			size_t pos = _vvec[i][j].find_last_of('.');
			std::string part_str = _vvec[i][j].substr(0, pos);
			std::string part_two = _vvec[i][j].substr(pos + 1, _vvec[i][j].size());
			int part_int = std::atoi(part_two.c_str());
			// assert(half_str == part_str);
			// assert(half_int + j == part_int);
			if (half_str != part_str) {
				printf("%s != %s\n", half_str.c_str(), part_str.c_str());
				exit(-1);
			}
			if (half_int + j != part_int) {
				printf("%d != %d\n", half_int + j, part_int);
				exit(-1);
			}
		}
	}

	printf("=== over ===\n");
    
	return 0;
}