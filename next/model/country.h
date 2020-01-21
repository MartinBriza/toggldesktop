#ifndef SRC_COUNTRY_H_
#define SRC_COUNTRY_H_

#include <string>

#include "Poco/Types.h"

#include "./base_model.h"

namespace toggl {

class TOGGL_INTERNAL_EXPORT CountryModel : public BaseModel {
 public:
    CountryModel()
        : BaseModel()
    , name_("") {}

    const std::string &Name() const {
        return name_;
    }
    void SetName(const std::string &value);

    const std::string &CountryCode() const {
        return country_code_;
    }
    void SetCountryCode(const std::string &value);

    // Override BaseModel
    std::string String() const override;
    std::string ModelName() const override;
    std::string ModelURL() const override;
    void LoadFromJSON(Json::Value data) override;

 private:
    std::string name_;
    std::string country_code_;
};

} // namespace toggl

#endif // SRC_COUNTRY_H_
