//
// Created by rutio on 2020-06-09.
//

#ifndef LUCIMALIR_FILTERPROXY_H
#define LUCIMALIR_FILTERPROXY_H

#include <core/pipeline/FilterWrapper.h>
#include <memory>
#include "BufferCollectionProxy.h"
#include "FilterParamsProxy.h"

class FilterProxy {
public:
  explicit FilterProxy(Filter* filter);

  // -------------------------------------------------------------------------
  // Visible from lua
  // -------------------------------------------------------------------------

  // property expired
  bool isExpired() const;

  // property auto_request
  bool isAutoRequestOnChanges() const;

  void setAutoRequestOnChanges(bool value);

  // function replace_filter
  //OptionalError replaceFilter(FilterProxy* other);

  // function interrupt
  void interrupt();

  // function request_rendering
  void requestRendering();

  // function connect_to
  OptionalError connectTo(FilterProxy* prev);

  // property input
  BufferCollectionProxy getInput() const;

  void setInput(BufferCollectionProxy* buffer);

  // property output
  BufferCollectionProxy getOutput() const;

  void setOutput(BufferCollectionProxy* buffer);

  // property params
  FilterParamsProxy getParams() const;

  // -------------------------------------------------------------------------
  // Static functions visible from lua, visible in "filters" namespace
  // -------------------------------------------------------------------------

  // function connect
  static OptionalError connect(FilterProxy* f1, FilterProxy* f2);

  // function renderers.basic
  static FilterProxy rendererBasic();

protected:
  // -------------------------------------------------------------------------
  // Helper functions for derived classes
  // -------------------------------------------------------------------------

  void notifyFilterInternalChanges();

  void requestChangingInput();

private:
  // gets raw pointer after checking that the subject pointer isn't expired
  // otherwise throws
  FilterWrapper* getSubject() const;

  std::weak_ptr<FilterWrapper> subject;
};


#endif //LUCIMALIR_FILTERPROXY_H
