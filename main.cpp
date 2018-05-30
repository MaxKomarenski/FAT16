#include <iostream>
#include <vector>
#include <fstream>

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



int main() {
    std::cout << "work" << std::endl;
    std::vector<uint8_t> bytes = read_image_of_fat16("hd0_just_FAT16_without_MBR.img");
    
    return 0;
}