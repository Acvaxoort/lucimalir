//
// Created by rutio on 2020-06-09.
//

#include <core/Core.h>
#include <core/pipeline/Filter.h>
#include <core/pipeline/filters/MandelbrotJulia.h>
#include "FilterProxy.h"

/*
FilterProxy::FilterProxy(Filter* filter) {
  auto filter_shared = std::shared_ptr<Filter>(filter);
  auto shared = std::make_shared<FilterWrapper>(std::move(filter_shared));
  subject = shared;
  Core& core = Core::getInstance();
  core.addFilter(std::move(shared));
}

bool FilterProxy::isExpired() const {
  return subject.expired();
}

FilterWrapper* FilterProxy::getSubject() const {
  if (subject.expired()) {
    throw std::runtime_error("invalid filter object");
  } else {
    return subject.lock().get();
  }
}

bool FilterProxy::isAutoRequestOnChanges() const {
  return getSubject()->isAutoRequestOnChanges();
}

void FilterProxy::setAutoRequestOnChanges(bool value) {
  getSubject()->setAutoRequestOnChanges(value);
}
 */
/*
OptionalError FilterProxy::replaceFilter(FilterProxy* other) {
  FilterWrapper* this_wrapper = getSubject();
  FilterWrapper* other_wrapper = other->getSubject();
  std::shared_ptr<Filter> other_filter = other_wrapper->getFilter().lock();
  auto status = this_wrapper->canReplaceFilter(other_filter.get());
  if (!status.success) {
    return status;
  }
  status = this_wrapper->replaceFilter(std::move(other_filter));
  Core& core = Core::getInstance();
  core.removeFilter(other->subject);
  return status;
}*/
/*
void FilterProxy::interrupt() {
  getSubject()->interrupt();
}

void FilterProxy::requestRendering() {
  getSubject()->requestRendering();
}

OptionalError FilterProxy::connectTo(FilterProxy* prev) {
  FilterWrapper* this_wrapper = getSubject();
  FilterWrapper* other_wrapper = prev->getSubject();
  auto status = this_wrapper->canConnectTo(other_wrapper);
  if (!status.success) {
    return status;
  }
  status = this_wrapper->connectTo(other_wrapper);
  return status;
}

BufferCollectionProxy FilterProxy::getInput() const {
  return BufferCollectionProxy(getSubject()->getInputBuffer().lock());
}

void FilterProxy::setInput(BufferCollectionProxy* buffer) {
  if (buffer) {
    throw std::runtime_error("only nil assignment is supported");
  } else {
    getSubject()->disconnectInput();
  }
}

BufferCollectionProxy FilterProxy::getOutput() const {
  return BufferCollectionProxy(getSubject()->getOutputBuffer().lock());
}

void FilterProxy::setOutput(BufferCollectionProxy* buffer) {
  if (buffer) {
    throw std::runtime_error("only nil assignment is supported");
  } else {
    getSubject()->disconnectOutput();
  }
}

FilterParamsProxy FilterProxy::getParams() const {
  return FilterParamsProxy(subject);
}

OptionalError FilterProxy::connect(FilterProxy* f1, FilterProxy* f2) {
  return f2->connectTo(f1);
}

void FilterProxy::notifyFilterInternalChanges() {
  getSubject()->notifyFilterInternalChanges();
}

void FilterProxy::requestChangingInput() {
  getSubject()->requestChangingInput();
}

FilterProxy FilterProxy::rendererBasic() {
  return FilterProxy(new MandelbrotJulia());
}
*/
