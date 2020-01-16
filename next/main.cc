
#include <cstdlib>
#include <iostream>
#include <memory>

#include "credentials.h"

#include "misc/error.h"
#include "misc/logger.h"
#include "misc/const.h"
#include "httpclient.h"
#include "togglapi.h"

#include <json/json.h>

#include <string>
#include <sstream>

#include "Poco/DeflatingStream.h"
#include "Poco/Environment.h"
#include "Poco/Exception.h"
#include "Poco/FileStream.h"
#include "Poco/InflatingStream.h"
#include "Poco/Logger.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/HTTPCredentials.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Net/PrivateKeyPassphraseHandler.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/Session.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/NumberParser.h"
#include "Poco/StreamCopier.h"
#include "Poco/TextEncoding.h"
#include "Poco/URI.h"
#include "Poco/UTF8Encoding.h"


int main(void) {
    TogglApi api("linux_native_app", TEST_USERNAME, TEST_PASSWORD);

    auto result = api.v9_status();

    std::cout << result.second << std::endl;
    std::cout << result.first.String() << std::endl;

    return EXIT_SUCCESS;
}
