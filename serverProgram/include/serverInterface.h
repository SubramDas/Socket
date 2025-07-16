#ifndef ISERVER_H
#define ISERVER_H

// Guards
// "Redefinition of class"
// "Function already defined"

#include "../../common/common.h"

class ServerInterface {
public:
    virtual void start() = 0;
    virtual ~ServerInterface() = default;
};

#endif // ISERVER_H
