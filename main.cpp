#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <map>
#include <iterator>


std::vector<uint8_t> read_image_of_fat16(const std::string &name_of_file){
    std::vector<uint8_t> bytes;
    std::ifstream file(name_of_file.c_str(), std::ios::binary);
    file.seekg(0,std::ios::end); // put pointer to the end of file.
    long size = file.tellg(); // tellg() - return value where pointer is and from this we can get a size.
    file.seekg(0, std::ios::beg); // return pointer to begin of file.
    bytes.resize(size + 1); // set size of vector
    file.read(reinterpret_cast<char *>(bytes.data()), size); // write bytes to vector

    return bytes;
}

std::vector<uint8_t> slice(std::vector<uint8_t> &vector, int m, int n) {
    auto first = vector.cbegin() + m;
    auto last = vector.cbegin() + n + 1;

    std::vector<uint8_t> vec(first,last);
    return vec;
}

std::string set_range(std::vector<int> indexes){
    std::ostringstream oss;

    if (!indexes.empty())
    {
        // Convert all but the last element to avoid a trailing ","
        std::copy(indexes.begin(), indexes.end()-1,
                  std::ostream_iterator<int>(oss, ","));

        // Now add the last element with no delimiter
        oss << indexes.back();
    }

    return oss.str();
}

std::vector<uint8_t> get_bytes_per_sector(std::vector<uint8_t> &bytes, std::pair<int, int>byteRange){
    std::vector<uint8_t> sub_vector = slice(bytes, byteRange.first, byteRange.second);
    return sub_vector;
}


int main() {
    std::map<std::string, std::pair<int, int>> bootOptions = {
            {"text_identifier_OS", std::pair<int, int>(0,2)},
            {"machine_instruction", std::pair<int, int>(3,10)},
            {"bytes per sector", std::pair<int, int>(11,12)},
            {"sectors per cluster", std::pair<int, int>(13,13)},
            {"size of reserved area", std::pair<int, int>(14,15)},
            {"number of FATs", std::pair<int, int>(16,16)},
            {"max number of files in root directory", std::pair<int, int>(17,18)},
            {"number of sectors in the file system", std::pair<int, int>(19,20)},
            {"media type", std::pair<int, int>(21,21)},
            {"size of each FAT", std::pair<int, int>(22,23)},
            {"sectors per track in storage device", std::pair<int, int>(24,25)},
            {"number of heads in storage device", std::pair<int, int>(26,27)},
            {"number of sectors before the start partition", std::pair<int, int>(28,31)},
            {"number of sectors in the file system", std::pair<int, int>(32,35)},
            {"signature value", std::pair<int, int>(510,511)}};

    size_t sectorSize = 512;
    std::cout << "work" << std::endl;
    std::vector<uint8_t> bytes = read_image_of_fat16("../hd0_just_FAT16_without_MBR.img");
    std::vector<int> test;


    for(int i=0; i < get_bytes_per_sector(bytes, bootOptions["sectors per cluster"]).size(); ++i){
        test.push_back(unsigned(get_bytes_per_sector(bytes, bootOptions["sectors per cluster"])[i]));
    }

    std::cout<<"Sector size: " << sectorSize << std::endl;
    std::cout<<"Sectors per cluster: " << set_range(test) << std::endl;
    std::cout<<"Number of FATs: " << 2 << std::endl;
    std::cout<<"Number of FATs copies sectors/bytes:" << sectorSize*2 <<std::endl;
    std::cout<<"Root size:" << std::endl;
    std::cout<<"Reserved sectors:" << std::endl;


    /*for(int i=0 ; i<512; ++i){
        std::cout<<bytes[i];
    }*/

    return 0;
}