//
// Created by rutio on 2020-06-21.
//

#ifndef LUCIMALIR_WEAKPTRFILTERWRAPPER_H
#define LUCIMALIR_WEAKPTRFILTERWRAPPER_H

#include <memory>

class WeakPtrFilterWrapper {
public:
  WeakPtrFilterWrapper(std::weak_ptr<FilterWrapper> filter_wrapper)
      : filter_wrapper(filter_wrapper) {}

  FilterWrapper* lockGet() {
    return filter_wrapper.lock().get();
  }

  std::weak_ptr<FilterWrapper> filter_wrapper;
};


#endif //LUCIMALIR_WEAKPTRFILTERWRAPPER_H
