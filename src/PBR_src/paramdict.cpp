#include <PBR_include/paramdict.h>
#include <PBR_include/parser.h>


namespace PBR {

    //ParameterDictionary::ParameterDictionary(ParsedParameterVector p, const RGBColorSpace* colorSpace):
    ParameterDictionary::ParameterDictionary(ParsedParameterVector p):
        params(std::move(p)) {
        nOwnedParams = params.size();
        std::reverse(params.begin(), params.end());
        //CHECK(colorSpace);
        //checkParameterTypes();
    }

    ParameterDictionary::ParameterDictionary(ParsedParameterVector p0,
        const ParsedParameterVector& params1): 
        params(std::move(p0)) {
        nOwnedParams = params.size();
        std::reverse(params.begin(), params.end());
        //CHECK(colorSpace);
        params.insert(params.end(), params1.rbegin(), params1.rend());
        //checkParameterTypes();
    }


}
