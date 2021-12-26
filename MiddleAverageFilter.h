#pragma once

namespace Math
{
  
  template<typename T, unsigned size>
    class MiddleAverageFilter
  {
    T data[size] = {} ;
    unsigned id = 0u ;
    T sum = 0 ;
    unsigned tmp = 0u ;
  public:
    void push(const T& value)
    {
      sum = sum - data[id] ;
      data[id] = value ;
      sum = sum + value ;
      id = (id + 1) % size ;
      if (tmp < size) {
	tmp++ ;
      }
    }
    
    T getAverage() const
    {
      //T sum = 0;
      return sum / tmp;
    }
  };
}
