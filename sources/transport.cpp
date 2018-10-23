#include "mathematica++/transport.h"

mathematica::mtensor_adapter::mtensor_adapter(WolframLibraryData data): _data(data){}

mathematica::basic_transport::basic_transport(WMK_LINK link): _link(link), _connection(driver::link(link)), _shell(_connection){}

mathematica::value mathematica::basic_transport::recv(){
    mathematica::value val;
    _shell >> val;
    return val;
}

mathematica::transport::transport(WolframLibraryData data): _data(data), _io(data->getWSLINK(data)), _libname("LibraryFunction"){}

void mathematica::transport::set_name(const std::string& name){
    _libname = name;
}


mathematica::wtransport::wtransport(WolframLibraryData data, mathematica::transport::link_type link): transport(data), _control(link){
    mathematica::value input = _control._connection->fetch_token(&_control._shell);
    _input = boost::dynamic_pointer_cast<mathematica::tokens::function>(input);
}

mathematica::mtransport::mtransport(WolframLibraryData data, int argc, MArgument* argv, MArgument res): transport(data), _result(data, res){
    for(int i = 0; i < argc; ++i){
        argument_adapter* adapter = new argument_adapter(data, argv[i]);
        _arguments.push_back(adapter);
    }
}

mathematica::mtransport::~mtransport(){
    for(argument_adapter* adapter: _arguments){
        delete adapter;
        adapter = 0;
    }
    _arguments.clear();
}

const mathematica::argument_adapter& mathematica::mtransport::arg(std::size_t index) const{
    return *_arguments[index];
}

mathematica::argument_adapter& mathematica::mtransport::res(){
    return _result;
}

std::size_t mathematica::mtransport::nargs() const{
    return _arguments.size();
}


mathematica::initializer::initializer(WolframLibraryData data, std::string library_name): transport(data), _library_name(library_name.empty() ? std::string("LibraryFunction") : library_name){
    declare(messages::argx());
    declare(messages::type());
    declare(messages::overload());
    declare(messages::rankerror());
    declare(messages::tensortype());
}

mathematica::initializer& mathematica::initializer::declare(const mathematica::user_message& msg){
    mathematica::m reg = msg.registration(_library_name);
    *this << reg;
    ignore();
    return *this;
}

mathematica::initializer& mathematica::operator<<(mathematica::initializer& shell, const mathematica::user_message& msg){
    return shell.declare(msg);
}

mint mathematica::transport::pass(bool abort){
    try{
        throw;
    }catch(library::exceptions::internal_error& ex){
        ex.pass(*this, _libname);
        if(abort){
            _data->AbortQ();
        }
        return ex.code();
    }catch(mathematica::exceptions::error& ex){
        *this << mathematica::m("Echo")(ex.what(), _libname+"Exception");
        ignore();
        if(abort){
            _data->AbortQ();
        }
        return ex.code();
    }catch(std::exception& ex){
        *this << mathematica::m("Echo")(ex.what(), _libname+"Exception");
        ignore();
        if(abort){
            _data->AbortQ();
        }
        return LIBRARY_FUNCTION_ERROR;
    }catch(...){
        *this << mathematica::m("Echo")("Unknown uncaught exception thrown", _libname+"Exception");
        ignore();
        if(abort){
            _data->AbortQ();
        }
        return LIBRARY_FUNCTION_ERROR;
    }
}
