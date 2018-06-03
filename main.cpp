#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <map>
#include <iterator>
#include <algorithm>

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

std::vector<uint8_t> slice(const std::vector<uint8_t> &vector, int m, int n) {
    auto first = vector.cbegin() + m;
    auto last = vector.cbegin() + n + 1;

    std::vector<uint8_t> vec(first,last);
    return vec;
}

std::vector<uint8_t> get_info(std::vector<uint8_t> &bytes, std::pair<int, int>byteRange){
    std::vector<uint8_t> sub_vector = slice(bytes, byteRange.first, byteRange.second);
    return sub_vector;
}


std::vector<std::vector<uint8_t >> get_all_files(std::vector<uint8_t> &bytes, uint16_t start, uint16_t size){
    std::vector<uint8_t> files = slice(bytes, start, bytes.size() - 1);

    int m = 0;
    int n = 31;
    std::vector<std::vector<uint8_t>> all_files;

    for(int i = m ; i < size ; i++){

        std::vector<uint8_t> file = slice(files, m, n);
        all_files.push_back(file);
        m = m + 32;
        n = n + 32;

    }

    return all_files;
}

uint16_t get_data(int offset, std::vector<uint8_t> &bytes){
    return (*reinterpret_cast<uint16_t *> (bytes.data() + offset));
}

int main(){
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

    std::map<std::string, std::pair<int,int>> fileOption = {
            {"name", std::pair<int,int>(0,7)},
            {"extension", std::pair<int, int>(8, 10)},
            {"attributes",std::pair<int,int>(11, 11)},
            {"creation time", std::pair<int,int>(14,15)},
            {"creation date", std::pair<int,int>(16,17)},
            {"size of file", std::pair<int,int>(28,31)},
            {"number of first cluster", std::pair<int,int>(20,21)},
            {"modified time", std::pair<int,int>(22,23)},
            {"modified date", std::pair<int,int>(24,25)}};

    size_t sectorSize = 512;
    std::vector<uint8_t> bytes = read_image_of_fat16("../hd0_just_FAT16_without_MBR.img");

    std::vector<uint8_t> bytesPerSector = get_info(bytes, bootOptions["bytes per sector"]);
    std::vector<uint8_t> reservedArea = get_info(bytes, bootOptions["size of reserved area"]);
    std::vector<uint8_t> numberOfFATs = get_info(bytes, bootOptions["number of FATs"]);
    std::vector<uint8_t> sizeOfEachFAT = get_info(bytes, bootOptions["size of each FAT"]);
    std::vector<uint8_t> sectorPerCluster = get_info(bytes, bootOptions["sectors per cluster"]);
    std::vector<uint8_t> maxNumberOfFilesInRootDirectory = get_info(bytes, bootOptions["max number of files in root directory"]);


    uint16_t start_point = get_data(0,bytesPerSector)
                      * get_data(0,reservedArea)
                      + get_data(0,numberOfFATs)
                        *get_data(0, sizeOfEachFAT)
                        *get_data(0,bytesPerSector);
    std::cout<<"Files starts: " << start_point << std::endl;


    std::cout<<"Sectors per cluster: " << get_data(0, sectorPerCluster) << std::endl;
    std::cout<<"Number of FATs: " << get_data(0,numberOfFATs) << std::endl;
    std::cout<<"Number of FATs copies sectors/bytes: " << get_data(0, sizeOfEachFAT) <<"/"<< get_data(0, sizeOfEachFAT)*get_data(0,bytesPerSector)<<std::endl;
    std::cout<<"Root size: "<< get_data(0, maxNumberOfFilesInRootDirectory) << std::endl;
    std::cout<<"Reserved sectors: " << get_data(0,reservedArea) << std::endl;
    std::cout<<"-----------------------------------------"<<std::endl;


    //getting files information
    std::vector<std::vector<uint8_t>> all_files = get_all_files(bytes, start_point, get_data(0, maxNumberOfFilesInRootDirectory)/32);

    for (int i = 0; i < all_files.size(); ++i) {

        std::vector<uint8_t> name = get_info(all_files[i], fileOption["name"]);
        std::vector<uint8_t> extension = get_info(all_files[i], fileOption["extension"]);


        for (int j = 0; j < name.size(); ++j) {
            std::cout << (char) get_data(j, name);
        }

        std::cout << ".";

        for (int k  = 0; k < extension.size(); ++k){
            std::cout << (char) get_data(k, extension);
        }


        std::cout << "" << std::endl;

        std::vector<uint8_t> sizeOfFile = get_info(all_files[i], fileOption["size of file"]);
        std::vector<uint8_t> creationFileTime = get_info(all_files[i], fileOption["creation time"]);
        std::vector<uint8_t> modifiedFileTime = get_info(all_files[i], fileOption["modified time"]);
        std::vector<uint8_t> attributes = get_info(all_files[i], fileOption["attributes"]);
        std::vector<uint8_t> numOfFirstCluster = get_info(all_files[i], fileOption["number of first cluster"]);


        std::cout<<"Size of file: "<<get_data(0, sizeOfFile)<<std::endl;
        std::cout<<"Date and time of creation: "<<get_data(0, creationFileTime)<<std::endl;
        std::cout<<"Date and time of modification: "<<get_data(0, modifiedFileTime)<<std::endl;
        std::cout<<"Attributes: "<<get_data(0, attributes)<<std::endl;
        std::cout<<"Number of first cluster: "<<get_data(0, numOfFirstCluster)<<std::endl;
        std::cout<<"-----------------------------------------"<<std::endl;

    }

}
