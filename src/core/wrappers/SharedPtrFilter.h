//
// Created by rutio on 2020-06-21.
//

#ifndef LUCIMALIR_SHAREDPTRFILTER_H
#define LUCIMALIR_SHAREDPTRFILTER_H

#include <memory>
#include <core/pipeline/Filter.h>

class SharedPtrFilter {
public:
  SharedPtrFilter(Filter* filter): filter(filter) {}
  std::shared_ptr<Filter> filter;
};


#endif //LUCIMALIR_SHAREDPTRFILTER_H
