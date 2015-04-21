#include <Vorb/types.h>
#include <Vorb/io/Keg.h>

struct Country {
public:
    Array<nString> provinces;
};
KEG_TYPE_DECL(Country);

KEG_TYPE_DEF(Country, Country, kt) {
    using namespace keg;
    kt.addValue("Provinces", Value::array(offsetof(Country, provinces), BasicType::STRING));
}
