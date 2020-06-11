#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iomanip>
#include <unistd.h>
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

//Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string name_1{"MemTotal"};
  string name_2{"MemFree"};
  string name_3{"Buffers"};
  string line;
  float totalMem;
  float freeMem;
  float buffers;
  int counter = 0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (!stream) {
    throw std::runtime_error("Non - existing file");
  }
  while (std::getline(stream, line)) {
    if (counter == 3) {
      break;
    }
    if (line.compare(0, name_1.size(), name_1) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beging(buf), end;
      std::vector<std::string> words(beging, end);
      totalMem = std::stof(words[1]);
      counter++;
    } else if (line.compare(0, name_2.size(), name_2) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beging(buf), end;
      std::vector<std::string> words(beging, end);
      freeMem = std::stof(words[1]);
      counter++;
    } else if (line.compare(0, name_3.size(), name_3) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> beging(buf), end;
      std::vector<std::string> words(beging, end);
      buffers = std::stof(words[1]);
      counter++;
    }
  }
  return float((1 - (freeMem / (totalMem - buffers))));
}
long LinuxParser::UpTime() { 
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return stol(value); 
}

// // TODO: Read and return the system uptime
// long LinuxParser::UpTime() {
//   std::ifstream stream(kProcDirectory + kUptimeFilename);
//   if (!stream) {
//     throw std::runtime_error("Non - existing file");
//   }
//   string line;
//   std::getline(stream, line);
//   std::istringstream buf(line);
//   std::istream_iterator<string> begin(buf), end;
//   vector<string> words(begin, end);
//   return std::stoi(words[0]);
// }

// Jiffies is the number of ticks occurred since system start-up
// jiffies / HZ = seconds
// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long TotalJiffies = 0;
  auto Jiffies = LinuxParser::CpuUtilization();
  for(auto & elem : Jiffies){
      TotalJiffies += std::stoi(elem);
  }
  return TotalJiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, info;
  long total = 0;
  int i = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // Tokenize
    while (std::getline(linestream, info, ' ')) {
      if ((i == 13) || (i == 14) || (i == 15)) {
        total += stol(info);
        i++;
        continue;
      } else if (i == 16) {
        total += stol(info);
        break;
      }
      i++;
    }
  }
  return total;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto Jiffies = LinuxParser::CpuUtilization();
  return std::stoi(Jiffies[kIdle_]) + std::stoi(Jiffies[kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::getline(stream, line);
  std::istringstream buf(line);
  std::istream_iterator<string> beg(buf), end;
  vector<string> words(beg, end);
  words.erase(words.begin(), words.begin() + 1);
  return words;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string mark{"processes"};
  string res;
  if (stream) {
    while (std::getline(stream, line)) {
      if (line.compare(0, mark.size(), mark) == 0) {
        std::istringstream buf(line);
        std::istream_iterator<string> beg(buf), end;
        vector<string> words(beg, end);
        return std::stoi(words[1]);
      }
    }
  } else {
    throw std::runtime_error("no such file");
  }

  return 0;
}
// int LinuxParser::TotalProcesses() { 
//   string line, key, value;
//   std::ifstream filestream(kProcDirectory + kStatFilename);
//   if (filestream.is_open()) {
//     while (std::getline(filestream, line)) {
//       std::istringstream linestream(line);
//       while (linestream >> key >> value) {
//         if (key == "processes") {
//           return stoi(value);
//         }
//       }
//       /*if (line.find("processes") != string::npos){
//         return stoi(line.substr(line.find(" ") + 1));
//       }*/
//     }
//   }

//   return 0;
// }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string mark{"procs_running"};
  string res;
  if (stream) {
    while (std::getline(stream, line)) {
      if (line.compare(0, mark.size(), mark) == 0) {
        std::istringstream buf(line);
        std::istream_iterator<string> beg(buf), end;
        vector<string> words(beg, end);
        return std::stoi(words[1]);
      }
    }
  } else {
    throw std::runtime_error("no such file");
  }

  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + kSlash + std::to_string(pid) +
                       kCmdlineFilename);
  std::getline(stream, line);
  return line;
}

// string LinuxParser::Ram(int pid) {
//   std::string line;
//   // Declaring search attribute for file
//   std::string name = "VmData";
//   std::string value;
//   float result;
//   // Opening stream for specific file
//   std::ifstream stream(kProcDirectory  + std::to_string(pid) +
//                        kStatusFilename);
//   while (std::getline(stream, line)) {
//     // Searching line by line
//     if (line.compare(0, name.size(), name) == 0) {
//       // slicing string line on ws for values using sstream
//       std::istringstream buf(line);
//       std::istream_iterator<std::string> beg(buf);
//       std::istream_iterator<std::string>
//           end;  // constructs an iterator points to the end of a stream
//       std::vector<std::string> values(beg, end);
//       // conversion kB -> GB
//       result = (std::stof(values[1]) / float(1000));
//       break;
//     }
//     return to_string(result);
//   }
// }
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::stringstream ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:"){
          // Convert to MB before returning value
          ram << std::fixed << std::setprecision(3) << stof(value) / 1000;
          return ram.str();
        }
      }
    }
  }
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string UID{"Uid"};
  string line;
  string skip;
  std::ifstream stream(kProcDirectory + kSlash + std::to_string(pid) +
                       kStatusFilename);
  while (std::getline(stream, line)) {
    if (line.compare(0, UID.size(), UID) == 0) {
      std::istringstream buf(line);
      buf >> skip >> UID;
      return UID;
    }
  }
  return "";
}

// Read and return the user associated with a process
// string LinuxParser::User(int pid) {
//   string line;
//   string username;
//   std::fstream stream(kPasswordPath);
//   while (std::getline(stream, line)) {
//     if (line.find("x:" + LinuxParser::Uid(pid)) != std::string::npos) {
//       username = line.substr(0, line.find(":"));
//       break;
//     }
//   }
//   return username;
// }
string LinuxParser::User(int pid) { 
  string line, info, user;
  string uid = LinuxParser::Uid(pid);
  int i;

  if (uid == ""){
    return "None";
  }

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      i = 0;
      while (std::getline(linestream, info, ':')) {
        if (i == 0){
          user = info;
          i++;
          continue;
        }
        if (info == uid) {
          return user;
        }
        else if (i == 2) {
          break;
        }
        i++;
      }
    }
  }
  return "None";
}

long LinuxParser::UpTime(int pid) { 
  string line, info;
  int i;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      i = 0;
      while (std::getline(linestream, info, ' ')) {
        if (i == 21) {
          return (stol(info) / sysconf(_SC_CLK_TCK)) ;
        }
        i++;
      }
    }
  }
return 0;
}