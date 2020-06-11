#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  

 private:
  long total_jiffies_start = 0;
  long active_jiffies_start = 0;
  long total_jiffies_end = 0;
  long active_jiffies_end = 0;	
};

#endif