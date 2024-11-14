#include "main.h"
#include <fstream>

long get_timestamp(const std::string& path) {
  struct stat result;
  if (stat(path.c_str(), &result) == 0) 
    return result.st_mtime;
  
  return 0;
}

bool file_exists(const std::string& path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

long get_file_size(const std::string& path) {
  struct stat result;
  if (stat(path.c_str(), &result) == 0) 
    return result.st_size;
  
  return 0;
}

std::string read_file(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    ERROR("Failed to open file: " << path);
    return "";
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();
  return content;
}

void write_file(const std::string& path, const std::string& content) {
  std::ofstream(file);
  if (!file.is_open()) {
    ERROR("Failed to open file: " << path);
    return;
  }
  file << content;
  file.close();
}

bool copy_file(const std::string& src, const std::string& dest) {
  std::ifstream src_file(src, std::ios::binary);
  if (!src_file.is_open()) {
    ERROR("Failed to open file: " << src);
    return false;
  }

  std::ofstream dest_file(dest, std::ios::binary);
  if (!dest_file.is_open()) {
    ERROR("Failed to open file: " << dest);
    return false;
  }

  dest_file << src_file.rdbuf();
  src_file.close();
  dest_file.close();
  return true;
}