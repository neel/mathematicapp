#include "mathematica++/transport.h"

mathematica::mtensor_adapter::mtensor_adapter(WolframLibraryData data, MTensor tensor): _data(data), _tensor(tensor){
    _rank = _data->MTensor_getRank(_tensor);
    _type = _data->MTensor_getType(_tensor);
}

// mathematica::mtensor_adapter::collection_type mathematica::mtensor_adapter::vector() const{
//     mathematica::mtensor_adapter::collection_type collection;
//     mint const* dims = _data->MTensor_getDimensions(_tensor); // _rank number of elements in dims
//     
//     return collection;
// }

mathematica::basic_transport::basic_transport(WMK_LINK link): _link(link), _connection(driver::link(link)), _shell(_connection){
    
}

mathematica::value mathematica::basic_transport::recv(){
    mathematica::value val;
    _shell >> val;
    return val;
}

mathematica::transport::transport(WolframLibraryData data): _data(data), _io(data->getWSLINK(data)){
    
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
