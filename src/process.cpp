#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  Pid_ = std::stoi(LinuxParser::Uid(pid));
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
}

// Return this process's ID
int Process::Pid() { return Pid_; }

//  Return this process's CPU utilization
float Process::CpuUtilization() {
  long jiffies_start, jiffies_end;
  long uptime_start, uptime_end, delta;

  jiffies_start = LinuxParser::ActiveJiffies(Pid_);
  uptime_start = LinuxParser::UpTime(Pid_);
  // Wait 100ms
  usleep(100000);  // in microseconds

  jiffies_end = LinuxParser::ActiveJiffies(Pid_);
  uptime_end = LinuxParser::UpTime(Pid_);

  delta = uptime_end - uptime_start;
  // if delta is 0, it means the process doesn't use
  // the CPU anymore
  if (delta == 0) {
    return 0;
  }
  return (float)(jiffies_end - jiffies_start) /
         (float)(uptime_end - uptime_start);
}

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid_); }

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { LinuxParser::UpTime(Pid_); }

//  Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  long ram = stol(LinuxParser::Ram(this->Pid_));
  long ram_a = stol(LinuxParser::Ram(a.Pid_));

  if (ram > ram_a) {
    return true;
  }

  return false;
}