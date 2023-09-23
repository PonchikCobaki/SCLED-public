#pragma once

#include <Stream.h>



namespace APICLG {

  class PathParameters { 
    public:
    
    String name;
    String value;
    
    PathParameters();
    PathParameters(String name, String value);

  };

}
