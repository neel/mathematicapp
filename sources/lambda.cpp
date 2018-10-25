#include "mathematica++/lambda.h"
#include "mathematica++/io.h"

mathematica::lambda::cached::cached(mathematica::connector& shell, mathematica::token::ptr expr): _shell(shell){
    _scope = _shell.begin();
    _shell.save() << expr;
    _key = _shell.last_key();
}

mathematica::lambda::cached::~cached(){
    _shell.end();
}

mathematica::lambda::cached mathematica::lambda::cache(mathematica::connector& shell) const{
    return mathematica::lambda::cached(shell, _expr);
}
