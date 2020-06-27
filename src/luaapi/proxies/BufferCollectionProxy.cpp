//
// Created by rutio on 2020-06-09.
//

#include "BufferCollectionProxy.h"

BufferCollectionProxy::BufferCollectionProxy(
    std::shared_ptr<BufferCollection> subject)
    : subject(subject) {
}
