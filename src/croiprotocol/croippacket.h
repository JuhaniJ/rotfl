#ifndef CROIPPACKETWRAP_H
#define CROIPPACKETWRAP_H

#include <cstdlib>

namespace Croip {

class CroipPacket
{
public:
    CroipPacket();
    virtual ~CroipPacket();

    virtual int serialize(char * const buf) const;
    virtual int deserialize(char * const buf) const;

private:
    u_int32_t type_;
};

} //namespace Croip

#endif // CROIPPACKETWRAP_H
