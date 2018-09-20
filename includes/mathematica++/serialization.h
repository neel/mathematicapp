#ifndef MATHEMATICAPP_SERIALIZATION_H
#define MATHEMATICAPP_SERIALIZATION_H

namespace mathematica{
    template <typename T>
    struct association{
        typedef T serialized_type;
    };
}

#endif // MATHEMATICAPP_SERIALIZATION_H
