//
// Created by rutio on 2020-06-09.
//

#ifndef LUCIMALIR_BUFFERCOLLECTIONPROXY_H
#define LUCIMALIR_BUFFERCOLLECTIONPROXY_H

#include <core/pipeline/BufferCollection.h>

class BufferCollectionProxy {
public:
  BufferCollectionProxy(std::shared_ptr<BufferCollection> subject);

  // property expired
  bool isExpired() const;

private:
  BufferCollection* getSubject() const;

  std::weak_ptr<BufferCollection> subject;
};


#endif //LUCIMALIR_BUFFERCOLLECTIONPROXY_H
